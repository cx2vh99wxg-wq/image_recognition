#define main renderer_main
#include "../udp_rk_rk_yolo/S/x11/csrc/main.cpp"
#undef main
#include <cassert>
int main(){start_fullscreen=0;assert(!load_car_png("../car.png"));EmergencyBrakeSharedMemory state={};state.valid=1;emergency_brake_shm=&state;RenderEngine e={};assert(!render_engine_init(&e));assert(!present_dashboard(&e));state.person_detected=1;state.box_right=120;state.box_bottom=180;std::fill_n((uint32_t*)e.display_buffer,1280*480,0xff3278b9);XSetForeground(e.display,e.gc,0x111111);assert(present_dashboard(&e)==-1);assert(!e.shm_pending);render_engine_cleanup(&e);emergency_brake_shm=nullptr;stbi_image_free(car_png_data);car_png_data=nullptr;puts("MULTI_REGION_MIDDLE_FAILURE_SAFE_CLEANUP_PASS");}
