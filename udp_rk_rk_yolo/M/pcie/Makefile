# 说明：Makefile for PCIe Capture Module
# Maintainer: 科研轮转
# 本版更新：2026年10月4日

#############################################################################
# 目标、源码及产物路径
#############################################################################
TARGET = pcie_capture
TOPDIR ?= $(abspath ..)
BUILD_DIR = build
SRC_DIR = csrc
INC_DIR = include
LIB_DIR = lib
MODEL_DIR = model

# 选择 C 和 C++ 工具链
CC = gcc
CXX = g++
STRIP = strip

# 传给编译器的参数
CFLAGS = -std=c99 -Wall -O3 -g -MMD -MP
CXXFLAGS = -std=c++11 -Wall -O3 -g -MMD -MP

# 说明：渲染功能控制
ENABLE_RENDERING ?= 1
ifeq ($(ENABLE_RENDERING), 1)
    CFLAGS += -DENABLE_RENDERING=1
    CXXFLAGS += -DENABLE_RENDERING=1
else
    CFLAGS += -DENABLE_RENDERING=0
    CXXFLAGS += -DENABLE_RENDERING=0
endif

#############################################################################
# 头文件搜索范围
#############################################################################
INCLUDES = -I$(INC_DIR) -I../include

# 说明：如果从顶层构建，包含顶层的include目录
ifdef TOPDIR
    INCLUDES += -I$(TOPDIR)/include
endif

# 说明：X11支持（仅在启用渲染时）
ifeq ($(ENABLE_RENDERING), 1)
    INCLUDES += -I/usr/include/X11
endif

#############################################################################
# 说明：库路径和链接库
#############################################################################
LIBS = -L$(LIB_DIR) -L../lib
LDFLAGS = -pthread -lm -ldl

# RKNN库 - 总是链接 (如果找不到会给出警告但继续构建)
LIBS += -lrknnrt

# 说明：X11库（仅在启用渲染时）
ifeq ($(ENABLE_RENDERING), 1)
    LDFLAGS += -lX11 -lXext
endif

#############################################################################
# 选择需要编译的源码
#############################################################################
# 说明：基础源文件
C_SRCS_BASE = $(SRC_DIR)/fspi_module.c $(SRC_DIR)/preprocess.c
CXX_SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# 说明：总是包含YOLO功能
C_SRCS = $(C_SRCS_BASE) $(SRC_DIR)/yolo_integration.c

C_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRCS))
CXX_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CXX_SRCS))

# 说明：如果从顶层构建，添加顶层的共享内存对象文件
ifdef TOPDIR
    SHARED_MEMORY_OBJ = $(TOPDIR)/build/shared_memory.o
    OBJS = $(C_OBJS) $(CXX_OBJS) $(SHARED_MEMORY_OBJ)
else
    OBJS = $(C_OBJS) $(CXX_OBJS)
endif

#############################################################################
# 直接调用 make 时采用的入口
#############################################################################
.PHONY: all clean install debug release info

all: info $(BUILD_DIR)/$(TARGET)

.PHONY: test-pixels
$(BUILD_DIR)/test_frame_pixels: ../../common/image/tests/test_frame_pixels.cpp ../../common/image/frame_pixels.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror $< -o $@

test-pixels: $(BUILD_DIR)/test_frame_pixels
	./$(BUILD_DIR)/test_frame_pixels

# 说明：Offline classification checks/replay never open PCIe or vehicle interfaces.
$(BUILD_DIR)/test_line_style: tests/test_line_style.cpp include/line_style_analyzer.h include/ego_lane_selector.h include/line_style_trace.h ../include/line_style.h ../include/line_style_wire.h ../include/line_style_ipc.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 $(INCLUDES) $< -o $@

test-line-style: $(BUILD_DIR)/test_line_style
	./$(BUILD_DIR)/test_line_style

$(BUILD_DIR)/test_ego_lane: tests/test_ego_lane.cpp include/line_style_analyzer.h include/ego_lane_selector.h include/line_style_trace.h ../include/line_style.h ../include/line_style_wire.h ../include/line_style_ipc.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 $(INCLUDES) $< -o $@

