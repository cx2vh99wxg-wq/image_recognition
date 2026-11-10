# 说明：PCIE + YOLO + UDP 整合项目 Makefile
# 构建PCIE视频流、YOLO目标识别和UDP网络传输整合系统

# 选择 C 和 C++ 工具链
CC = gcc
CXX = g++

# 说明：模型路径定义
# 构建时使用（相对于pcie目录，因为主Makefile在pcie目录下运行应用）
MODEL_PATH_COMPILE = "model/RK3566_RK3568/yolov5s-640-640.rknn"
LABELS_PATH_COMPILE = "model/coco_80_labels_list.txt"

# 构建选项 (高性能优化)
CFLAGS = -Wall -O3 -march=native -mtune=cortex-a55 -std=c11 -DYOLO_MODEL_PATH=\"$(MODEL_PATH_COMPILE)\" -DYOLO_LABELS_PATH=\"$(LABELS_PATH_COMPILE)\" -MMD -MP
CXXFLAGS = -Wall -O3 -march=native -mtune=cortex-a55 -std=c++11 -DYOLO_MODEL_PATH=\"$(MODEL_PATH_COMPILE)\" -DYOLO_LABELS_PATH=\"$(LABELS_PATH_COMPILE)\" -MMD -MP

# 工程目录
SRCDIR = csrc
INCDIR = include
BUILDDIR = build

# 说明：目标程序名
TARGET = $(BUILDDIR)/app

# 选择需要编译的源码
C_SOURCES = $(SRCDIR)/yolo_integration.c \
            $(SRCDIR)/preprocess.c \
            $(SRCDIR)/person_alert_effects.c

CXX_SOURCES = $(SRCDIR)/main.cpp \
              $(SRCDIR)/postprocess.cpp

