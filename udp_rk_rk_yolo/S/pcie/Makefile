# PCIE + YOLO + UDP 整合项目 Makefile
# 编译PCIE视频流、YOLO目标检测和UDP网络传输整合系统

# 编译器设置
CC = gcc
CXX = g++

# 模型路径定义
# 编译时使用（相对于pcie目录，因为主Makefile在pcie目录下运行程序）
MODEL_PATH_COMPILE = "model/RK3566_RK3568/yolov5s-640-640.rknn"
LABELS_PATH_COMPILE = "model/coco_80_labels_list.txt"

# 编译选项 (高性能优化)
CFLAGS = -Wall -O3 -march=native -std=c11 -DYOLO_MODEL_PATH=\"$(MODEL_PATH_COMPILE)\" -DYOLO_LABELS_PATH=\"$(LABELS_PATH_COMPILE)\"
CXXFLAGS = -Wall -O3 -march=native -std=c++11 -DYOLO_MODEL_PATH=\"$(MODEL_PATH_COMPILE)\" -DYOLO_LABELS_PATH=\"$(LABELS_PATH_COMPILE)\"

# 项目目录
SRCDIR = csrc
INCDIR = include
BUILDDIR = build

# 目标程序名
TARGET = $(BUILDDIR)/app

# 源文件
C_SOURCES = $(SRCDIR)/yolo_integration.c \
            $(SRCDIR)/preprocess.c \
            $(SRCDIR)/person_alert_effects.c

CXX_SOURCES = $(SRCDIR)/main.cpp \
              $(SRCDIR)/postprocess.cpp

# 对象文件
C_OBJECTS = $(C_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
CXX_OBJECTS = $(CXX_SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
OBJECTS = $(C_OBJECTS) $(CXX_OBJECTS)

# 包含路径
INCLUDES = -I$(INCDIR) \
           -I../include \
           -I../rknn_yolov5_demo/include

# 库路径
LIBDIRS = -L./lib \
          -L../build

# 链接库
LIBS = -lrknnrt \
       -lshared \
       -lpthread \
       -lm \
       -lrt

# 编译规则
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BUILDDIR)
	@echo "链接目标程序: $@"
	$(CXX) $(OBJECTS) $(LIBDIRS) $(LIBS) -o $@
	@echo "编译完成!"

# C源文件编译规则
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@echo "编译C文件: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# C++源文件编译规则
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	@echo "编译C++文件: $<"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 创建build目录
$(BUILDDIR):
	@echo "创建build目录"
	mkdir -p $(BUILDDIR)

# 清理
clean:
	@echo "清理编译文件"
	rm -rf $(BUILDDIR)

# 清理所有（包括FSPI库）
clean-all: clean
	@echo "清理FSPI库"
	$(MAKE) -C ../fspi clean

# 运行程序 (使用默认模型路径)
run: $(TARGET)
	@echo "=== 检查FSPI库依赖 ==="
	@if [ ! -f "../fspi/lib/libfspi.a" ]; then \
		echo "错误: FSPI库不存在，请先编译FSPI模块:"; \
		echo "  cd ../fspi && make lib"; \
		exit 1; \
	else \
		echo "FSPI库依赖检查通过"; \
	fi
	@echo "=== 检查PCIE驱动 ==="
	@if [ ! -e "/dev/pango_pci_driver" ]; then \
		echo "警告: PCIE驱动设备文件不存在，尝试加载驱动..."; \
		if [ -f "../debian_pcie/pango_pci_driver.ko" ]; then \
			sudo insmod ../debian_pcie/pango_pci_driver.ko || { echo "PCIE驱动加载失败"; exit 1; }; \
			echo "PCIE驱动加载成功"; \
		else \
			echo "错误: 找不到PCIE驱动文件"; exit 1; \
		fi; \
	else \
		echo "PCIE驱动设备文件已存在"; \
	fi
	@echo "=== 检查YOLO模型文件 ==="
	@if [ ! -f "model/RK3566_RK3568/yolov5s-640-640.rknn" ]; then \
		echo "错误: YOLO模型文件不存在: model/RK3566_RK3568/yolov5s-640-640.rknn"; exit 1; \
	fi
	@if [ ! -f "model/coco_80_labels_list.txt" ]; then \
		echo "错误: YOLO标签文件不存在: model/coco_80_labels_list.txt"; exit 1; \
	fi
	@if [ ! -f "lib/librknnrt.so" ]; then \
		echo "错误: RKNN运行时库不存在: lib/librknnrt.so"; exit 1; \
	fi
	@echo "所有依赖检查通过"
	@echo "=== 运行程序 ==="
	@echo "程序启动中..."
	@echo "按任意键或点击窗口退出程序"
	cd $(BUILDDIR) && sudo ./$(notdir $(TARGET))

# 调试信息
debug: CFLAGS += -g -DDEBUG
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# 安装（可选）
install: $(TARGET)
	@echo "安装程序到 /usr/local/bin"
	sudo cp $(TARGET) /usr/local/bin/

# 帮助信息
help:
	@echo "=== PCIE + YOLO 整合系统 Makefile ==="
	@echo "可用的make目标:"
	@echo "  run     - 编译并运行程序 (推荐使用)"
	@echo "  all     - 仅编译程序"
	@echo "  clean   - 清理编译文件"
	@echo "  debug   - 编译调试版本"
	@echo "  install - 安装程序到系统"
	@echo "  help    - 显示此帮助信息"
	@echo ""
	@echo "快速开始: make run"

# 显示编译信息
info:
	@echo "项目信息:"
	@echo "  目标程序: $(TARGET)"
	@echo "  C源文件: $(C_SOURCES)"
	@echo "  C++源文件: $(CXX_SOURCES)"
	@echo "  包含路径: $(INCLUDES)"
	@echo "  库路径: $(LIBDIRS)"
	@echo "  链接库: $(LIBS)"