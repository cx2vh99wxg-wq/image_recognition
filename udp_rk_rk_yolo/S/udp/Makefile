# =============================================================================
# PCIe DMA 图像采集测试程序 Makefile (支持C++和C混合编译)
# =============================================================================

# 项目配置
TARGET = video_receiver
CXX = g++
CC = gcc

# 目录配置
BUILD_DIR = build
SRC_DIR = csrc
INC_DIR = include

# 文件配置
CXXSRCS = $(filter-out $(SRC_DIR)/main_old.cpp,$(wildcard $(SRC_DIR)/*.cpp))  # 排除旧的main文件
CSRCS = # UDP程序不需要FSPI等C文件，只使用C++和共享库
INCLUDES = $(wildcard $(INC_DIR)/*.h)
CXX_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CXXSRCS))
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CSRCS))
OBJECTS = $(CXX_OBJECTS) $(C_OBJECTS)
BIN = $(BUILD_DIR)/$(TARGET)

# 共享库配置
SHARED_LIB ?= libshared.a

# 编译标志配置
INCFLAGS = -I$(INC_DIR) -I../$(INC_DIR)

# UDP后台程序不需要X11库和FSPI库，只需要基本的网络和共享内存库
# 编译和链接标志
# -O3: 最高优化级别
# -march=native: 使用本机CPU指令集优化
# -funroll-loops: 循环展开优化
# -pthread: 线程支持
CXXFLAGS = $(INCFLAGS) -std=c++14 -O3 -march=native -funroll-loops -pthread
CFLAGS = $(INCFLAGS) -std=c99 -O3 -march=native -funroll-loops -pthread
LDFLAGS = -lrt -lpthread

# 检查共享库是否存在
ifneq ($(wildcard $(SHARED_LIB)),)
    LDFLAGS += $(SHARED_LIB)
endif

# =============================================================================
# 构建规则
# =============================================================================

# 默认目标
all: $(BIN)

# 创建构建目录
$(shell mkdir -p $(BUILD_DIR))

# 生成最终可执行文件
$(BIN): $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# 编译源文件生成目标文件 - C++文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES)
	@mkdir -p $(BUILD_DIR)
	$(CXX) -c $(CXXFLAGS) $< -o $@

# 编译源文件生成目标文件 - C文件  
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDES)
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

# 运行程序
run: $(BIN)
	@echo "启动UDP视频接收程序..."
	@echo "DISPLAY: $(DISPLAY)"
	@echo "用户: $(shell whoami)"
	@if [ "$(shell id -u)" = "0" ]; then \
		if [ ! -z "$(SUDO_USER)" ]; then \
			echo "以用户 $(SUDO_USER) 权限运行程序..."; \
			sudo -u "$(SUDO_USER)" DISPLAY="$(DISPLAY)" XAUTHORITY="/home/$(SUDO_USER)/.Xauthority" $(BIN); \
		else \
			echo "以root权限运行程序..."; \
			DISPLAY="$(DISPLAY)" $(BIN); \
		fi \
	else \
		echo "以当前用户权限运行程序..."; \
		$(BIN); \
	fi

# 清理构建文件
clean:
	rm -rf $(BUILD_DIR)

# 声明伪目标
.PHONY: all clean run