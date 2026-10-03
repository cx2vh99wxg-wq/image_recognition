# =============================================================================
# X11八摄像头渲染程序 Makefile
# =============================================================================

# 项目配置
TARGET = eight_camera_renderer
CXX = g++
CC = gcc

# 目录配置
BUILD_DIR = build
SRC_DIR = .
INC_DIR = include

# 文件配置
CXXSRCS = $(wildcard $(SRC_DIR)/*.cpp)
CSRCS = ../../csrc/shared_memory.c  # 添加共享内存源文件
INCLUDES = $(wildcard $(INC_DIR)/*.h) $(wildcard ../../include/*.h)  # 添加上级头文件
CXX_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CXXSRCS))
C_OBJECTS = $(patsubst ../../csrc/%.c,$(BUILD_DIR)/%.o,$(CSRCS))
OBJECTS = $(CXX_OBJECTS) $(C_OBJECTS)
BIN = $(BUILD_DIR)/$(TARGET)

# 编译标志配置  
INCFLAGS = -I../../include -I$(INC_DIR)

# 获取X11库的编译标志和链接库
X11_LIBS = -lX11 -lXext

# 编译和链接标志
# -O3: 最高优化级别
# -march=native: 使用本机CPU指令集优化
# -funroll-loops: 循环展开优化
# -pthread: 线程支持
CXXFLAGS = $(INCFLAGS) -std=c++14 -O3 -march=native -funroll-loops -pthread
CFLAGS = $(INCFLAGS) -std=c99 -O3 -march=native -funroll-loops -pthread
LDFLAGS = $(X11_LIBS) -lrt -lpthread

# =============================================================================
# 构建规则
# =============================================================================

.PHONY: all clean help run

# 默认目标
all: $(BIN)

# 链接可执行文件
$(BIN): $(OBJECTS) | $(BUILD_DIR)
	@echo "链接可执行文件: $@"
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "编译完成: $(TARGET) - 8摄像头视频渲染程序"

# 编译C++源文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES) | $(BUILD_DIR)
	@echo "编译C++: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译C源文件 (用于共享内存模块)
$(BUILD_DIR)/%.o: ../../csrc/%.c $(INCLUDES) | $(BUILD_DIR)
	@echo "编译共享内存C文件: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# 编译C源文件 (本地文件)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDES) | $(BUILD_DIR)
	@echo "编译C: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# 创建构建目录
$(BUILD_DIR):
	@echo "创建构建目录: $(BUILD_DIR)"
	mkdir -p $(BUILD_DIR)

# 运行程序
run: $(BIN)
	@echo "运行X11八摄像头渲染程序..."
	./$(BIN)

# 清理构建文件
clean:
	@echo "清理构建文件..."
	rm -rf $(BUILD_DIR)
	@echo "清理完成"

# 显示帮助信息
help:
	@echo "=== X11八摄像头渲染程序构建系统 ==="
	@echo "可用目标："
	@echo "  all     - 编译程序 (默认)"
	@echo "  clean   - 清理构建文件"
	@echo "  run     - 编译并运行程序"
	@echo "  help    - 显示此帮助信息"
	@echo ""
	@echo "程序功能："
	@echo "  - 读取PCIe和UDP共享内存"
	@echo "  - 支持8个独立摄像头显示 (0x11-0x88)"
	@echo "  - 支持全部摄像头2x4网格显示 (0x99)"
	@echo "  - 响应FSPI按钮控制的视角切换"
	@echo "  - 单摄像头320x240放大到640x480显示"
	@echo "  - X11实时显示"
	@echo ""
	@echo "依赖："
	@echo "  - X11开发库"
	@echo "  - 共享内存库"

# 调试信息
debug-info:
	@echo "=== 构建配置信息 ==="
	@echo "目标程序: $(TARGET)"
	@echo "C++编译器: $(CXX)"
	@echo "C编译器: $(CC)"
	@echo "C++源文件: $(CXXSRCS)"
	@echo "C源文件: $(CSRCS)"
	@echo "目标文件: $(OBJECTS)"
	@echo "包含路径: $(INCFLAGS)"
	@echo "C++编译标志: $(CXXFLAGS)"
	@echo "C编译标志: $(CFLAGS)"
	@echo "链接标志: $(LDFLAGS)"
	@echo "共享库: $(SHARED_LIB)"
