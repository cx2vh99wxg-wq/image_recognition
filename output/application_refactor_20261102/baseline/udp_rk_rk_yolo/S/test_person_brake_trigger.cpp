#include "person_brake_trigger.h"
#include "postprocess.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <vector>

static int failed;

static void expect(bool condition, const char *name)
{
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", name);
        failed = 1;
    }
}

/* Synthetic RKNN output uses the existing NCHW int8 layout and quantization. */
static void put_candidate(std::vector<int8_t> &tensor, int grid_len, int cell,
                          int class_id, int8_t object_score, int8_t class_score)
{
    for (int channel = 0; channel < 4; ++channel) {
        tensor[channel * grid_len + cell] = 0;
    }
    tensor[4 * grid_len + cell] = object_score;
    tensor[(5 + class_id) * grid_len + cell] = class_score;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::fprintf(stderr, "Usage: %s path/to/coco_80_labels_list.txt\n", argv[0]);
        return 2;
    }

    expect(!person_brake_should_trigger("person", nextafterf(0.15f, 0.0f)),
           "last representable value below 15% stays clear");
    expect(person_brake_should_trigger("person", 0.15f), "exactly 15% triggers");
    expect(person_brake_should_trigger("person", nextafterf(0.15f, 1.0f)),
           "first representable value above 15% triggers");
    expect(!person_brake_should_trigger("car", 0.99f), "other class stays clear");
    expect(!person_brake_should_trigger("person_extra", 0.99f), "class must match exactly");
    expect(!person_brake_should_trigger(NULL, 0.99f), "null class stays clear");
    expect(!person_brake_should_trigger("person", NAN), "NaN stays clear");
    expect(!person_brake_should_trigger("person", INFINITY), "infinity stays clear");

    expect(initPostProcess(argv[1]) == 0, "load actual COCO label file");

    const int grid0 = 80 * 80;
    std::vector<int8_t> tensor0(PROP_BOX_SIZE * 3 * grid0, -128);
    std::vector<int8_t> tensor1(PROP_BOX_SIZE * 3 * 40 * 40, -128);
    std::vector<int8_t> tensor2(PROP_BOX_SIZE * 3 * 20 * 20, -128);
    BOX_RECT pads = {0, 0, 0, 0};
    detect_result_group_t result;
    std::memset(&result, 0x55, sizeof(result));

    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "empty frame decodes");
    expect(result.count == 0 && result.person_brake_count == 0,
           "empty frame clears display and brake summary");

    /* Fill every display slot with car; the later person must still brake. */
    for (int i = 0; i < OBJ_NUMB_MAX_SIZE; ++i) {
        put_candidate(tensor0, grid0, i, 2, 127, 127);
    }
    put_candidate(tensor0, grid0, OBJ_NUMB_MAX_SIZE, 0, 127, 64);
    put_candidate(tensor0, grid0, OBJ_NUMB_MAX_SIZE + 1, 0, 127, 100);
    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "frame exceeding display cap decodes");
    expect(result.count == OBJ_NUMB_MAX_SIZE, "display keeps original 64-result cap");
    bool display_has_person = false;
    for (int i = 0; i < result.count; ++i) {
        display_has_person |= std::strcmp(result.results[i].name, "person") == 0;
    }
    expect(!display_has_person, "display scan order remains unchanged");
    expect(result.person_brake_count == 2, "person beyond display cap still triggers");
    expect(std::strcmp(result.person_brake_result.name, "person") == 0 &&
           result.person_brake_result.prop > 0.89f,
           "brake box belongs to the highest-confidence person");

    std::fill(tensor0.begin(), tensor0.end(), -128);
    for (int i = 0; i < OBJ_NUMB_MAX_SIZE; ++i) {
        put_candidate(tensor0, grid0, i, 0, 127, -89);  // Brake-only candidates.
        put_candidate(tensor0, grid0, OBJ_NUMB_MAX_SIZE + i, 2, 127, 127);
    }
    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "brake-only candidates before visible candidates decode");
    expect(result.count == OBJ_NUMB_MAX_SIZE &&
           std::strcmp(result.results[0].name, "car") == 0 &&
           std::strcmp(result.results[OBJ_NUMB_MAX_SIZE - 1].name, "car") == 0,
           "brake-only candidates do not consume any of the 64 display slots");
    expect(result.person_brake_count == OBJ_NUMB_MAX_SIZE,
           "all brake-only candidates contribute to the summary");

    std::fill(tensor0.begin(), tensor0.end(), -128);
    put_candidate(tensor0, grid0, 0, 0, -39, -39);  // (89/255)^2 < 0.15
    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "low-confidence person frame decodes");
    expect(result.count == 1 && result.person_brake_count == 0,
           "display can show a person below 15% without triggering brake");

    std::fill(tensor0.begin(), tensor0.end(), -128);
    put_candidate(tensor0, grid0, 0, 0, 25, 25);  // 0.60 * 0.60 = 0.36
    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "joint confidence frame decodes");
    expect(result.count == 1 && result.person_brake_count == 1,
           "threshold uses objectness times class probability");

    std::fill(tensor0.begin(), tensor0.end(), -128);
    put_candidate(tensor0, grid0, 0, 0, 127, -89);  // 39/255 > 0.15, class < 0.25
    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "person below original class display gate decodes");
    expect(result.count == 0 && result.person_brake_count == 1,
           "class factor just above 15% triggers without changing display gate");

    std::fill(tensor0.begin(), tensor0.end(), -128);
    put_candidate(tensor0, grid0, 0, 0, -89, 127);  // objectness < 0.25
    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "person below original objectness display gate decodes");
    expect(result.count == 0 && result.person_brake_count == 1,
           "objectness factor just above 15% triggers without changing display gate");

    std::fill(tensor0.begin(), tensor0.end(), -128);
    put_candidate(tensor0, grid0, 0, 0, 127, -90);  // 38/255 < 0.15
    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "person below 15% decodes");
    expect(result.person_brake_count == 0, "quantized confidence below 15% stays clear");

    std::fill(tensor0.begin(), tensor0.end(), -128);
    put_candidate(tensor0, grid0, 0, 0, 127, 76);  // 204/255 = 0.80
    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "80% person frame decodes");
    expect(result.count == 1 && result.person_brake_count == 1 &&
           result.results[0].prop == result.person_brake_result.prop,
           "80% drawing score and brake score are identical and trigger");

    std::fill(tensor0.begin(), tensor0.end(), -128);
    put_candidate(tensor0, grid0, 0, 2, 127, 76);
    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "80% nonperson frame decodes");
    expect(result.count == 1 && result.person_brake_count == 0,
           "high confidence other class cannot trigger person brake");

    std::fill(tensor0.begin(), tensor0.end(), -128);
    put_candidate(tensor1, 40 * 40, 0, 0, 127, 64);
    put_candidate(tensor2, 20 * 20, 0, 0, 127, 127);
    expect(post_process_simple(tensor0.data(), tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) == 0,
           "later output layers decode");
    expect(result.person_brake_count == 2 && result.person_brake_result.prop > 0.99f,
           "all three model outputs contribute to the summary");

    std::memset(&result, 0x55, sizeof(result));
    expect(post_process_simple(NULL, tensor1.data(), tensor2.data(),
                               640, 640, 0.25f, 0.45f, pads, 1.0f, 1.0f, &result) != 0,
           "invalid output reports failure");
    expect(result.count == 0 && result.person_brake_count == 0,
           "failure cannot leave a stale brake summary");

    deinitPostProcess();
    if (failed) return 1;
    std::puts("person brake threshold and full-proposal summary tests passed");
    return 0;
}
