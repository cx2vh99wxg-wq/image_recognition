# ===========================================================================
# Makefile — 人员 A（感知层）· M 端采集程序
# 仅编译 A 负责的源文件；共享内存(shm_ipc)、UDP、渲染、SPI 分属 B/C。
# 拼接时链接 B 提供的 common/csrc/shm_ipc.c 与 librknnrt。
# ===========================================================================
TARGET      = perception_m
BUILD_DIR   = build
SRC_DIR     = csrc
INC_DIR     = include
LIB_DIR     = lib
MODEL_DIR   = model

CC          = gcc
CXX         = g++
STRIP       = strip

# 统一编译规范（见分工方案 6.3）
CFLAGS      = -std=c11 -Wall -Wextra -O2 -g
CXXFLAGS    = -std=c++17 -Wall -Wextra -O2 -g

# A 的头 + 公共契约头（common 在仓库根 common/）
INCLUDES    = -I$(INC_DIR) -I../../../common/include

# 渲染开关：默认关（显示是 B 的职责）
ENABLE_RENDERING ?= 0
ifeq ($(ENABLE_RENDERING),1)
    CFLAGS   += -DENABLE_RENDERING=1
    CXXFLAGS += -DENABLE_RENDERING=1
endif

# 库
LDFLAGS     = -pthread -lm -ldl
LIBS        = -L$(LIB_DIR) -lrknnrt

# A 的源文件（不含 fspi/C 的 SPI、不含 shared_memory/B 的基础设施）
C_SRCS      = $(SRC_DIR)/preprocess.c $(SRC_DIR)/yolo_integration.c
CXX_SRCS    = $(SRC_DIR)/yolop_opencv_integration.cpp $(SRC_DIR)/main.cpp
C_OBJS      = $(patsubst $(SRC_DIR)/%.c,   $(BUILD_DIR)/%.o, $(C_SRCS))
CXX_OBJS    = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CXX_SRCS))

.PHONY: all clean install help

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(C_OBJS) $(CXX_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(C_OBJS) $(CXX_OBJS) -o $@ $(LIBS) $(LDFLAGS)
	@echo "构建完成: $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

install: all
	@mkdir -p $(BUILD_DIR)/bin
	@cp $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/bin/
	@cp -r $(MODEL_DIR) $(BUILD_DIR)/ 2>/dev/null || true

clean:
	@rm -rf $(BUILD_DIR)

help:
	@echo "A · M 端感知程序构建"
	@echo "  make                构建（默认无渲染）"
	@echo "  make ENABLE_RENDERING=1  含本地 X11 显示（需 B 的渲染协同）"
	@echo "  make clean           清理"
