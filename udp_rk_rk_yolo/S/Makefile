# =============================================================================
# FPGA八摄像头视频显示系统主Makefile
# 作者: 辉哥大盗
# 日期: 2025年11月1日
# 功能: 编译PCIe和UDP程序，提供8摄像头独立显示功能
# =============================================================================

# 项目配置
SHARED_LIB = libshared.a
PCIE_TARGET = pcie/build/app
UDP_TARGET = udp/build/video_receiver
X11_TARGET = x11/csrc/build/eight_camera_renderer
FSPI_TARGET = fspi/build/vehicle_control

# 编译器配置
CXX = g++
CC = gcc
AR = ar

# 目录配置
SHARED_SRC_DIR = csrc
SHARED_INC_DIR = include
BUILD_DIR = build

# 共享库源文件
SHARED_CSRCS = $(wildcard $(SHARED_SRC_DIR)/*.c)
SHARED_OBJECTS = $(patsubst $(SHARED_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SHARED_CSRCS))

# 编译标志
INCFLAGS = -I$(SHARED_INC_DIR)
CFLAGS = $(INCFLAGS) -std=c99 -O3 -march=native -funroll-loops -pthread -fPIC
ARFLAGS = rcs

# 获取系统信息用于调试
UNAME := $(shell uname -s)
ARCH := $(shell uname -m)

# =============================================================================
# 主要目标
# =============================================================================

.PHONY: all clean pcie udp x11 fspi shared run run-background run-display run-fspi stop stop-force help install deps check

# 默认目标：编译所有程序
all: shared pcie udp x11 fspi
	@echo "=== 编译完成 ==="
	@echo "PCIe程序:  $(PCIE_TARGET)"
	@echo "UDP程序:   $(UDP_TARGET)"
	@echo "X11程序:   $(X11_TARGET)"
	@echo "FSPI程序:  $(FSPI_TARGET)"
	@echo "共享库:    $(BUILD_DIR)/$(SHARED_LIB)"
	@echo ""
	@echo "使用 'make run-background' 启动后台程序 (PCIe + UDP)"
	@echo "使用 'make run-display' 启动显示程序 (X11)"
	@echo "使用 'make run-fspi' 启动车辆控制程序 (FSPI)"
	@echo "使用 'make run' 启动完整系统"
	@echo "使用 'make stop' 停止系统"

# 启动后台程序（PCIe + UDP）
run-background: all
	@echo "=== 启动FPGA双通道后台程序 ==="
	@echo "系统: $(UNAME) $(ARCH)"
	@echo "时间: $(shell date)"
	@echo ""
	@# 检查程序是否存在
	@if [ ! -f "$(PCIE_TARGET)" ]; then \
		echo "错误: PCIe程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(UDP_TARGET)" ]; then \
		echo "错误: UDP程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@# 清理之前的共享内存
	@echo "清理共享内存..."
	@-ipcrm -M 0x12345679 2>/dev/null || true
	@-ipcrm -M 0x1234567A 2>/dev/null || true
	@-ipcrm -M 0x1234567C 2>/dev/null || true
	@-ipcrm -M 0x1234567D 2>/dev/null || true
	@-ipcrm -S 0x12345679 2>/dev/null || true
	@-ipcrm -S 0x1234567A 2>/dev/null || true
	@-ipcrm -S 0x1234567C 2>/dev/null || true
	@-ipcrm -S 0x1234567D 2>/dev/null || true
	@echo "共享内存清理完成"
	@echo ""
	@# 启动PCIe后台程序
	@echo "启动PCIe图像采集后台程序..."
	@echo "运行PCIe程序需要sudo权限..."
	@cd pcie && sudo ./build/app &
	@echo "PCIe程序已启动，等待初始化..."
	@sleep 3
	@echo ""
	@# 启动UDP后台程序
	@echo "启动UDP网络接收后台程序..."
	@cd udp && ./build/video_receiver &
	@echo "UDP程序已启动，等待初始化..."
	@sleep 2
	@echo ""
	@echo "后台程序启动完成！"
	@echo "现在可以运行 'make run-display' 启动显示程序"

# 启动车辆控制程序
run-fspi: all
	@echo "=== 启动FSPI车辆控制程序 ==="
	@if [ ! -f "$(FSPI_TARGET)" ]; then \
		echo "错误: FSPI程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@echo "启动车辆控制程序..."
	@echo "控制说明: w=前进, s=后退, a=左转, d=右转, x=停止, q=退出"
	@echo "注意: 需要管理员权限访问SPI设备"
	@echo ""
	@cd fspi && sudo ./build/vehicle_control

# 启动显示程序
run-display: all
	@echo "=== 启动X11八摄像头显示程序 ==="
	@if [ ! -f "$(X11_TARGET)" ]; then \
		echo "错误: X11程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@echo "启动八摄像头渲染显示..."
	@echo "显示模式: 8个独立摄像头 (0x11-0x88) 或全部显示 (0x99)"
	@echo "操作: 按Q键、Esc键或关闭窗口退出"
	@echo ""
	@cd x11/csrc && ./build/eight_camera_renderer

# 启动完整系统（后台 + FSPI控制 + 显示）
run: all
	@echo "=== 启动FPGA八摄像头视频显示系统 (带视角切换) ==="
	@echo "系统: $(UNAME) $(ARCH)"
	@echo "时间: $(shell date)"
	@echo ""
	@echo "功能说明:"
	@echo "- PCIe图像采集 + YOLO人员检测 + UDP网络接收"
	@echo "- FSPI模式识别视角切换 + 紧急制动控制"
	@echo "- X11实时显示 (支持9种显示模式)"
	@echo "- 模式控制: 直接识别0x11-0x88对应摄像头，0x99显示全部"
	@echo "- 紧急制动: PCIe检测到人员时→FSPI寄存器4写0→紧急制动"
	@echo ""
	@# 检查程序是否存在
	@if [ ! -f "$(PCIE_TARGET)" ]; then \
		echo "错误: PCIe程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(UDP_TARGET)" ]; then \
		echo "错误: UDP程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(FSPI_TARGET)" ]; then \
		echo "错误: FSPI程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(X11_TARGET)" ]; then \
		echo "错误: X11程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@# 清理之前的共享内存
	@echo "清理共享内存..."
	@-ipcrm -M 0x12345679 2>/dev/null || true
	@-ipcrm -M 0x1234567A 2>/dev/null || true
	@-ipcrm -M 0x1234567C 2>/dev/null || true
	@-ipcrm -M 0x1234567D 2>/dev/null || true
	@-ipcrm -S 0x12345679 2>/dev/null || true
	@-ipcrm -S 0x1234567A 2>/dev/null || true
	@-ipcrm -S 0x1234567C 2>/dev/null || true
	@-ipcrm -S 0x1234567D 2>/dev/null || true
	@echo "共享内存清理完成"
	@echo ""
	@# 启动PCIe后台程序
	@echo "1. 启动PCIe图像采集程序..."
	@cd pcie && sudo ./build/app & echo $$! > pcie.pid
	@echo "PCIe程序已启动 (PID: $(shell cat pcie/pcie.pid 2>/dev/null || echo 'unknown'))"
	@sleep 3
	@echo ""
	@# 启动UDP后台程序
	@echo "2. 启动UDP网络接收程序..."
	@cd udp && ./build/video_receiver & echo $$! > udp.pid
	@echo "UDP程序已启动 (PID: $(shell cat udp/udp.pid 2>/dev/null || echo 'unknown'))"
	@sleep 2
	@echo ""
	@# 启动FSPI控制程序
	@echo "3. 启动FSPI按钮控制程序..."
	@cd fspi && sudo bash -c './build/vehicle_control & echo $$!' > fspi.pid
	@echo "FSPI程序已启动 (PID: $(shell cat fspi/fspi.pid 2>/dev/null || echo 'unknown'))"
	@echo "模式控制: 直接读取0x11-0x99模式值切换摄像头视角"
	@sleep 3
	@echo ""
	@# 启动X11显示程序（前台运行）
	@echo "4. 启动X11显示程序（前台运行）..."
	@echo "显示窗口: 640x480 (单摄像头) 或 1280x480 (全部摄像头)"
	@echo "显示模式: 8个独立摄像头 (0x11-0x88) 或全部显示 (0x99)"
	@echo "操作: 按Q键、Esc键或关闭窗口退出"
	@echo "退出时将自动停止所有后台程序"
	@echo ""
	@# 设置陷阱，确保退出时清理
	@trap 'make stop' EXIT; cd x11/csrc && ./build/eight_camera_renderer

# 停止系统
stop:
	@echo "=== 停止FPGA双通道视频显示系统 ==="
	@# 使用shell脚本避免make进程被影响
	@bash -c '\
		echo "停止X11程序..."; \
		pkill -TERM -f "eight_camera_renderer" 2>/dev/null || true; \
		sleep 1; \
	'
	@# 停止FSPI程序
	@bash -c '\
		echo "停止FSPI程序..."; \
		if [ -f "fspi/fspi.pid" ]; then \
			sudo kill -TERM `cat fspi/fspi.pid` 2>/dev/null || true; \
			sudo kill -KILL `cat fspi/fspi.pid` 2>/dev/null || true; \
			rm -f fspi/fspi.pid; \
		fi; \
		sudo pkill -TERM -f "vehicle_control" 2>/dev/null || true; \
		sleep 2; \
		sudo pkill -KILL -f "vehicle_control" 2>/dev/null || true; \
		sudo pkill -TERM -f "sudo.*vehicle_control" 2>/dev/null || true; \
		sudo pkill -KILL -f "sudo.*vehicle_control" 2>/dev/null || true; \
		sudo pkill -TERM -f "nohup.*vehicle_control" 2>/dev/null || true; \
		sudo pkill -KILL -f "nohup.*vehicle_control" 2>/dev/null || true; \
	'
	@# 停止UDP程序
	@bash -c '\
		echo "停止UDP程序..."; \
		if [ -f "udp/udp.pid" ]; then \
			kill -TERM `cat udp/udp.pid` 2>/dev/null || true; \
			rm -f udp/udp.pid; \
		fi; \
		pkill -TERM -f "video_receiver" 2>/dev/null || true; \
	'
	@# 停止PCIe程序
	@bash -c '\
		echo "停止PCIe程序..."; \
		if [ -f "pcie/pcie.pid" ]; then \
			sudo kill -TERM `cat pcie/pcie.pid` 2>/dev/null || true; \
			sudo kill -KILL `cat pcie/pcie.pid` 2>/dev/null || true; \
			rm -f pcie/pcie.pid; \
		fi; \
		sudo pkill -TERM -f "build/app" 2>/dev/null || true; \
		sleep 1; \
		sudo pkill -KILL -f "build/app" 2>/dev/null || true; \
	'
	@# 清理共享内存和信号量
	@echo "清理共享内存..."
	@-ipcrm -M 0x12345679 2>/dev/null || true
	@-ipcrm -M 0x1234567A 2>/dev/null || true
	@-ipcrm -M 0x1234567C 2>/dev/null || true
	@-ipcrm -M 0x1234567D 2>/dev/null || true
	@echo "清理信号量..."
	@-ipcrm -S 0x12345679 2>/dev/null || true
	@-ipcrm -S 0x1234567A 2>/dev/null || true
	@-ipcrm -S 0x1234567C 2>/dev/null || true
	@-ipcrm -S 0x1234567D 2>/dev/null || true
	@echo "系统停止完成"

# 强制停止系统（使用SIGKILL）
stop-force:
	@echo "=== 强制停止FPGA双通道视频显示系统 ==="
	@echo "警告: 使用SIGKILL强制终止所有进程"
	@# 强制停止所有相关进程
	@echo "强制停止X11程序..."
	@-sudo pkill -KILL -f "eight_camera_renderer" 2>/dev/null || true
	@echo "强制停止FSPI程序..."
	@-sudo pkill -KILL -f "vehicle_control" 2>/dev/null || true
	@-sudo pkill -KILL -f "sudo.*vehicle_control" 2>/dev/null || true
	@-sudo pkill -KILL -f "nohup.*vehicle_control" 2>/dev/null || true
	@echo "强制停止UDP程序..."
	@-sudo pkill -KILL -f "video_receiver" 2>/dev/null || true
	@echo "强制停止PCIe程序..."
	@-sudo pkill -KILL -f "build/app" 2>/dev/null || true
	@# 清理PID文件
	@rm -f pcie/pcie.pid udp/udp.pid fspi/fspi.pid
	@# 清理共享内存和信号量
	@echo "强制清理共享内存..."
	@-sudo ipcrm -M 0x12345679 2>/dev/null || true
	@-sudo ipcrm -M 0x1234567A 2>/dev/null || true
	@-sudo ipcrm -M 0x1234567C 2>/dev/null || true
	@-sudo ipcrm -M 0x1234567D 2>/dev/null || true
	@echo "强制清理信号量..."
	@-sudo ipcrm -S 0x12345679 2>/dev/null || true
	@-sudo ipcrm -S 0x1234567A 2>/dev/null || true
	@-sudo ipcrm -S 0x1234567C 2>/dev/null || true
	@-sudo ipcrm -S 0x1234567D 2>/dev/null || true
	@echo "强制停止完成"

# =============================================================================
# 编译目标
# =============================================================================

# 创建构建目录
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# 编译共享库
shared: $(BUILD_DIR)/$(SHARED_LIB)

$(BUILD_DIR)/$(SHARED_LIB): $(SHARED_OBJECTS) | $(BUILD_DIR)
	@echo "创建共享库: $@"
	$(AR) $(ARFLAGS) $@ $^

# 编译共享库对象文件
$(BUILD_DIR)/%.o: $(SHARED_SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "编译共享库: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# 编译PCIe程序
pcie: shared
	@echo "=== 编译PCIe图像采集程序 ==="
	@# 将共享库复制到pcie目录
	@cp $(BUILD_DIR)/$(SHARED_LIB) pcie/$(SHARED_LIB)
	@# 创建include软链接
	@if [ ! -L pcie/include ]; then \
		cd pcie && ln -sf ../$(SHARED_INC_DIR) include; \
	fi
	@# 编译PCIe程序
	@$(MAKE) -C pcie SHARED_LIB=../$(BUILD_DIR)/$(SHARED_LIB)
	@echo "PCIe程序编译完成: $(PCIE_TARGET)"

# 编译UDP程序
udp: shared
	@echo "=== 编译UDP网络接收后台程序 ==="
	@# 将共享库复制到udp目录
	@cp $(BUILD_DIR)/$(SHARED_LIB) udp/$(SHARED_LIB)
	@# 创建include软链接
	@if [ ! -L udp/include ]; then \
		cd udp && ln -sf ../$(SHARED_INC_DIR) include; \
	fi
	@# 编译UDP程序
	@$(MAKE) -C udp SHARED_LIB=../$(BUILD_DIR)/$(SHARED_LIB)
	@echo "UDP程序编译完成: $(UDP_TARGET)"

# 编译X11程序
x11: shared
	@echo "=== 编译X11八摄像头渲染程序 ==="
	@# 编译X11程序
	@$(MAKE) -C x11/csrc SHARED_LIB=../../$(BUILD_DIR)/$(SHARED_LIB)
	@echo "X11程序编译完成: $(X11_TARGET)"

# 编译FSPI程序
fspi:
	@echo "=== 编译FSPI车辆控制程序 ==="
	@# FSPI程序是独立的，不需要共享库
	@$(MAKE) -C fspi
	@echo "FSPI程序编译完成: $(FSPI_TARGET)"

# =============================================================================
# 清理目标
# =============================================================================

# 清理所有编译产物
clean:
	@echo "=== 清理编译产物 ==="
	@rm -rf $(BUILD_DIR)
	@$(MAKE) -C pcie clean
	@$(MAKE) -C udp clean
	@$(MAKE) -C x11/csrc clean
	@$(MAKE) -C fspi clean
	@rm -f pcie/$(SHARED_LIB) udp/$(SHARED_LIB)
	@echo "清理完成"

# 深度清理（包括共享内存）
distclean: clean stop
	@echo "=== 深度清理 ==="
	@echo "清理完成"

# =============================================================================
# 辅助目标
# =============================================================================

# 检查系统依赖
check:
	@echo "=== 系统环境检查 ==="
	@echo "系统: $(UNAME) $(ARCH)"
	@echo "编译器:"
	@which $(CC) || (echo "错误: 未找到gcc编译器"; exit 1)
	@which $(CXX) || (echo "错误: 未找到g++编译器"; exit 1)
	@echo "库检查:"
	@pkg-config --exists x11 || (echo "警告: 未找到X11开发库"; exit 1)
	@echo "权限检查:"
	@if [ ! -w /dev/shm ]; then echo "警告: 无法写入/dev/shm"; fi
	@echo "环境检查完成"

# 安装系统依赖（Debian/Ubuntu）
deps:
	@echo "=== 安装系统依赖 ==="
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "检测到apt包管理器，安装依赖..."; \
		sudo apt-get update; \
		sudo apt-get install -y build-essential libx11-dev libxext-dev; \
	elif command -v yum >/dev/null 2>&1; then \
		echo "检测到yum包管理器，安装依赖..."; \
		sudo yum install -y gcc gcc-c++ libX11-devel libXext-devel; \
	else \
		echo "未识别的包管理器，请手动安装以下依赖:"; \
		echo "- build-essential (gcc, g++, make)"; \
		echo "- libx11-dev"; \
		echo "- libxext-dev"; \
	fi

# 显示状态信息
status:
	@echo "=== 系统状态 ==="
	@echo "编译状态:"
	@if [ -f "$(PCIE_TARGET)" ]; then \
		echo "✓ PCIe程序: $(PCIE_TARGET)"; \
		ls -lh $(PCIE_TARGET); \
	else \
		echo "✗ PCIe程序: 未编译"; \
	fi
	@if [ -f "$(UDP_TARGET)" ]; then \
		echo "✓ UDP程序: $(UDP_TARGET)"; \
		ls -lh $(UDP_TARGET); \
	else \
		echo "✗ UDP程序: 未编译"; \
	fi
	@if [ -f "$(FSPI_TARGET)" ]; then \
		echo "✓ FSPI程序: $(FSPI_TARGET)"; \
		ls -lh $(FSPI_TARGET); \
	else \
		echo "✗ FSPI程序: 未编译"; \
	fi
	@echo ""
	@echo "运行状态:"
	@PCIE_PIDS=$$(pgrep -f "build/app" 2>/dev/null || true); \
	if [ ! -z "$$PCIE_PIDS" ]; then \
		echo "✓ PCIe程序运行中 (PIDs: $$PCIE_PIDS)"; \
	else \
		echo "✗ PCIe程序未运行"; \
	fi
	@UDP_PIDS=$$(pgrep -f "video_receiver" 2>/dev/null || true); \
	if [ ! -z "$$UDP_PIDS" ]; then \
		echo "✓ UDP程序运行中 (PIDs: $$UDP_PIDS)"; \
	else \
		echo "✗ UDP程序未运行"; \
	fi
	@FSPI_PIDS=$$(pgrep -f "vehicle_control" 2>/dev/null || true); \
	if [ ! -z "$$FSPI_PIDS" ]; then \
		echo "✓ FSPI程序运行中 (PIDs: $$FSPI_PIDS)"; \
	else \
		echo "✗ FSPI程序未运行"; \
	fi
	@X11_PIDS=$$(pgrep -f "eight_camera_renderer" 2>/dev/null || true); \
	if [ ! -z "$$X11_PIDS" ]; then \
		echo "✓ X11程序运行中 (PIDs: $$X11_PIDS)"; \
	else \
		echo "✗ X11程序未运行"; \
	fi
	@echo ""
	@echo "共享内存状态:"
	@PCIE_SHM=$$(ipcs -m | grep 12345679 2>/dev/null || true); \
	if [ ! -z "$$PCIE_SHM" ]; then \
		echo "✓ PCIe共享内存存在"; \
	else \
		echo "✗ PCIe共享内存不存在"; \
	fi
	@UDP_SHM=$$(ipcs -m | grep 1234567A 2>/dev/null || true); \
	if [ ! -z "$$UDP_SHM" ]; then \
		echo "✓ UDP共享内存存在"; \
	else \
		echo "✗ UDP共享内存不存在"; \
	fi
	@CTRL_SHM=$$(ipcs -m | grep 1234567C 2>/dev/null || true); \
	if [ ! -z "$$CTRL_SHM" ]; then \
		echo "✓ 控制共享内存存在"; \
	else \
		echo "✗ 控制共享内存不存在"; \
	fi
	@BRAKE_SHM=$$(ipcs -m | grep 1234567D 2>/dev/null || true); \
	if [ ! -z "$$BRAKE_SHM" ]; then \
		echo "✓ 紧急制动共享内存存在"; \
	else \
		echo "✗ 紧急制动共享内存不存在"; \
	fi

# 快速测试编译
test: all
	@echo "=== 编译测试完成 ==="
	@echo "所有程序编译成功！"
	@echo ""
	@echo "程序列表:"
	@ls -lh $(PCIE_TARGET) $(UDP_TARGET) $(FSPI_TARGET) $(X11_TARGET)
	@echo ""
	@echo "使用 'make run' 启动完整系统"

# 显示帮助信息
help:
	@echo "=== FPGA八摄像头视频显示系统 Makefile 帮助 ==="
	@echo ""
	@echo "主要目标:"
	@echo "  all            - 编译所有程序（默认目标）"
	@echo "  run            - 编译并运行整个系统（后台+显示）"
	@echo "  run-background - 只启动后台程序（PCIe + UDP）"
	@echo "  run-display    - 只启动显示程序（X11）"
	@echo "  run-fspi       - 只启动车辆控制程序（FSPI）"
	@echo "  stop           - 停止运行中的系统"
	@echo "  stop-force     - 强制停止所有进程（如果stop无效时使用）"
	@echo "  clean          - 清理编译产物"
	@echo ""
	@echo "编译目标:"
	@echo "  shared    - 编译共享库"
	@echo "  pcie      - 编译PCIe图像采集程序"
	@echo "  udp       - 编译UDP网络接收程序"
	@echo "  x11       - 编译X11八摄像头渲染程序"
	@echo "  fspi      - 编译FSPI车辆控制程序"
	@echo ""
	@echo "辅助目标:"
	@echo "  check     - 检查系统环境"
	@echo "  deps      - 安装系统依赖"
	@echo "  status    - 显示系统状态"
	@echo "  distclean - 深度清理"
	@echo "  help      - 显示此帮助信息"
	@echo ""
	@echo "典型工作流程:"
	@echo "  make run               # 一键启动完整系统 (推荐)"
	@echo "  make stop              # 停止所有程序"
	@echo "  make stop-force        # 强制停止所有程序 (如果stop失败时使用)"
	@echo ""
	@echo "高级使用:"
	@echo "  1. make run-background  # 启动数据采集和接收"
	@echo "  2. make run-fspi        # 启动按钮控制程序"
	@echo "  3. make run-display     # 启动显示程序"
	@echo "  4. make stop           # 停止所有程序"
	@echo ""
	@echo "系统架构:"
	@echo "  - PCIe程序: 采集4摄像头图像→YOLO人员检测→写入PCIe共享内存+紧急制动共享内存"
	@echo "  - UDP程序:  接收4摄像头网络数据→写入UDP共享内存"
	@echo "  - FSPI程序: 读取模式值→写入控制共享内存→视角切换"
	@echo "             读取紧急制动共享内存→寄存器4控制→紧急制动"
	@echo "  - X11程序:  读取三个共享内存→根据模式渲染640x480或1280x480显示"
	@echo ""
	@echo "视角切换:"
	@echo "  - 读取0x11-0x88: 对应摄像头单独显示"
	@echo "  - 读取0x99: 全部8个摄像头显示"

# =============================================================================
# 依赖关系
# =============================================================================

# 确保共享库在程序之前编译
$(PCIE_TARGET): shared
$(UDP_TARGET): shared

# 共享库依赖于源文件
$(BUILD_DIR)/$(SHARED_LIB): $(SHARED_OBJECTS)

# 对象文件依赖于头文件
$(SHARED_OBJECTS): $(wildcard $(SHARED_INC_DIR)/*.h)