test-ego-lane: $(BUILD_DIR)/test_ego_lane
	./$(BUILD_DIR)/test_ego_lane

$(BUILD_DIR)/test_front_radial: tests/test_front_radial.cpp include/front_camera_layout.h include/front_radial.h include/front_radial_preview.h include/lane_curve_geometry.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 $(INCLUDES) $< -o $@

.PHONY: test-front-radial
test-front-radial: $(BUILD_DIR)/test_front_radial
	./$(BUILD_DIR)/test_front_radial

$(BUILD_DIR)/test_front_ipm: tests/test_front_ipm.cpp include/front_radial_preview.h include/ground_projection.h include/ego_lane_selector.h include/lane_curve_geometry.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 $(INCLUDES) $< -o $@

.PHONY: test-front-ipm
test-front-ipm: $(BUILD_DIR)/test_front_ipm
	./$(BUILD_DIR)/test_front_ipm

$(BUILD_DIR)/test_lane_curve_geometry: tests/test_lane_curve_geometry.cpp include/lane_curve_geometry.h include/front_radial.h include/ground_projection.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 $(INCLUDES) $< -o $@

test-lane-curve-geometry: $(BUILD_DIR)/test_lane_curve_geometry
	./$(BUILD_DIR)/test_lane_curve_geometry

$(BUILD_DIR)/front_geometry_replay: tools/front_geometry_replay.cpp include/front_radial.h include/ground_projection.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 $(INCLUDES) $< -o $@

front-geometry-replay: $(BUILD_DIR)/front_geometry_replay

$(BUILD_DIR)/line_style_replay: tools/line_style_replay.cpp include/line_style_analyzer.h include/ego_lane_selector.h include/line_style_trace.h ../include/line_style.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 $(INCLUDES) $< -o $@

line-style-replay: $(BUILD_DIR)/line_style_replay

$(BUILD_DIR)/test_traffic: tests/test_traffic.cpp include/traffic_vision.h ../include/traffic_light.h ../include/traffic_light_ipc.h ../include/traffic_light_wire.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 $(INCLUDES) $< -o $@

test-traffic: $(BUILD_DIR)/test_traffic
	./$(BUILD_DIR)/test_traffic

$(BUILD_DIR)/test_npu_scheduling: tests/test_npu_scheduling.cpp include/latest_lane_worker.h include/traffic_vision.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 -pthread $(INCLUDES) $< -o $@

test-npu-scheduling: $(BUILD_DIR)/test_npu_scheduling
	./$(BUILD_DIR)/test_npu_scheduling

$(BUILD_DIR)/traffic_probe: tools/traffic_probe.cpp $(BUILD_DIR)/traffic_detector.o include/traffic_detector.h include/traffic_vision.h include/traffic_offline_guard.h ../include/traffic_light.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(BUILD_DIR)/traffic_detector.o -o $@ $(LIBS) -lm -pthread

traffic-probe: $(BUILD_DIR)/traffic_probe

$(BUILD_DIR)/traffic_replay: tools/traffic_replay.cpp $(BUILD_DIR)/traffic_detector.o $(BUILD_DIR)/traffic_service.o include/traffic_offline_guard.h include/traffic_service.h include/traffic_vision.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(BUILD_DIR)/traffic_detector.o $(BUILD_DIR)/traffic_service.o -o $@ $(LIBS) -lm -pthread

traffic-replay: $(BUILD_DIR)/traffic_replay

# 说明：One saved image through the real RKNN model. No capture/control publication.
$(BUILD_DIR)/line_style_model_probe: tools/line_style_model_probe.cpp include/line_style_analyzer.h include/ego_lane_selector.h include/line_style_trace.h ../include/line_style.h $(BUILD_DIR)/yolo_integration.o $(BUILD_DIR)/preprocess.o $(SHARED_MEMORY_OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(BUILD_DIR)/yolo_integration.o $(BUILD_DIR)/preprocess.o $(SHARED_MEMORY_OBJ) -o $@ $(LIBS) -lm -pthread -lrt -ldl

line-style-model-probe: $(BUILD_DIR)/line_style_model_probe

#############################################################################
# 构建信息
#############################################################################
info:
	@echo "======================================"
	@echo "PCIe Capture Module Build Console"
	@echo "======================================"
	@echo "Build target: $(TARGET)"
	@echo "Build output: $(BUILD_DIR)"
	@echo "Renderer option: $(ENABLE_RENDERING)"
	@echo "YOLO detector: on"
	@echo "Toolchain: $(CC) / $(CXX)"
	@echo "======================================"

#############################################################################
# 说明：主目标
#############################################################################
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "Writing executable: $@"
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJS) -o $@ $(LIBS) $(LDFLAGS)
	@echo "Build ready: $@"

