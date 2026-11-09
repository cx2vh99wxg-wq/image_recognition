# =============================================================================
# 说明：X11六摄像头渲染程序 Makefile
# =============================================================================

# 项目设置
TARGET = eight_camera_renderer
RUN_ARGS ?=
PREVIEW_OUTPUT ?= /tmp/perception-dashboard.ppm
CXX = g++
CC = gcc

# 工程路径约定
BUILD_DIR = build
SRC_DIR = .
INC_DIR = include

# 参与构建的文件
CXXSRCS = $(wildcard $(SRC_DIR)/*.cpp)
CSRCS = ../../csrc/shared_memory.c  # 说明：添加共享内存源文件
INCLUDES = $(wildcard $(INC_DIR)/*.h) $(wildcard ../../include/*.h)  # 说明：添加上级头文件
CXX_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CXXSRCS))
C_OBJECTS = $(patsubst ../../csrc/%.c,$(BUILD_DIR)/%.o,$(CSRCS))
OBJECTS = $(CXX_OBJECTS) $(C_OBJECTS)
BIN = $(BUILD_DIR)/$(TARGET)

# 构建标志设置
INCFLAGS = -I../../include -I$(INC_DIR)

# 获取X11库的构建标志和链接库
X11_LIBS = -lX11 -lXext

# 构建和链接标志
# 说明：-O3: 最高优化级别
# 说明：-march=native: 使用本机CPU指令集优化
# 说明：-funroll-loops: 循环展开优化
# 说明：-pthread: 线程支持
CXXFLAGS = $(INCFLAGS) -std=c++14 -O3 -march=native -funroll-loops -pthread
CFLAGS = $(INCFLAGS) -std=c99 -O3 -march=native -funroll-loops -pthread
LDFLAGS = $(X11_LIBS) -lrt -lpthread

# 说明：XRender is optional; plain X11 and MIT-SHM remain available fallbacks.
XRENDER_LIBS := $(shell pkg-config --libs xrender 2>/dev/null)
ifneq ($(strip $(XRENDER_LIBS)),)
CXXFLAGS += -DPERCEPTION_HAVE_XRENDER=1 $(shell pkg-config --cflags xrender 2>/dev/null)
LDFLAGS += $(XRENDER_LIBS)
endif

# 说明：Prefer the direct GPU presenter when EGL/GLES2 development libraries exist.
# 说明：The renderer retains XRender, MIT-SHM and XPutImage runtime fallbacks.
GLES_LIBS := $(shell pkg-config --libs egl glesv2 2>/dev/null)
ifneq ($(strip $(GLES_LIBS)),)
CXXFLAGS += -DPERCEPTION_HAVE_GLES=1 $(shell pkg-config --cflags egl glesv2 2>/dev/null)
LDFLAGS += $(GLES_LIBS)
endif

# =============================================================================
# 说明：构建规则
# =============================================================================

.PHONY: all clean help run preview test-fps test-speed-warning test-six-camera

# 直接调用 make 时采用的入口
all: test-camera-guard $(BIN)

.PHONY: test-camera-guard
$(BUILD_DIR)/test_camera_guard_rows: tests/test_camera_guard_rows.cpp ../../../common/image/camera_valid_rows.h include/six_camera_render.h ../../../common/image/frame_pixels.h | $(BUILD_DIR)
	$(CXX) -std=c++14 -O2 -Wall -Wextra -Werror $(INCFLAGS) $< -o $@

test-camera-guard: $(BUILD_DIR)/test_camera_guard_rows
	./$(BUILD_DIR)/test_camera_guard_rows

$(BUILD_DIR)/test_dashboard_ui_cache: tests/test_dashboard_ui_cache.cpp include/dashboard_ui.h include/six_camera_render.h include/speed_warning_glyphs.h ../../include/camera_views.h | $(BUILD_DIR)
	$(CXX) -std=c++14 -O2 -Wall -Wextra -Werror $(INCFLAGS) $< -o $@

.PHONY: test-dashboard-ui-cache
test-dashboard-ui-cache: $(BUILD_DIR)/test_dashboard_ui_cache
	./$(BUILD_DIR)/test_dashboard_ui_cache

$(BUILD_DIR)/test_lane_angle_cue: tests/test_lane_angle_cue.cpp include/lane_angle_cue.h include/dashboard_ui.h | $(BUILD_DIR)
	$(CXX) -std=c++14 -O2 -Wall -Wextra -Werror $(INCFLAGS) $< -o $@

.PHONY: test-lane-angle
test-lane-angle: $(BUILD_DIR)/test_lane_angle_cue
	./$(BUILD_DIR)/test_lane_angle_cue

$(BUILD_DIR)/test_six_camera: tests/test_six_camera.cpp include/six_camera_render.h ../../include/camera_views.h | $(BUILD_DIR)
	$(CXX) -std=c++14 -O2 -Wall -Wextra -Werror $(INCFLAGS) $< -o $@

test-six-camera: $(BUILD_DIR)/test_six_camera
	"$(abspath $(BUILD_DIR)/test_six_camera)"

$(BUILD_DIR)/test_camera_integration: tests/test_camera_integration.cpp main.cpp $(INCLUDES) $(BUILD_DIR)/shared_memory.o | $(BUILD_DIR)
	$(CXX) -std=c++14 -O2 -Wall $(INCFLAGS) $< $(BUILD_DIR)/shared_memory.o $(X11_LIBS) -lrt -lpthread -o $@

$(BUILD_DIR)/test_video_input_fps: tests/test_video_input_fps.cpp include/video_input_fps.h | $(BUILD_DIR)
	$(CXX) -std=c++14 -Wall -Wextra -Werror -Iinclude $< -o $@

test-fps: $(BUILD_DIR)/test_video_input_fps
	"$(abspath $(BUILD_DIR)/test_video_input_fps)"

$(BUILD_DIR)/test_speed_warning: tests/test_speed_warning.cpp include/dashboard_ui.h include/speed_warning_glyphs.h | $(BUILD_DIR)
	$(CXX) -std=c++14 -O2 -Wall -Wextra -Iinclude $< -o $@

test-speed-warning: $(BUILD_DIR)/test_speed_warning
	"$(abspath $(BUILD_DIR)/test_speed_warning)"
	VISION_DASHBOARD_FAST=1 "$(abspath $(BUILD_DIR)/test_speed_warning)"

# 链接程序文件
$(BIN): $(OBJECTS) | $(BUILD_DIR)
	@echo "链接程序文件: $@"
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "构建结束: $(TARGET) - Perception Engine 全景界面"

# 构建C++源文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES) | $(BUILD_DIR)
	@echo "构建C++: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 构建C源文件 (用于共享内存模块)
$(BUILD_DIR)/%.o: ../../csrc/%.c $(INCLUDES) ../../../common/perception/include/curve_shm_attach.h | $(BUILD_DIR)
	@echo "构建共享内存C文件: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# 构建C源文件 (本地文件)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDES) | $(BUILD_DIR)
	@echo "构建C: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# 建立产物目录
$(BUILD_DIR):
	@echo "建立产物目录: $(BUILD_DIR)"
	mkdir -p $(BUILD_DIR)

# 运行应用
run: $(BIN)
	@echo "说明：运行X11六摄像头渲染程序..."
	./$(BIN) $(RUN_ARGS)

# 使用已保存的视频帧离线生成真实界面，无需接入采集/车控进程。
preview: $(BIN)
	./$(BIN) --preview "$(PREVIEW_OUTPUT)"

# 移除构建文件
clean:
	@echo "移除构建文件..."
	rm -rf $(BUILD_DIR)
	@echo "移除完成"

# 呈现用法信息
help:
	@echo "=== 说明：X11六摄像头渲染程序构建系统 ==="
	@echo "说明：可用目标："
	@echo "  all     - 构建程序 (默认)"
	@echo "  clean   - 移除构建文件"
	@echo "  run     - 构建并运行应用"
	@echo "  说明：preview - 使用 combined_last_frame.ppm 导出完整界面预览"
	@echo "  help    - 呈现此用法信息"
	@echo ""
	@echo "说明：程序功能："
	@echo "  - 读取PCIe和UDP共享内存"
	@echo "  - 六个物理摄像头: 22/33/44后左/后右/后中, 66/77/88前右/前左/前中"
	@echo "  - 0x99: 六路3x2视频 + 独立车辆/报警面板"
	@echo "  - 响应FSPI按钮控制的视角切换"
	@echo "  - 单摄像头320x240放大到640x480呈现"
	@echo "  - X11实时呈现"
	@echo "  - 默认全屏；F11/F切换全屏，Q退出，Esc退出全屏"
	@echo "  - make run RUN_ARGS=--windowed 以窗口模式运行"
	@echo ""
	@echo "说明：依赖："
	@echo "  - X11开发库"
	@echo "  - 共享内存库"

# 说明：调试信息
debug-info:
	@echo "=== 构建设置信息 ==="
	@echo "说明：目标程序: $(TARGET)"
	@echo "C++构建器: $(CXX)"
	@echo "C构建器: $(CC)"
	@echo "说明：C++源文件: $(CXXSRCS)"
	@echo "说明：C源文件: $(CSRCS)"
	@echo "说明：目标文件: $(OBJECTS)"
	@echo "说明：包含路径: $(INCFLAGS)"
	@echo "C++构建标志: $(CXXFLAGS)"
	@echo "C构建标志: $(CFLAGS)"
	@echo "说明：链接标志: $(LDFLAGS)"
	@echo "说明：共享库: $(SHARED_LIB)"

.PHONY: test-dashboard-cache
$(BUILD_DIR)/test_dashboard_cache: tests/test_dashboard_cache.cpp include/dashboard_ui.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@
test-dashboard-cache: $(BUILD_DIR)/test_dashboard_cache
	VISION_DASHBOARD_FAST=0 ./$(BUILD_DIR)/test_dashboard_cache > $(BUILD_DIR)/dashboard-full.hashes
	VISION_DASHBOARD_FAST=1 ./$(BUILD_DIR)/test_dashboard_cache > $(BUILD_DIR)/dashboard-fast.hashes
	cmp $(BUILD_DIR)/dashboard-full.hashes $(BUILD_DIR)/dashboard-fast.hashes

.PHONY: test-vehicle-artwork
$(BUILD_DIR)/test_vehicle_artwork: tests/test_vehicle_artwork.cpp include/vehicle_artwork.h | $(BUILD_DIR)
	$(CXX) -std=c++14 -O2 -Wall -Wextra -Werror -Iinclude $< -o $@
test-vehicle-artwork: $(BUILD_DIR)/test_vehicle_artwork
	./$(BUILD_DIR)/test_vehicle_artwork

.PHONY: test-pedestrian-scene
$(BUILD_DIR)/test_pedestrian_scene: tests/test_pedestrian_scene.cpp include/pedestrian_scene.h include/lane_angle_cue.h include/vehicle_artwork.h ../../../common/perception/include/display_people.h ../../../common/perception/include/display_people_wire.h ../..//pcie/include/person_scene_snapshot.h | $(BUILD_DIR)
	$(CXX) -std=c++14 -O2 -Wall -Wextra -Werror $(INCFLAGS) $< -o $@
test-pedestrian-scene: $(BUILD_DIR)/test_pedestrian_scene
	./$(BUILD_DIR)/test_pedestrian_scene

$(BUILD_DIR)/main.o: include/pedestrian_scene.h include/lane_angle_cue.h include/vehicle_artwork.h ../../../common/perception/include/display_people.h

.PHONY: test-front-brake-cue
$(BUILD_DIR)/test_front_brake_cue: tests/test_front_brake_cue.cpp include/front_brake_cue.h include/lane_angle_cue.h ../../include/front_person_guard.h | $(BUILD_DIR)
	$(CXX) -std=c++14 -O2 -Wall -Wextra -Werror $(INCFLAGS) $< -o $@
test-front-brake-cue: $(BUILD_DIR)/test_front_brake_cue
	./$(BUILD_DIR)/test_front_brake_cue
$(BUILD_DIR)/main.o: include/front_brake_cue.h ../../include/front_person_guard.h
$(BUILD_DIR)/main.o: ../../../common/performance/frame_intervals.h

.PHONY: test-frame-intervals
$(BUILD_DIR)/test_frame_intervals: tests/test_frame_intervals.cpp ../../../common/performance/frame_intervals.h | $(BUILD_DIR)
	$(CXX) -std=c++14 -O2 -Wall -Wextra -Werror $(INCFLAGS) $< -o $@
test-frame-intervals: $(BUILD_DIR)/test_frame_intervals
	./$(BUILD_DIR)/test_frame_intervals
