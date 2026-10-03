# Makefile for PCIe Capture Module
# Author: 辉哥大盗
# Date: 2025年10月17日

#############################################################################
# 基本配置
#############################################################################
TARGET = pcie_capture
TOPDIR = $(shell pwd)
BUILD_DIR = build
SRC_DIR = csrc
INC_DIR = include
LIB_DIR = lib
MODEL_DIR = model

# 编译器设置
CC = gcc
CXX = g++
STRIP = strip

# 编译选项
CFLAGS = -std=c99 -Wall -O2 -g
CXXFLAGS = -std=c++11 -Wall -O2 -g

# 渲染功能控制
ENABLE_RENDERING ?= 1
ifeq ($(ENABLE_RENDERING), 1)
    CFLAGS += -DENABLE_RENDERING=1
    CXXFLAGS += -DENABLE_RENDERING=1
else
    CFLAGS += -DENABLE_RENDERING=0
    CXXFLAGS += -DENABLE_RENDERING=0
endif

#############################################################################
# 包含路径
#############################################################################
INCLUDES = -I$(INC_DIR)

# 如果从顶层构建，包含顶层的include目录
ifdef TOPDIR
    INCLUDES += -I$(TOPDIR)/include
endif

# X11支持（仅在启用渲染时）
ifeq ($(ENABLE_RENDERING), 1)
    INCLUDES += -I/usr/include/X11
endif

#############################################################################
# 库路径和链接库
#############################################################################
LIBS = -L$(LIB_DIR) -L../lib
LDFLAGS = -pthread -lm -ldl

# RKNN库 - 总是链接 (如果找不到会给出警告但继续编译)
LIBS += -lrknnrt

# X11库（仅在启用渲染时）
ifeq ($(ENABLE_RENDERING), 1)
    LDFLAGS += -lX11 -lXext
endif

#############################################################################
# 源文件
#############################################################################
# 基础源文件
C_SRCS_BASE = $(SRC_DIR)/fspi_module.c $(SRC_DIR)/preprocess.c
CXX_SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# 总是包含YOLO功能
C_SRCS = $(C_SRCS_BASE) $(SRC_DIR)/yolo_integration.c

C_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRCS))
CXX_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CXX_SRCS))

# 如果从顶层构建，添加顶层的共享内存对象文件
ifdef TOPDIR
    SHARED_MEMORY_OBJ = $(TOPDIR)/build/shared_memory.o
    OBJS = $(C_OBJS) $(CXX_OBJS) $(SHARED_MEMORY_OBJ)
else
    OBJS = $(C_OBJS) $(CXX_OBJS)
endif

#############################################################################
# 默认目标
#############################################################################
.PHONY: all clean install debug release info

all: info $(BUILD_DIR)/$(TARGET)

#############################################################################
# 编译信息
#############################################################################
info:
	@echo "======================================"
	@echo "PCIe Capture Module Build System"
	@echo "======================================"
	@echo "Target: $(TARGET)"
	@echo "Build Directory: $(BUILD_DIR)"
	@echo "Rendering Support: $(ENABLE_RENDERING)"
	@echo "YOLO Detection: Enabled"
	@echo "Compiler: $(CC) / $(CXX)"
	@echo "======================================"

#############################################################################
# 主目标
#############################################################################
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "Linking: $@"
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJS) -o $@ $(LIBS) $(LDFLAGS)
	@echo "Build successful: $@"

#############################################################################
# C源文件编译
#############################################################################
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling C: $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

#############################################################################
# C++源文件编译
#############################################################################
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling C++: $<"
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

#############################################################################
# 构建模式
#############################################################################
debug: CFLAGS += -DDEBUG -g -O0
debug: CXXFLAGS += -DDEBUG -g -O0
debug: $(BUILD_DIR)/$(TARGET)

release: CFLAGS += -DNDEBUG -O3
release: CXXFLAGS += -DNDEBUG -O3
release: $(BUILD_DIR)/$(TARGET)
	$(STRIP) $(BUILD_DIR)/$(TARGET)

#############################################################################
# 安装
#############################################################################
install: $(BUILD_DIR)/$(TARGET)
	@echo "Installing $(TARGET)..."
	@mkdir -p $(BUILD_DIR)/bin
	@cp $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/bin/
	@cp -r $(MODEL_DIR) $(BUILD_DIR)/ 2>/dev/null || true
	@echo "Installation complete"

#############################################################################
# 清理
#############################################################################
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

#############################################################################
# 帮助
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
# 依赖关系
#############################################################################
-include $(OBJS:.o=.d)

# 生成依赖文件
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) -MM $(CFLAGS) $(INCLUDES) $< | sed 's,\($*\.o\)[ :]*,$(BUILD_DIR)/\1 $@ : ,g' > $@

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@$(CXX) -MM $(CXXFLAGS) $(INCLUDES) $< | sed 's,\($*\.o\)[ :]*,$(BUILD_DIR)/\1 $@ : ,g' > $@