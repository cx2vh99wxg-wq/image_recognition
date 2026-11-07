# =============================================================================
# FPGA六摄像头视频呈现系统主Makefile
# 说明：作者: 科研轮转
# 说明：本版更新：2026年9月24日
# 功能: 构建PCIe和UDP程序，提供六个物理摄像头独立呈现功能
# =============================================================================

# 项目设置
SHARED_LIB = libshared.a
PCIE_TARGET = pcie/build/app
UDP_TARGET = udp/build/video_receiver
X11_TARGET = x11/csrc/build/eight_camera_renderer
FSPI_TARGET = fspi/build/vehicle_control
BRAKE_RELEASE_TEST = $(BUILD_DIR)/test_brake_release_policy

# 构建器设置
CXX = g++
CC = gcc
AR = ar

# 展开清理命令，同时避免 dry-run 把整条显示配方当成递归 make 执行。
RUN_CLEANUP_MAKE = $(MAKE)

# 工程路径约定
SHARED_SRC_DIR = csrc
SHARED_INC_DIR = include
BUILD_DIR = build

# 说明：共享库源文件
SHARED_CSRCS = $(wildcard $(SHARED_SRC_DIR)/*.c)
SHARED_OBJECTS = $(patsubst $(SHARED_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SHARED_CSRCS))

# 构建标志
INCFLAGS = -I$(SHARED_INC_DIR)
CFLAGS = $(INCFLAGS) -std=c99 -O3 -march=native -mtune=cortex-a55 -funroll-loops -pthread -fPIC -MMD -MP
ARFLAGS = rcs

# 说明：获取系统信息用于调试
UNAME := $(shell uname -s)
ARCH := $(shell uname -m)

# =============================================================================
# 说明：主要目标
# =============================================================================

.PHONY: all clean pcie udp x11 fspi shared run run-background run-display run-fspi stop stop-force help install deps check test-brake-release

# 默认目标：构建所有程序
all: shared pcie udp x11 fspi
	@echo "=== 构建结束 ==="
	@echo "说明：PCIe程序:  $(PCIE_TARGET)"
	@echo "说明：UDP程序:   $(UDP_TARGET)"
	@echo "说明：X11程序:   $(X11_TARGET)"
	@echo "说明：FSPI程序:  $(FSPI_TARGET)"
	@echo "说明：共享库:    $(BUILD_DIR)/$(SHARED_LIB)"
	@echo ""
	@echo "使用 'make run-background' 运行后台程序 (PCIe + UDP)"
	@echo "使用 'make run-display' 运行呈现程序 (X11)"
	@echo "使用 'make run-fspi' 运行车辆控制程序 (FSPI)"
	@echo "使用 'make run' 运行完整系统"
	@echo "使用 'make stop' 结束系统"

# 运行后台程序（PCIe + UDP）
run-background: all tune-network
	@echo "=== 运行FPGA双通道后台程序 ==="
	@echo "说明：系统: $(UNAME) $(ARCH)"
	@echo "说明：时间: $(shell date)"
	@echo ""
	@# 说明：检查程序是否存在
	@if [ ! -f "$(PCIE_TARGET)" ]; then \
		echo "异常: PCIe程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(UDP_TARGET)" ]; then \
		echo "异常: UDP程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@# 移除之前的共享内存
	@echo "移除共享内存..."
	@-ipcrm -M 0x12345679 2>/dev/null || true
	@-ipcrm -M 0x1234567A 2>/dev/null || true
	@-ipcrm -M 0x1234567C 2>/dev/null || true
	@-ipcrm -M 0x1234567D 2>/dev/null || true
	@-ipcrm -S 0x12345679 2>/dev/null || true
	@-ipcrm -S 0x1234567A 2>/dev/null || true
	@-ipcrm -S 0x1234567C 2>/dev/null || true
	@-ipcrm -S 0x1234567D 2>/dev/null || true
	@echo "共享内存移除完成"
	@echo ""
	@# 运行PCIe后台程序
	@echo "运行PCIe图像采集后台程序..."
	@echo "说明：运行PCIe程序需要sudo权限..."
	@cd pcie && sudo ./build/app &
	@echo "PCIe程序已运行，等待初始化..."
	@sleep 3
	@echo ""
	@# 运行UDP后台程序
	@echo "运行UDP网络收取后台程序..."
	@cd udp && ./build/video_receiver &
	@echo "UDP程序已运行，等待初始化..."
	@sleep 2
	@echo ""
	@echo "后台程序运行完成！"
	@echo "现在可以运行 'make run-display' 运行呈现程序"

# 运行车辆控制程序
run-fspi: all
	@echo "=== 运行FSPI车辆控制程序 ==="
	@if [ ! -f "$(FSPI_TARGET)" ]; then \
		echo "异常: FSPI程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@echo "运行车辆控制程序..."
	@echo "控制按键: w=前进, s=后退, a=左转, d=右转, x=结束, q=退出"
	@echo "说明：注意: 需要管理员权限访问SPI设备"
	@echo ""
	@cd fspi && sudo python3 ../steering/tools/launch_vehicle_control.py

# 运行呈现程序
run-display: all
	@echo "=== 运行X11六摄像头呈现程序 ==="
	@if [ ! -f "$(X11_TARGET)" ]; then \
		echo "异常: X11程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@echo "运行六摄像头渲染呈现..."
	@echo "呈现模式: 6个物理摄像头 (22/33/44/66/77/88) 或六路全景 (99)"
	@echo "说明：操作: 按Q键、Esc键或关闭窗口退出"
	@echo ""
	@cd x11/csrc && ./build/eight_camera_renderer

# 运行完整系统（后台 + FSPI控制 + 呈现）
run: all tune-network
	@echo "=== 运行FPGA六摄像头视频呈现系统 (带视角切换) ==="
	@echo "说明：系统: $(UNAME) $(ARCH)"
	@echo "说明：时间: $(shell date)"
	@echo ""
	@echo "说明：功能说明:"
	@echo "- PCIe图像采集 + YOLO人员识别 + UDP网络收取"
	@echo "- FSPI模式识别视角切换 + 紧急制动控制"
	@echo "- X11实时呈现 (支持6个单视角和1个全景模式)"
	@echo "- 模式控制: 22/33/44为后左/后右/后中，66/77/88为前右/前左/前中，99呈现全景"
	@echo "- 紧急制动: PCIe识别到人员时→FSPI寄存器4写0→紧急制动"
	@echo ""
	@# 说明：检查程序是否存在
	@if [ ! -f "$(PCIE_TARGET)" ]; then \
		echo "异常: PCIe程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(UDP_TARGET)" ]; then \
		echo "异常: UDP程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(FSPI_TARGET)" ]; then \
		echo "异常: FSPI程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(X11_TARGET)" ]; then \
		echo "异常: X11程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@# 交互终端的 .bashrc 可能再次执行 make run；已有后台任务时直接拒绝重复运行。
	@if pgrep -x vehicle_control >/dev/null || pgrep -x video_receiver >/dev/null || pgrep -x app >/dev/null; then \
		echo "异常: S端系统已有运行进程，请先关闭旧呈现窗口并执行 make stop 后再运行"; \
		exit 1; \
	fi
	@# 移除之前的共享内存
	@echo "移除共享内存..."
	@-ipcrm -M 0x12345679 2>/dev/null || true
	@-ipcrm -M 0x1234567A 2>/dev/null || true
	@-ipcrm -M 0x1234567C 2>/dev/null || true
	@-ipcrm -M 0x1234567D 2>/dev/null || true
	@-ipcrm -S 0x12345679 2>/dev/null || true
	@-ipcrm -S 0x1234567A 2>/dev/null || true
	@-ipcrm -S 0x1234567C 2>/dev/null || true
	@-ipcrm -S 0x1234567D 2>/dev/null || true
	@echo "共享内存移除完成"
	@echo ""
	@# 运行PCIe后台程序
	@echo "1. 运行PCIe图像采集程序..."
	@cd pcie && sudo ./build/app & echo $$! > pcie.pid
	@echo "PCIe程序已运行 (PID: $(shell cat pcie/pcie.pid 2>/dev/null || echo 'unknown'))"
	@sleep 3
	@echo ""
	@# 运行UDP后台程序
	@echo "2. 运行UDP网络收取程序..."
	@cd udp && ./build/video_receiver & echo $$! > udp.pid
	@echo "UDP程序已运行 (PID: $(shell cat udp/udp.pid 2>/dev/null || echo 'unknown'))"
	@sleep 2
	@echo ""
	@# 运行FSPI控制程序
	@echo "3. 运行FSPI按钮控制程序..."
	@cd fspi && sudo bash -c 'python3 ../steering/tools/launch_vehicle_control.py & echo $$! > fspi.pid'
	@sleep 3
	@fspi_pid=$$(cat fspi/fspi.pid 2>/dev/null); \
		case "$$fspi_pid" in ''|*[!0-9]*) echo "异常: FSPI PID 无效，自动刹停不可用"; exit 1;; esac; \
		if ! ps -p "$$fspi_pid" -o stat=,comm= 2>/dev/null | awk '$$1 !~ /^Z/ && $$2 == "vehicle_control" { running=1 } END { exit !running }'; then \
			echo "异常: FSPI车控进程未运行，自动刹停不可用 (PID=$$fspi_pid)"; \
			exit 1; \
		fi; \
		echo "说明：FSPI车控程序已运行 (PID=$$fspi_pid)"
	@echo "说明：模式控制: 读取六视角编号及99全景；11/55兼容归并到中间视角"
	@echo ""
	@# 运行X11呈现程序（前台运行）
	@echo "4. 运行X11呈现程序（前台运行）..."
	@echo "呈现窗口: 640x480 (单摄像头) 或 1280x480 (全部摄像头)"
	@echo "呈现模式: 6个物理摄像头 (22/33/44/66/77/88) 或六路全景 (99)"
	@echo "说明：操作: 按Q键、Esc键或关闭窗口退出"
	@echo "退出时将自动结束所有后台程序"
	@echo ""
	@# 设置陷阱，确保退出时移除
	@trap '$(RUN_CLEANUP_MAKE) -C "$(CURDIR)" stop' EXIT; cd x11/csrc && ./build/eight_camera_renderer

# 结束系统
stop:
	@echo "=== 结束FPGA双通道视频呈现系统 ==="
	@# 说明：用完整命令行锚定目标，避免 pkill 匹配到执行本命令的 shell。
	@echo "结束X11程序..."
	@-pkill -TERM -f '^./build/eight_camera_renderer$$' 2>/dev/null || true
	@sleep 1
	@# 结束FSPI程序
	@echo "结束FSPI程序..."
	@-sudo pkill -TERM -x vehicle_control 2>/dev/null || true
	@sleep 2
	@-sudo pkill -KILL -x vehicle_control 2>/dev/null || true
	@rm -f fspi/fspi.pid
	@# 结束UDP程序
	@echo "结束UDP程序..."
	@-pkill -TERM -f '^./build/video_receiver$$' 2>/dev/null || true
	@rm -f udp/udp.pid
	@# 结束PCIe程序
	@echo "结束PCIe程序..."
	@-sudo pkill -TERM -f '^./build/app$$' 2>/dev/null || true
	@sleep 1
	@-sudo pkill -KILL -f '^./build/app$$' 2>/dev/null || true
	@rm -f pcie/pcie.pid
	@# 移除共享内存和信号量
	@echo "移除共享内存..."
	@-ipcrm -M 0x12345679 2>/dev/null || true
	@-ipcrm -M 0x1234567A 2>/dev/null || true
	@-ipcrm -M 0x1234567C 2>/dev/null || true
	@-ipcrm -M 0x1234567D 2>/dev/null || true
	@echo "移除信号量..."
	@-ipcrm -S 0x12345679 2>/dev/null || true
	@-ipcrm -S 0x1234567A 2>/dev/null || true
	@-ipcrm -S 0x1234567C 2>/dev/null || true
	@-ipcrm -S 0x1234567D 2>/dev/null || true
	@echo "系统结束完成"

# 强制结束系统（使用SIGKILL）
stop-force:
	@echo "=== 强制结束FPGA双通道视频呈现系统 ==="
	@echo "注意: 使用SIGKILL强制终止所有进程"
	@# 强制结束所有相关进程
	@echo "强制结束X11程序..."
	@-sudo pkill -KILL -f '^./build/eight_camera_renderer$$' 2>/dev/null || true
	@echo "强制结束FSPI程序..."
	@-sudo pkill -KILL -x vehicle_control 2>/dev/null || true
	@echo "强制结束UDP程序..."
	@-sudo pkill -KILL -f '^./build/video_receiver$$' 2>/dev/null || true
	@echo "强制结束PCIe程序..."
	@-sudo pkill -KILL -f '^./build/app$$' 2>/dev/null || true
	@# 移除PID文件
	@rm -f pcie/pcie.pid udp/udp.pid fspi/fspi.pid
	@# 移除共享内存和信号量
	@echo "强制移除共享内存..."
	@-sudo ipcrm -M 0x12345679 2>/dev/null || true
	@-sudo ipcrm -M 0x1234567A 2>/dev/null || true
	@-sudo ipcrm -M 0x1234567C 2>/dev/null || true
	@-sudo ipcrm -M 0x1234567D 2>/dev/null || true
	@echo "强制移除信号量..."
	@-sudo ipcrm -S 0x12345679 2>/dev/null || true
	@-sudo ipcrm -S 0x1234567A 2>/dev/null || true
	@-sudo ipcrm -S 0x1234567C 2>/dev/null || true
	@-sudo ipcrm -S 0x1234567D 2>/dev/null || true
	@echo "强制结束完成"

# =============================================================================
# 构建目标
# =============================================================================

# 建立产物目录
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# 构建共享库
shared: $(BUILD_DIR)/$(SHARED_LIB)

# 说明：行人刹停释放策略单测无需摄像头、SPI 或 FPGA 设备。
$(BRAKE_RELEASE_TEST): test_brake_release_policy.c $(SHARED_INC_DIR)/brake_release_policy.h | $(BUILD_DIR)
	$(CC) -std=c99 -Wall -Wextra -Werror $(INCFLAGS) $< -o $@

test-brake-release: $(BRAKE_RELEASE_TEST)
	./$(BRAKE_RELEASE_TEST)

$(BUILD_DIR)/$(SHARED_LIB): $(SHARED_OBJECTS) | $(BUILD_DIR)
	@echo "建立共享库: $@"
	$(AR) $(ARFLAGS) $@ $^

# 构建共享库对象文件
$(BUILD_DIR)/shared_memory.o: $(SHARED_INC_DIR)/shared_memory.h $(SHARED_INC_DIR)/brake_release_policy.h ../common/perception/include/curve_shm_attach.h ../common/image/frame_checksum.h

.PHONY: test-checksum
$(BUILD_DIR)/test_frame_checksum: ../common/image/tests/test_frame_checksum.cpp ../common/image/frame_checksum.h | $(BUILD_DIR)
	$(CXX) -std=c++11 -O3 -mtune=cortex-a55 -Wall -Wextra -Werror $< -o $@

test-checksum: $(BUILD_DIR)/test_frame_checksum
	./$(BUILD_DIR)/test_frame_checksum

$(BUILD_DIR)/%.o: $(SHARED_SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "构建共享库: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# 构建PCIe程序
pcie: shared
	@echo "=== 构建PCIe图像采集程序 ==="
	@# 说明：将共享库复制到pcie目录
	@cp $(BUILD_DIR)/$(SHARED_LIB) pcie/$(SHARED_LIB)
	@# 建立include软链接
	@if [ ! -L pcie/include ]; then \
		cd pcie && ln -sf ../$(SHARED_INC_DIR) include; \
	fi
	@# 构建PCIe程序
	@$(MAKE) -C pcie SHARED_LIB=../$(BUILD_DIR)/$(SHARED_LIB)
	@echo "PCIe程序构建结束: $(PCIE_TARGET)"

# 构建UDP程序
udp: shared
	@echo "=== 构建UDP网络收取后台程序 ==="
	@# 说明：将共享库复制到udp目录
	@cp $(BUILD_DIR)/$(SHARED_LIB) udp/$(SHARED_LIB)
	@# 建立include软链接
	@if [ ! -L udp/include ]; then \
		cd udp && ln -sf ../$(SHARED_INC_DIR) include; \
	fi
	@# 构建UDP程序
	@$(MAKE) -C udp SHARED_LIB=../$(BUILD_DIR)/$(SHARED_LIB)
	@echo "UDP程序构建结束: $(UDP_TARGET)"

# 构建X11程序
x11: shared
	@echo "=== 构建X11六摄像头渲染程序 ==="
	@# 构建X11程序
	@$(MAKE) -C x11/csrc SHARED_LIB=../../$(BUILD_DIR)/$(SHARED_LIB)
	@echo "X11程序构建结束: $(X11_TARGET)"

# 构建FSPI程序
fspi:
	@echo "=== 构建FSPI车辆控制程序 ==="
	@# 说明：FSPI程序是独立的，不需要共享库
	@$(MAKE) -C fspi
	@echo "FSPI程序构建结束: $(FSPI_TARGET)"

# =============================================================================
# 移除目标
# =============================================================================

# 移除所有构建产物
clean:
	@echo "=== 移除构建产物 ==="
	@rm -rf $(BUILD_DIR)
	@$(MAKE) -C pcie clean
	@$(MAKE) -C udp clean
	@$(MAKE) -C x11/csrc clean
	@$(MAKE) -C fspi clean
	@rm -f pcie/$(SHARED_LIB) udp/$(SHARED_LIB)
	@echo "移除完成"

# 深度移除（包括共享内存）
distclean: clean stop
	@echo "=== 深度移除 ==="
	@echo "移除完成"

# =============================================================================
# 说明：辅助目标
# =============================================================================

# 说明：检查系统依赖
check:
	@echo "=== 说明：系统环境检查 ==="
	@echo "说明：系统: $(UNAME) $(ARCH)"
	@echo "构建器:"
	@which $(CC) || (echo "异常: 未定位到gcc构建器"; exit 1)
	@which $(CXX) || (echo "异常: 未定位到g++构建器"; exit 1)
	@echo "说明：库检查:"
	@pkg-config --exists x11 || (echo "注意: 未定位到X11开发库"; exit 1)
	@echo "说明：权限检查:"
	@if [ ! -w /dev/shm ]; then echo "注意: 无法写入/dev/shm"; fi
	@echo "说明：环境检查完成"

# 说明：安装系统依赖（Debian/Ubuntu）
deps:
	@echo "=== 说明：安装系统依赖 ==="
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "识别到apt包管理器，安装依赖..."; \
		sudo apt-get update; \
		sudo apt-get install -y build-essential libx11-dev libxext-dev; \
	elif command -v yum >/dev/null 2>&1; then \
		echo "识别到yum包管理器，安装依赖..."; \
		sudo yum install -y gcc gcc-c++ libX11-devel libXext-devel; \
	else \
		echo "说明：未识别的包管理器，请手动安装以下依赖:"; \
		echo "- build-essential (gcc, g++, make)"; \
		echo "- libx11-dev"; \
		echo "- libxext-dev"; \
	fi

# 呈现状态信息
status:
	@echo "=== 说明：系统状态 ==="
	@echo "构建状态:"
	@if [ -f "$(PCIE_TARGET)" ]; then \
		echo "说明：✓ PCIe程序: $(PCIE_TARGET)"; \
		ls -lh $(PCIE_TARGET); \
	else \
		echo "✗ PCIe程序: 未构建"; \
	fi
	@if [ -f "$(UDP_TARGET)" ]; then \
		echo "说明：✓ UDP程序: $(UDP_TARGET)"; \
		ls -lh $(UDP_TARGET); \
	else \
		echo "✗ UDP程序: 未构建"; \
	fi
	@if [ -f "$(FSPI_TARGET)" ]; then \
		echo "说明：✓ FSPI程序: $(FSPI_TARGET)"; \
		ls -lh $(FSPI_TARGET); \
	else \
		echo "✗ FSPI程序: 未构建"; \
	fi
	@echo ""
	@echo "说明：运行状态:"
	@PCIE_PIDS=$$(pgrep -f "build/app" 2>/dev/null || true); \
	if [ ! -z "$$PCIE_PIDS" ]; then \
		echo "说明：✓ PCIe程序运行中 (PIDs: $$PCIE_PIDS)"; \
	else \
		echo "说明：✗ PCIe程序未运行"; \
	fi
	@UDP_PIDS=$$(pgrep -f "video_receiver" 2>/dev/null || true); \
	if [ ! -z "$$UDP_PIDS" ]; then \
		echo "说明：✓ UDP程序运行中 (PIDs: $$UDP_PIDS)"; \
	else \
		echo "说明：✗ UDP程序未运行"; \
	fi
	@FSPI_PIDS=$$(pgrep -f "vehicle_control" 2>/dev/null || true); \
	if [ ! -z "$$FSPI_PIDS" ]; then \
		echo "说明：✓ FSPI程序运行中 (PIDs: $$FSPI_PIDS)"; \
	else \
		echo "说明：✗ FSPI程序未运行"; \
	fi
	@X11_PIDS=$$(pgrep -f "eight_camera_renderer" 2>/dev/null || true); \
	if [ ! -z "$$X11_PIDS" ]; then \
		echo "说明：✓ X11程序运行中 (PIDs: $$X11_PIDS)"; \
	else \
		echo "说明：✗ X11程序未运行"; \
	fi
	@echo ""
	@echo "说明：共享内存状态:"
	@PCIE_SHM=$$(ipcs -m | grep 12345679 2>/dev/null || true); \
	if [ ! -z "$$PCIE_SHM" ]; then \
		echo "说明：✓ PCIe共享内存存在"; \
	else \
		echo "说明：✗ PCIe共享内存不存在"; \
	fi
	@UDP_SHM=$$(ipcs -m | grep 1234567A 2>/dev/null || true); \
	if [ ! -z "$$UDP_SHM" ]; then \
		echo "说明：✓ UDP共享内存存在"; \
	else \
		echo "说明：✗ UDP共享内存不存在"; \
	fi
	@CTRL_SHM=$$(ipcs -m | grep 1234567C 2>/dev/null || true); \
	if [ ! -z "$$CTRL_SHM" ]; then \
		echo "说明：✓ 控制共享内存存在"; \
	else \
		echo "说明：✗ 控制共享内存不存在"; \
	fi
	@BRAKE_SHM=$$(ipcs -m | grep 1234567D 2>/dev/null || true); \
	if [ ! -z "$$BRAKE_SHM" ]; then \
		echo "说明：✓ 紧急制动共享内存存在"; \
	else \
		echo "说明：✗ 紧急制动共享内存不存在"; \
	fi

# 快速验证构建
test: all test-brake-release
	@echo "=== 构建验证完成 ==="
	@echo "所有程序构建通过！"
	@echo ""
	@echo "说明：程序列表:"
	@ls -lh $(PCIE_TARGET) $(UDP_TARGET) $(FSPI_TARGET) $(X11_TARGET)
	@echo ""
	@echo "使用 'make run' 运行完整系统"

# 呈现用法信息
help:
	@echo "=== FPGA六摄像头视频呈现系统 Makefile 用法 ==="
	@echo ""
	@echo "说明：主要目标:"
	@echo "  all            - 构建所有程序（默认目标）"
	@echo "  run            - 构建并运行整个系统（后台+呈现）"
	@echo "  run-background - 只运行后台程序（PCIe + UDP）"
	@echo "  run-display    - 只运行呈现程序（X11）"
	@echo "  run-fspi       - 只运行车辆控制程序（FSPI）"
	@echo "  stop           - 结束运行中的系统"
	@echo "  stop-force     - 强制结束所有进程（如果stop无效时使用）"
	@echo "  clean          - 移除构建产物"
	@echo ""
	@echo "构建目标:"
	@echo "  shared    - 构建共享库"
	@echo "  pcie      - 构建PCIe图像采集程序"
	@echo "  udp       - 构建UDP网络收取程序"
	@echo "  x11       - 构建X11六摄像头渲染程序"
	@echo "  fspi      - 构建FSPI车辆控制程序"
	@echo ""
	@echo "说明：辅助目标:"
	@echo "  说明：check     - 检查系统环境"
	@echo "  说明：deps      - 安装系统依赖"
	@echo "  status    - 呈现系统状态"
	@echo "  distclean - 深度移除"
	@echo "  help      - 呈现此用法信息"
	@echo ""
	@echo "说明：典型工作流程:"
	@echo "  make run               # 一键启动完整系统 (推荐)"
	@echo "  make stop              # 停止所有程序"
	@echo "  make stop-force        # 强制停止所有程序 (如果stop失败时使用)"
	@echo ""
	@echo "说明：高级使用:"
	@echo "  1. make run-background  # 运行数据采集和收取"
	@echo "  2. make run-fspi        # 运行按钮控制程序"
	@echo "  3. make run-display     # 运行呈现程序"
	@echo "  4. make stop           # 结束所有程序"
	@echo ""
	@echo "说明：系统架构:"
	@echo "  - PCIe程序: 采集S端3路有效图像（保留四分区传输封装）→YOLO人员识别→写入PCIe共享内存+紧急制动共享内存"
	@echo "  - UDP程序:  收取M端3路有效图像（保留四分区传输封装）→写入UDP共享内存"
	@echo "  - FSPI程序: 读取模式值→写入控制共享内存→视角切换"
	@echo "             说明：读取紧急制动共享内存→寄存器4控制→紧急制动"
	@echo "  - X11程序:  读取三个共享内存→根据模式渲染640x480或1280x480呈现"
	@echo ""
	@echo "说明：视角切换:"
	@echo "  - 读取22/33/44/66/77/88: 六个物理视角单独呈现"
	@echo "  - 读取0x99: 六个物理摄像头3x2全景呈现"

# =============================================================================
# 跟踪头文件变动
# =============================================================================

# 确保共享库在程序之前构建
$(PCIE_TARGET): shared
$(UDP_TARGET): shared

# 说明：共享库依赖于源文件
$(BUILD_DIR)/$(SHARED_LIB): $(SHARED_OBJECTS)

# 说明：对象文件依赖于头文件
$(SHARED_OBJECTS): $(wildcard $(SHARED_INC_DIR)/*.h)

# 说明：Two complete raw frames need more than the kernel's 212 KiB default limit.
# 说明：Raise limits only; VISION_NET_TUNE=0 leaves the host settings unchanged.
.PHONY: tune-network
tune-network:
	@if [ "$${VISION_NET_TUNE:-1}" != "0" ]; then \
		for setting in rmem_max wmem_max; do \
			current=$$(cat /proc/sys/net/core/$$setting 2>/dev/null || echo 0); \
			if [ "$$current" -lt 4194304 ]; then \
				if [ "$$(id -u)" = 0 ]; then \
					sysctl -q -w net.core.$$setting=4194304; \
				else \
					sudo -n sysctl -q -w net.core.$$setting=4194304 || \
					echo "Video socket limit unchanged: run make tune-network as root"; \
				fi; \
			fi; \
		done; \
	fi


# 说明：Shared headers are authoritative; rebuild their consumers when they change.
-include $(SHARED_OBJECTS:.o=.d)
