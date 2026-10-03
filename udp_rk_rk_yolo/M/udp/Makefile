# Makefile for UDP Network Module
# Author: 辉哥大盗
# Date: 2025年10月17日

#############################################################################
# 基本配置
#############################################################################
TARGET = udp_sender
TOPDIR = $(shell pwd)
BUILD_DIR = build
SRC_DIR = csrc
INC_DIR = include

# 编译器设置
CC = gcc
CXX = g++
STRIP = strip

# 编译选项
CFLAGS = -std=c99 -Wall -O2 -g
CXXFLAGS = -std=c++11 -Wall -O2 -g

#############################################################################
# 包含路径
#############################################################################
INCLUDES = -I$(INC_DIR)

# 如果从顶层构建，包含顶层的include目录
ifdef TOPDIR
    INCLUDES += -I$(TOPDIR)/include
endif

#############################################################################
# 库路径和链接库
#############################################################################
LDFLAGS = -pthread -lm

#############################################################################
# 源文件
#############################################################################
C_SRCS = $(wildcard $(SRC_DIR)/*.c)
CXX_SRCS = $(wildcard $(SRC_DIR)/*.cpp)

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
.PHONY: all clean install debug release info test

all: info $(BUILD_DIR)/$(TARGET)

#############################################################################
# 编译信息
#############################################################################
info:
	@echo "======================================"
	@echo "UDP Network Module Build System"
	@echo "======================================"
	@echo "Target: $(TARGET)"
	@echo "Build Directory: $(BUILD_DIR)"
	@echo "Compiler: $(CC) / $(CXX)"
	@echo "======================================"

#############################################################################
# 主目标
#############################################################################
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "Linking: $@"
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
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
# 测试目标
#############################################################################
test: $(BUILD_DIR)/$(TARGET)
	@echo "Running UDP transmission test..."
	@echo "Default target IP: 192.168.100.20:8888"
	@echo "Use: make test TARGET_IP=<ip_address> to specify different target"
	$(BUILD_DIR)/$(TARGET) $(TARGET_IP)

#############################################################################
# 安装
#############################################################################
install: $(BUILD_DIR)/$(TARGET)
	@echo "Installing $(TARGET)..."
	@mkdir -p $(BUILD_DIR)/bin
	@cp $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/bin/
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
	@echo "  test          - Build and run transmission test"
	@echo "  install       - Install binary"
	@echo "  clean         - Clean build directory"
	@echo "  help          - Show this help"
	@echo ""
	@echo "Test options:"
	@echo "  TARGET_IP=<ip>  - Specify target IP for test"
	@echo ""
	@echo "Examples:"
	@echo "  make                               - Build UDP sender"
	@echo "  make test                          - Test with default IP"
	@echo "  make test TARGET_IP=192.168.1.100 - Test with custom IP"
	@echo "  make debug                         - Build debug version"
	@echo "  make release                       - Build release version"

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