# 说明：Also support invoking make directly from M/pcie.
$(SHARED_MEMORY_OBJ): $(TOPDIR)/csrc/shared_memory.c $(TOPDIR)/include/shared_memory.h $(TOPDIR)/../common/perception/include/curve_shm_attach.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

#############################################################################
# C源文件构建
#############################################################################
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Translating C: $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

#############################################################################
# C++源文件构建
#############################################################################
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Translating C++: $<"
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

#############################################################################
# 说明：构建模式
#############################################################################
debug: CFLAGS += -DDEBUG -g -O0
debug: CXXFLAGS += -DDEBUG -g -O0
debug: $(BUILD_DIR)/$(TARGET)

release: CFLAGS += -DNDEBUG -O3
release: CXXFLAGS += -DNDEBUG -O3
release: $(BUILD_DIR)/$(TARGET)
	$(STRIP) $(BUILD_DIR)/$(TARGET)

#############################################################################
# 将产物放入安装路径
#############################################################################
install: $(BUILD_DIR)/$(TARGET)
	@echo "Setting up $(TARGET)..."
	@mkdir -p $(BUILD_DIR)/bin
	@cp $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/bin/
	@cp -r $(MODEL_DIR) $(BUILD_DIR)/ 2>/dev/null || true
	@echo "Installation finished"

#############################################################################
# 回收编译产物
#############################################################################
clean:
	@echo "Removing build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

#############################################################################
# 用法
#############################################################################
help:
	@echo "Available targets:"
	@echo "  all           - Build the project (default)"
	@echo "  debug         - Build debug version"
	@echo "  release       - Build optimized release version"
	@echo "  install       - Install binary and models"
	@echo "  clean         - Clean build directory"
	@echo "  help          - Show this help"
	@echo ""
	@echo "Options:"
	@echo "  ENABLE_RENDERING=1   - Enable rendering support (default)"
	@echo "  ENABLE_RENDERING=0   - Disable rendering support"
	@echo ""
	@echo "Examples:"
	@echo "  make                    - Build with rendering"
	@echo "  make ENABLE_RENDERING=0 - Build without rendering"
	@echo "  make debug              - Build debug version"
	@echo "  make release            - Build release version"

#############################################################################
# 跟踪头文件变动
#############################################################################
-include $(OBJS:.o=.d)

# 说明：生成依赖文件
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) -MM $(CFLAGS) $(INCLUDES) $< | sed 's,\($*\.o\)[ :]*,$(BUILD_DIR)/\1 $@ : ,g' > $@

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@$(CXX) -MM $(CXXFLAGS) $(INCLUDES) $< | sed 's,\($*\.o\)[ :]*,$(BUILD_DIR)/\1 $@ : ,g' > $@

.PHONY: test-marking-profile
$(BUILD_DIR)/test_marking_profile: tests/test_marking_profile.cpp include/lane_marking_profile.h include/ego_lane_selector.h include/line_style_analyzer.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror $(INCLUDES) $< -o $@
test-marking-profile: $(BUILD_DIR)/test_marking_profile
	./$(BUILD_DIR)/test_marking_profile

.PHONY: test-front-person
build/test_front_person_detector: tests/test_front_person_detector.cpp csrc/traffic_detector.cpp include/traffic_detector.h ../include/front_person.h
	@mkdir -p build
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror $(INCLUDES) tests/test_front_person_detector.cpp csrc/traffic_detector.cpp -o $@
test-front-person: build/test_front_person_detector
	./build/test_front_person_detector
