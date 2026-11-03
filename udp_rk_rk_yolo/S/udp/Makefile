# =============================================================================
# PCIe DMA 图像采集验证程序 Makefile (支持C++和C混合构建)
# =============================================================================

# 项目设置
TARGET = video_receiver
CXX = g++
CC = gcc

# 工程路径约定
BUILD_DIR = build
SRC_DIR = csrc
INC_DIR = include

# 参与构建的文件
CXXSRCS = $(filter-out $(SRC_DIR)/main_old.cpp,$(wildcard $(SRC_DIR)/*.cpp))  # 说明：排除旧的main文件
CSRCS = # UDP程序不需要FSPI等C文件，只使用C++和共享库
INCLUDES = $(wildcard $(INC_DIR)/*.h) ../include/shared_memory.h
CXX_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CXXSRCS))
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CSRCS))
OBJECTS = $(CXX_OBJECTS) $(C_OBJECTS)
BIN = $(BUILD_DIR)/$(TARGET)

# 共享库设置
SHARED_LIB ?= ../build/libshared.a
AR ?= ar

# 构建标志设置
INCFLAGS = -I$(INC_DIR) -I../$(INC_DIR)

# 说明：UDP后台程序不需要X11库和FSPI库，只需要基本的网络和共享内存库
# 构建和链接标志
# 说明：-O3: 最高优化级别
# 说明：-march=native: 使用本机CPU指令集优化
# 说明：-funroll-loops: 循环展开优化
# 说明：-pthread: 线程支持
CXXFLAGS = $(INCFLAGS) -std=c++14 -O3 -march=native -funroll-loops -pthread -MMD -MP
CFLAGS = $(INCFLAGS) -std=c99 -O3 -march=native -funroll-loops -pthread -MMD -MP
LDFLAGS = $(SHARED_LIB) -lrt -lpthread

# =============================================================================
# 说明：构建规则
# =============================================================================

# 直接调用 make 时采用的入口
all: $(BIN)

test-protocol: | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -UNDEBUG -Iinclude tests/test_video_transport_v2.cpp -o $(BUILD_DIR)/test_video_transport_v2
	$(BUILD_DIR)/test_video_transport_v2

# 建立产物目录
$(BUILD_DIR):
	mkdir -p $@

# 生成最终程序文件
$(BIN): $(OBJECTS) $(SHARED_LIB) | $(BUILD_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(SHARED_LIB): ../csrc/shared_memory.c $(wildcard ../include/*.h) ../../common/perception/include/curve_shm_attach.h | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c ../csrc/shared_memory.c -o $(BUILD_DIR)/shared_memory.o
	$(AR) rcs $@ $(BUILD_DIR)/shared_memory.o

# 构建源文件生成目标文件 - C++文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES)
	@mkdir -p $(BUILD_DIR)
	$(CXX) -c $(CXXFLAGS) $< -o $@

# 构建源文件生成目标文件 - C文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDES)
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

# 运行应用
run: $(BIN)
	@echo "运行UDP视频收取程序..."
	@echo "DISPLAY: $(DISPLAY)"
	@echo "说明：用户: $(shell whoami)"
	@if [ "$(shell id -u)" = "0" ]; then \
		if [ ! -z "$(SUDO_USER)" ]; then \
			echo "以用户 $(SUDO_USER) 权限运行应用..."; \
			sudo -u "$(SUDO_USER)" DISPLAY="$(DISPLAY)" XAUTHORITY="/home/$(SUDO_USER)/.Xauthority" $(BIN); \
		else \
			echo "以root权限运行应用..."; \
			DISPLAY="$(DISPLAY)" $(BIN); \
		fi \
	else \
		echo "以此时用户权限运行应用..."; \
		$(BIN); \
	fi

# 移除构建文件
clean:
	rm -rf $(BUILD_DIR)

# 说明：声明伪目标
.PHONY: all clean run test-protocol

-include $(OBJECTS:.o=.d)
