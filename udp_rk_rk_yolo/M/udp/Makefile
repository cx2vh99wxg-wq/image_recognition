# 说明：Makefile for UDP Network Module
# Maintainer: 科研轮转
# 本版更新：2026年11月3日

#############################################################################
# 目标、源码及产物路径
#############################################################################
TARGET = udp_sender
TOPDIR ?= $(abspath ..)
BUILD_DIR = build
SRC_DIR = csrc
INC_DIR = include

# 选择 C 和 C++ 工具链
CC = gcc
CXX = g++
STRIP = strip

# 传给编译器的参数
CFLAGS = -std=c99 -Wall -O3 -march=native -MMD -MP
CXXFLAGS = -std=c++11 -Wall -O3 -march=native -MMD -MP

#############################################################################
# 头文件搜索范围
#############################################################################
INCLUDES = -I$(INC_DIR)

# 说明：如果从顶层构建，包含顶层的include目录
ifdef TOPDIR
    INCLUDES += -I$(TOPDIR)/include
endif

#############################################################################
# 说明：库路径和链接库
#############################################################################
LDFLAGS = -pthread -lm

#############################################################################
# 选择需要编译的源码
#############################################################################
C_SRCS = $(wildcard $(SRC_DIR)/*.c)
CXX_SRCS = $(wildcard $(SRC_DIR)/*.cpp)

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
.PHONY: all clean install debug release info test test-protocol

all: info $(BUILD_DIR)/$(TARGET)

test-protocol:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -UNDEBUG -Iinclude tests/test_video_transport_v2.cpp -o $(BUILD_DIR)/test_video_transport_v2
	$(BUILD_DIR)/test_video_transport_v2

#############################################################################
# 构建信息
#############################################################################
info:
	@echo "======================================"
	@echo "UDP Network Module Build Console"
	@echo "======================================"
	@echo "Build target: $(TARGET)"
	@echo "Build output: $(BUILD_DIR)"
	@echo "Toolchain: $(CC) / $(CXX)"
	@echo "======================================"

#############################################################################
# 说明：主目标
#############################################################################
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "Writing executable: $@"
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build ready: $@"

$(SHARED_MEMORY_OBJ): $(TOPDIR)/csrc/shared_memory.c $(TOPDIR)/include/shared_memory.h $(TOPDIR)/../common/perception/include/curve_shm_attach.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(TOPDIR)/include -c $< -o $@

#############################################################################
# C源文件构建
#############################################################################
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(wildcard $(INC_DIR)/*.h) $(TOPDIR)/include/shared_memory.h
	@echo "Translating C: $<"
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

#############################################################################
# C++源文件构建
#############################################################################
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(wildcard $(INC_DIR)/*.h) $(TOPDIR)/include/shared_memory.h
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
# 验证目标
#############################################################################
test: $(BUILD_DIR)/$(TARGET)
	@echo "Launching UDP transmission test..."
	@echo "Default target IP: 192.168.100.20:8888"
	@echo "Use: make test TARGET_IP=<ip_address> to specify different target"
	$(BUILD_DIR)/$(TARGET) $(TARGET_IP)

#############################################################################
# 将产物放入安装路径
#############################################################################
install: $(BUILD_DIR)/$(TARGET)
	@echo "Setting up $(TARGET)..."
	@mkdir -p $(BUILD_DIR)/bin
	@cp $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/bin/
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
# 跟踪头文件变动
#############################################################################
-include $(OBJS:.o=.d)