# 说明：对象文件
C_OBJECTS = $(C_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
CXX_OBJECTS = $(CXX_SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
OBJECTS = $(C_OBJECTS) $(CXX_OBJECTS)

# 头文件搜索范围
INCLUDES = -I$(INCDIR) \
           -I../include \
           -I../rknn_yolov5_demo/include

# 说明：库路径
LIBDIRS = -L./lib \
          -L../build

# 说明：链接库
LIBS = -lrknnrt \
       -lshared \
       -lpthread \
       -lm \
       -lrt

# 各类输入的产物生成步骤
.PHONY: all clean

all: $(TARGET)

.PHONY: test-pixels
$(BUILDDIR)/test_frame_pixels: ../../common/image/tests/test_frame_pixels.cpp ../../common/image/frame_pixels.h $(INCDIR)/camera_orientation.h | $(BUILDDIR)
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -DTEST_S_ORIENTATION=1 $< -o $@

test-pixels: $(BUILDDIR)/test_frame_pixels
	./$(BUILDDIR)/test_frame_pixels

.PHONY: test-camera-capture-edges test-person-alert-cache
$(BUILDDIR)/test_camera_capture_edges: tests/test_camera_capture_edges.cpp include/camera_capture_edges.h include/camera_orientation.h | $(BUILDDIR)
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror $< -o $@
test-camera-capture-edges: $(BUILDDIR)/test_camera_capture_edges
	./$(BUILDDIR)/test_camera_capture_edges

ALERT_TEST_FLAGS = -std=c11 -O3 -march=native -mtune=cortex-a55 -Wall -Wextra -Werror $(INCLUDES) -D_POSIX_C_SOURCE=200809L -Dgettimeofday=test_gettimeofday
ALERT_REFERENCE_SYMBOLS = g_alert_state alert_effects_init get_current_time_ms update_blink_state calculate_fade_info blend_colors draw_person_alert_effect apply_person_alert_effects
$(BUILDDIR)/reference_alerts.o: tests/fixtures/person_alert_effects_reference.c include/person_alert_effects.h | $(BUILDDIR)
	$(CC) $(ALERT_TEST_FLAGS) $(foreach symbol,$(ALERT_REFERENCE_SYMBOLS),-D$(symbol)=reference_$(symbol)) -c $< -o $@
$(BUILDDIR)/test_cached_alerts.o: csrc/person_alert_effects.c include/person_alert_effects.h | $(BUILDDIR)
	$(CC) $(ALERT_TEST_FLAGS) -DPERSON_ALERT_CACHE_TEST -c $< -o $@
$(BUILDDIR)/test_person_alert_cache: tests/test_person_alert_cache.c $(BUILDDIR)/reference_alerts.o $(BUILDDIR)/test_cached_alerts.o
	$(CC) -std=c11 -O3 -Wall -Wextra -Werror -D_POSIX_C_SOURCE=200809L $(INCLUDES) $^ -lm -pthread -o $@
test-person-alert-cache: $(BUILDDIR)/test_person_alert_cache
	./$(BUILDDIR)/test_person_alert_cache

.PHONY: test-detection-layers
$(BUILDDIR)/test_detection_layers.o: tests/test_detection_layers.c | $(BUILDDIR)
	$(CC) -std=c11 -O3 $(INCLUDES) -c $< -o $@
$(BUILDDIR)/test_detection_layers: $(BUILDDIR)/test_detection_layers.o $(BUILDDIR)/yolo_integration.o $(BUILDDIR)/preprocess.o $(BUILDDIR)/postprocess.o $(BUILDDIR)/test_cached_alerts.o
	$(CXX) $^ $(LIBDIRS) $(LIBS) -o $@
test-detection-layers: $(BUILDDIR)/test_detection_layers
	./$(BUILDDIR)/test_detection_layers

$(TARGET): $(OBJECTS) | $(BUILDDIR)
	@echo "生成目标程序文件: $@"
	$(CXX) $(OBJECTS) $(LIBDIRS) $(LIBS) -o $@
	@echo "构建结束!"

# C源文件构建规则
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@echo "构建C文件: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# C++源文件构建规则
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	@echo "构建C++文件: $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/main.o: ../../common/image/frame_pixels.h $(INCDIR)/camera_orientation.h $(INCDIR)/yolo_integration.h \
                   $(INCDIR)/postprocess.h $(INCDIR)/latest_detection.h ../include/person_brake_trigger.h
$(BUILDDIR)/postprocess.o: $(INCDIR)/postprocess.h ../include/person_brake_trigger.h
$(BUILDDIR)/yolo_integration.o: ../../common/image/frame_pixels.h $(INCDIR)/yolo_integration.h $(INCDIR)/postprocess.h
$(BUILDDIR)/person_alert_effects.o: $(INCDIR)/person_alert_effects.h $(INCDIR)/postprocess.h

# 建立build目录
$(BUILDDIR):
	@echo "建立build目录"
	mkdir -p $(BUILDDIR)

.PHONY: test-latest-detection
$(BUILDDIR)/test_latest_detection: tests/test_latest_detection.cpp $(INCDIR)/latest_detection.h $(INCDIR)/postprocess.h | $(BUILDDIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 -pthread $(INCLUDES) $< -o $@

test-latest-detection: $(BUILDDIR)/test_latest_detection
	./$(BUILDDIR)/test_latest_detection

$(BUILDDIR)/test_person_display: tests/test_person_display.cpp include/person_display_candidates.h include/person_scene_snapshot.h include/postprocess.h | $(BUILDDIR)
	$(CXX) -std=c++11 -Wall -Wextra -Werror -O2 $(INCLUDES) $< -o $@
test-person-display: $(BUILDDIR)/test_person_display
	./$(BUILDDIR)/test_person_display

# 回收编译产物
clean:
	@echo "移除构建文件"
	rm -rf $(BUILDDIR)

# 移除所有（包括FSPI库）
clean-all: clean
	@echo "移除FSPI库"
	$(MAKE) -C ../fspi clean

# 运行应用 (使用默认模型路径)
run: $(TARGET)
	@echo "=== 说明：检查FSPI库依赖 ==="
	@if [ ! -f "../fspi/lib/libfspi.a" ]; then \
		echo "异常: FSPI库不存在，请先构建FSPI模块:"; \
		echo "  cd ../fspi && make lib"; \
		exit 1; \
	else \
		echo "说明：FSPI库依赖检查通过"; \
	fi
	@echo "=== 说明：检查PCIE驱动 ==="
	@if [ ! -e "/dev/pango_pci_driver" ]; then \
		echo "注意: PCIE驱动设备文件不存在，尝试加载驱动..."; \
		if [ -f "../debian_pcie/pango_pci_driver.ko" ]; then \
			sudo insmod ../debian_pcie/pango_pci_driver.ko || { echo "PCIE驱动加载未成功"; exit 1; }; \
			echo "PCIE驱动加载已完成"; \
		else \
			echo "异常: 找不到PCIE驱动文件"; exit 1; \
		fi; \
	else \
		echo "说明：PCIE驱动设备文件已存在"; \
	fi
	@echo "=== 说明：检查YOLO模型文件 ==="
	@if [ ! -f "model/RK3566_RK3568/yolov5s-640-640.rknn" ]; then \
		echo "异常: YOLO模型文件不存在: model/RK3566_RK3568/yolov5s-640-640.rknn"; exit 1; \
	fi
	@if [ ! -f "model/coco_80_labels_list.txt" ]; then \
		echo "异常: YOLO标签文件不存在: model/coco_80_labels_list.txt"; exit 1; \
	fi
	@if [ ! -f "lib/librknnrt.so" ]; then \
		echo "异常: RKNN运行时库不存在: lib/librknnrt.so"; exit 1; \
	fi
	@echo "说明：所有依赖检查通过"
	@echo "=== 运行应用 ==="
	@echo "程序运行中..."
	@echo "说明：按任意键或点击窗口退出程序"
	cd $(BUILDDIR) && sudo ./$(notdir $(TARGET))

# 说明：调试信息
debug: CFLAGS += -g -DDEBUG
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# 说明：安装（可选）
install: $(TARGET)
	@echo "说明：安装程序到 /usr/local/bin"
	sudo cp $(TARGET) /usr/local/bin/

# 用法信息
help:
	@echo "=== 说明：PCIE + YOLO 整合系统 Makefile ==="
	@echo "说明：可用的make目标:"
	@echo "  run     - 构建并运行应用 (推荐使用)"
	@echo "  all     - 仅构建程序"
	@echo "  clean   - 移除构建文件"
	@echo "  debug   - 构建调试版本"
	@echo "  说明：install - 安装程序到系统"
	@echo "  help    - 呈现此用法信息"
	@echo ""
	@echo "说明：快速开始: make run"

# 呈现构建信息
info:
	@echo "说明：项目信息:"
	@echo "  说明：目标程序: $(TARGET)"
	@echo "  说明：C源文件: $(C_SOURCES)"
	@echo "  说明：C++源文件: $(CXX_SOURCES)"
	@echo "  说明：包含路径: $(INCLUDES)"
	@echo "  说明：库路径: $(LIBDIRS)"
	@echo "  说明：链接库: $(LIBS)"

# 说明：Shared headers are authoritative; rebuild their consumers when they change.
-include $(OBJECTS:.o=.d)
