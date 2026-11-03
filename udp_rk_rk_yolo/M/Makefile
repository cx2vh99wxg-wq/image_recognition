# 说明：Makefile for M-End Project
# Maintainer: 科研轮转
# 本版更新：2026年10月30日
# 说明：Description: 管理PCIe模块和UDP模块的构建

#############################################################################
# 目标、源码及产物路径
#############################################################################
PROJECT_NAME = M-End_FPGA_System
TOPDIR = $(shell pwd)

# 说明：Same choice for normal, background and direct PCIe launch commands.
LANE_CURVE_MODE ?= ego-boundaries
ifneq ($(words $(LANE_CURVE_MODE)),1)
$(error LANE_CURVE_MODE must be ego-boundaries or legacy-projection)
endif
ifeq ($(filter $(LANE_CURVE_MODE),ego-boundaries legacy-projection),)
$(error LANE_CURVE_MODE must be ego-boundaries or legacy-projection)
endif

# 说明：源代码目录
SRCDIR = csrc
INCDIR = include
BUILDDIR = build

# 说明：子模块目录
PCIE_DIR = pcie
UDP_DIR = udp

# 说明：渲染功能控制
ENABLE_RENDERING ?= 1

# 说明：共享内存模块
SHARED_MEMORY_SRC = $(SRCDIR)/shared_memory.c
SHARED_MEMORY_OBJ = $(BUILDDIR)/shared_memory.o

# 选择 C 和 C++ 工具链
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c99 -O3 -march=native -pthread -fPIC -MMD -MP
CXXFLAGS = -Wall -Wextra -std=c++11 -O3 -march=native -pthread -fPIC -MMD -MP
INCLUDES = -I$(INCDIR)

#############################################################################
# 直接调用 make 时采用的入口
#############################################################################
.PHONY: all clean install help info pcie udp shared_memory debug release test status run run-background run-pcie run-udp run-both stop debug-shared-memory check-system distclean

all: info shared_memory pcie udp

#############################################################################
# 说明：项目信息
#############################################################################
info:
	@echo "======================================"
	@echo "$(PROJECT_NAME) Build Console"
	@echo "======================================"
	@echo "Maintainer: 科研轮转"
	@echo "本版更新：2026年10月30日"
	@echo "======================================"
	@echo "Components:"
	@echo "  - Shared Memory Module (csrc/)"
	@echo "  - PCIe Capture Module ($(PCIE_DIR)/)"
	@echo "  - UDP Network Module ($(UDP_DIR)/)"
	@echo "Renderer option: $(ENABLE_RENDERING)"
	@echo "YOLO detector: on"
	@echo "======================================"

#############################################################################
# 说明：子模块构建
#############################################################################
shared_memory: | $(BUILDDIR)
	@echo "Preparing Shared Memory Module..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SHARED_MEMORY_SRC) -o $(SHARED_MEMORY_OBJ)
	@echo "Shared Memory module build finished"

pcie: shared_memory
	@echo "Preparing PCIe Capture Module..."
	@$(MAKE) -C $(PCIE_DIR) ENABLE_RENDERING=$(ENABLE_RENDERING) TOPDIR=$(TOPDIR)
	@echo "PCIe module build finished"

udp: shared_memory
	@echo "Preparing UDP Network Module..."
	@$(MAKE) -C $(UDP_DIR) TOPDIR=$(TOPDIR)
	@echo "UDP module build finished"

# 建立build目录
$(BUILDDIR):
	@echo "Preparing build directory"
	mkdir -p $(BUILDDIR)

#############################################################################
# 说明：构建模式
#############################################################################
debug:
	@echo "Preparing debug versions..."
	@$(MAKE) -C $(PCIE_DIR) debug ENABLE_RENDERING=$(ENABLE_RENDERING) TOPDIR=$(TOPDIR)
	@$(MAKE) -C $(UDP_DIR) debug TOPDIR=$(TOPDIR)

release:
	@echo "Preparing release versions..."
	@$(MAKE) -C $(PCIE_DIR) release ENABLE_RENDERING=$(ENABLE_RENDERING) TOPDIR=$(TOPDIR)
	@$(MAKE) -C $(UDP_DIR) release TOPDIR=$(TOPDIR)

#############################################################################
# 将产物放入安装路径
#############################################################################
install: all
	@echo "Setting up M-End system..."
	@mkdir -p build/bin
	@$(MAKE) -C $(PCIE_DIR) install
	@$(MAKE) -C $(UDP_DIR) install
	@echo "Copying executables to main build directory..."
	@cp $(PCIE_DIR)/build/pcie_capture build/bin/ 2>/dev/null || true
	@cp $(UDP_DIR)/build/udp_sender build/bin/ 2>/dev/null || true
	@cp -r $(PCIE_DIR)/model build/ 2>/dev/null || true
	@echo "Installation finished"
	@echo "Executables available in: build/bin/"

#############################################################################
# 可单独调用的验证入口
#############################################################################
test-pcie: pcie
	@echo "Checking PCIe module..."
	@echo "Start with: ./$(PCIE_DIR)/build/pcie_capture"

test-udp: udp
	@echo "Checking UDP module..."
	@$(MAKE) -C $(UDP_DIR) test

test: test-pcie test-udp

#############################################################################
# 说明：运行
#############################################################################
run-pcie: pcie
	@echo "Launching PCIe capture module..."
	cd $(PCIE_DIR) && env LANE_CURVE_MODE=$(LANE_CURVE_MODE) ./build/pcie_capture

run-udp: udp tune-network
	@echo "Launching UDP sender module..."
	cd $(UDP_DIR) && ./build/udp_sender

# 运行完整M端系统
run: all tune-network
	@echo "=== 运行M端FPGA系统 ==="
	@echo "说明：系统: $$(uname -s) $$(uname -m)"
	@echo "说明：时间: $$(date)"
	@echo ""
	@# 说明：检查程序是否存在
	@if [ ! -f "$(PCIE_DIR)/build/pcie_capture" ]; then \
		echo "异常: PCIe程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(UDP_DIR)/build/udp_sender" ]; then \
		echo "异常: UDP程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if pgrep -x pcie_capture >/dev/null || pgrep -x udp_sender >/dev/null; then \
		echo "M端已有采集/发出进程，请先 make stop，避免重复占用DMA和图像共享段"; exit 1; \
	fi
	@# 移除之前的共享内存
	@echo "移除共享内存..."
	@-ipcs -m | grep 0x12345679 | awk '{print $$2}' | xargs -r ipcrm -m 2>/dev/null || true
	@-ipcs -s | grep $$(whoami) | awk '{print $$2}' | xargs -r ipcrm -s 2>/dev/null || true
	@echo "共享内存移除完成"
	@echo ""
	@# 运行PCIe模块 (后台运行，输出到终端)
	@echo "运行PCIe图像采集模块..."
	@echo "说明：运行PCIe程序需要sudo权限..."
	@cd $(PCIE_DIR) && sudo env LANE_CURVE_MODE=$(LANE_CURVE_MODE) ./build/pcie_capture &
	@echo "PCIe模块已运行，等待共享内存建立..."
	@# 等待共享内存建立（最多等待30秒）
	@WAIT_COUNT=0; \
	while [ $$WAIT_COUNT -lt 30 ]; do \
		if ipcs -m 2>/dev/null | grep -q 0x12345679; then \
			echo "✓ 共享内存已建立"; \
			break; \
		fi; \
		sleep 1; \
		WAIT_COUNT=$$((WAIT_COUNT + 1)); \
		if [ $$((WAIT_COUNT % 5)) -eq 0 ]; then \
			echo "说明：等待中... ($$WAIT_COUNT秒)"; \
		fi; \
	done
	@# 检查共享内存是否已完成建立
	@if ! ipcs -m 2>/dev/null | grep -q 0x12345679; then \
		echo "✗ 异常: PCIe程序未能建立共享内存"; \
		echo "请核对PCIe程序的终端输出"; \
		exit 1; \
	fi
	@echo ""
	@# 运行UDP模块 (前台运行)
	@echo "运行UDP网络发出模块..."
	@echo "UDP发出程序将在前台运行，按Ctrl+C退出"
	@cd $(UDP_DIR) && ./build/udp_sender

# 运行后台程序（PCIe + UDP后台）
run-background: all tune-network
	@echo "=== 运行M端后台程序 ==="
	@echo "说明：系统: $$(uname -s) $$(uname -m)"
	@echo "说明：时间: $$(date)"
	@echo ""
	@# 说明：检查程序是否存在
	@if [ ! -f "$(PCIE_DIR)/build/pcie_capture" ]; then \
		echo "异常: PCIe程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(UDP_DIR)/build/udp_sender" ]; then \
		echo "异常: UDP程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if pgrep -x pcie_capture >/dev/null || pgrep -x udp_sender >/dev/null; then \
		echo "M端已有采集/发出进程，请先 make stop，避免重复占用DMA和图像共享段"; exit 1; \
	fi
	@# 移除之前的共享内存
	@echo "移除共享内存..."
	@-ipcs -m | grep 0x12345679 | awk '{print $$2}' | xargs -r ipcrm -m 2>/dev/null || true
	@-ipcs -s | grep $$(whoami) | awk '{print $$2}' | xargs -r ipcrm -s 2>/dev/null || true
	@echo "共享内存移除完成"
	@echo ""
	@# 运行PCIe后台程序
	@echo "运行PCIe图像采集后台程序..."
	@echo "说明：运行PCIe程序需要sudo权限..."
	@cd $(PCIE_DIR) && sudo env LANE_CURVE_MODE=$(LANE_CURVE_MODE) nohup ./build/pcie_capture > pcie.log 2>&1 &
	@echo "PCIe程序已运行，等待共享内存建立..."
	@# 等待共享内存建立（最多等待30秒）
	@WAIT_COUNT=0; \
	while [ $$WAIT_COUNT -lt 30 ]; do \
		if ipcs -m 2>/dev/null | grep -q 0x12345679; then \
			echo "✓ 共享内存已建立"; \
			break; \
		fi; \
		sleep 1; \
		WAIT_COUNT=$$((WAIT_COUNT + 1)); \
		if [ $$((WAIT_COUNT % 5)) -eq 0 ]; then \
			echo "说明：等待中... ($$WAIT_COUNT秒)"; \
		fi; \
	done
	@# 检查共享内存是否已完成建立
	@if ! ipcs -m 2>/dev/null | grep -q 0x12345679; then \
		echo "✗ 异常: PCIe程序未能建立共享内存"; \
		echo "请核对 $(PCIE_DIR)/pcie.log 查看错误信息"; \
		tail -n 20 $(PCIE_DIR)/pcie.log 2>/dev/null || true; \
		exit 1; \
	fi
	@echo ""
	@# 运行UDP后台程序
	@echo "运行UDP网络发出后台程序..."
	@cd $(UDP_DIR) && nohup ./build/udp_sender > udp.log 2>&1 &
	@echo "UDP程序已运行，等待初始化..."
	@sleep 2
	@echo ""
	@echo "M端后台程序运行完成！"
	@echo "说明：日志文件:"
	@echo "  PCIe: $(PCIE_DIR)/pcie.log"
	@echo "  UDP:  $(UDP_DIR)/udp.log"

# 说明：兼容旧接口
run-both: run-background

# 说明：调试相关
debug-shared-memory:
	@echo "=== 说明：共享内存调试 ==="
	@echo "说明：使用ipcs -m查看共享内存状态"
	@echo "使用ipcrm -m <shmid>移除共享内存"

check-system:
	@echo "=== 说明：系统状态检查 ==="
	@echo "说明：PCIe模块: $(shell test -f pcie/build/pcie_capture && echo '已构建' || echo '未构建')"
	@echo "说明：UDP模块: $(shell test -f udp/build/udp_sender && echo '已构建' || echo '未构建')"
	@echo "说明：共享内存模块: $(shell test -f build/shared_memory.o && echo '已构建' || echo '未构建')"

# 结束M端系统
stop:
	@echo "=== 结束M端FPGA系统 ==="
	@# 结束PCIe程序
	@PCIE_PIDS=$$(pgrep -f "pcie_capture" 2>/dev/null || true); \
	if [ ! -z "$$PCIE_PIDS" ]; then \
		echo "结束PCIe程序 (PIDs: $$PCIE_PIDS)"; \
		echo $$PCIE_PIDS | xargs kill -TERM 2>/dev/null || true; \
		sleep 2; \
		PCIE_PIDS=$$(pgrep -f "pcie_capture" 2>/dev/null || true); \
		if [ ! -z "$$PCIE_PIDS" ]; then \
			echo "说明：强制终止PCIe程序"; \
			echo $$PCIE_PIDS | xargs kill -KILL 2>/dev/null || true; \
		fi; \
	else \
		echo "未定位到运行中的PCIe程序"; \
	fi
	@# 结束UDP程序
	@UDP_PIDS=$$(pgrep -f "udp_sender" 2>/dev/null || true); \
	if [ ! -z "$$UDP_PIDS" ]; then \
		echo "结束UDP程序 (PIDs: $$UDP_PIDS)"; \
		echo $$UDP_PIDS | xargs kill -TERM 2>/dev/null || true; \
		sleep 2; \
		UDP_PIDS=$$(pgrep -f "udp_sender" 2>/dev/null || true); \
		if [ ! -z "$$UDP_PIDS" ]; then \
			echo "说明：强制终止UDP程序"; \
			echo $$UDP_PIDS | xargs kill -KILL 2>/dev/null || true; \
		fi; \
	else \
		echo "未定位到运行中的UDP程序"; \
	fi
	@# 移除共享内存和信号量
	@echo "移除共享内存..."
	@-ipcs -m | grep 0x12345679 | awk '{print $$2}' | xargs -r ipcrm -m 2>/dev/null || true
	@echo "移除信号量..."
	@-ipcs -s | grep $$(whoami) | awk '{print $$2}' | xargs -r ipcrm -s 2>/dev/null || true
	@echo "M端系统结束完成"

#############################################################################
# 回收编译产物
#############################################################################
clean:
	@echo "Removing all modules..."
	@$(MAKE) -C $(PCIE_DIR) clean
	@$(MAKE) -C $(UDP_DIR) clean
	@rm -rf build
	@echo "Clean complete"

# 深度移除（包括结束进程和移除共享内存）
distclean: clean stop
	@echo "=== 深度移除 ==="
	@rm -f $(PCIE_DIR)/pcie.log $(UDP_DIR)/udp.log
	@echo "深度移除完成"

#############################################################################
# 用法
#############################################################################
help:
	@echo "M-End FPGA System Build Console"
	@echo "======================================"
	@echo "Available targets:"
	@echo "  all           - Build all modules (default)"
	@echo "  shared_memory - Build shared memory module only"
	@echo "  pcie          - Build PCIe capture module only"
	@echo "  udp           - Build UDP network module only"
	@echo "  debug         - Build debug versions of all modules"
	@echo "  release       - Build optimized release versions"
	@echo "  install       - Build and install all modules"
	@echo "  run           - Run complete M-end system (PCIe + UDP)"
	@echo "  run-background- Run M-end system in background mode"
	@echo "  run-pcie      - Run PCIe capture module only"
	@echo "  run-udp       - Run UDP sender module only"
	@echo "  run-both      - Run both modules in background (alias for run-background)"
	@echo "  test          - Test all modules"
	@echo "  test-pcie     - Test PCIe module only"
	@echo "  test-udp      - Test UDP module only"
	@echo "  clean         - Clean all build directories"
	@echo "  status        - Show build status"
	@echo "  check-system  - Check system build status"
	@echo "  stop          - Stop all running processes"
	@echo "  debug-shared-memory - Show shared memory debug info"
	@echo "  help          - Show this help"
	@echo ""
	@echo "Options:"
	@echo "  ENABLE_RENDERING=1   - Enable rendering in PCIe module (default)"
	@echo "  ENABLE_RENDERING=0   - Disable rendering in PCIe module"
	@echo ""
	@echo "Examples:"
	@echo "  make                           - Build all with rendering and YOLO"
	@echo "  make ENABLE_RENDERING=0        - Build all without rendering"
	@echo "  make run                       - Run complete M-end system"
	@echo "  make run-background            - Run M-end system in background"
	@echo "  make status                    - Check system status"
	@echo "  make stop                      - Stop all running processes"
	@echo "  make shared_memory             - Build shared memory module only"
	@echo "  make pcie                      - Build PCIe module only"
	@echo "  make udp                       - Build UDP module only"
	@echo "  make debug                     - Build debug versions"
	@echo "  make release                   - Build release versions"
	@echo "  make install                   - Build and install everything"
	@echo ""
	@echo "说明：典型工作流程:"
	@echo "  1. make run-background         # 运行M端后台系统"
	@echo "  说明：2. make status                 # 检查运行状态"
	@echo "  3. make stop                   # 结束所有进程"
	@echo ""
	@echo "说明：或者一键运行:"
	@echo "  make run                       # 启动完整M端系统"
	@echo ""
	@echo "Module Structure:"
	@echo "  csrc/                    - Shared memory C source"
	@echo "  include/                 - Shared memory headers"
	@echo "  $(PCIE_DIR)/                 - PCIe capture and optional rendering"
	@echo "    ├── csrc/            - C/C++ source files"
	@echo "    ├── include/         - Header files"
	@echo "    ├── lib/             - Libraries"
	@echo "    ├── model/           - YOLO models"
	@echo "    └── build/           - Build output"
	@echo ""
	@echo "  $(UDP_DIR)/                  - UDP network transmission"
	@echo "    ├── csrc/            - C/C++ source files"
	@echo "    ├── include/         - Header files"
	@echo "    └── build/           - Build output"
	@echo ""
	@echo "Data Flow:"
	@echo "  说明：PCIe → Shared Memory → UDP → Network → S端"
	@echo "  PCIe → X11 Display (if ENABLE_RENDERING=1)"

#############################################################################
# 说明：状态检查
#############################################################################
status:
	@echo "=== 说明：M端系统状态 ==="
	@echo "构建状态:"
	@if [ -f "$(PCIE_DIR)/build/pcie_capture" ]; then \
		echo "说明：✓ PCIe程序: $(PCIE_DIR)/build/pcie_capture"; \
		ls -lh $(PCIE_DIR)/build/pcie_capture; \
	else \
		echo "✗ PCIe程序: 未构建"; \
	fi
	@if [ -f "$(UDP_DIR)/build/udp_sender" ]; then \
		echo "说明：✓ UDP程序: $(UDP_DIR)/build/udp_sender"; \
		ls -lh $(UDP_DIR)/build/udp_sender; \
	else \
		echo "✗ UDP程序: 未构建"; \
	fi
	@echo ""
	@echo "说明：运行状态:"
	@PCIE_PIDS=$$(pgrep -f "pcie_capture" 2>/dev/null || true); \
	if [ ! -z "$$PCIE_PIDS" ]; then \
		echo "说明：✓ PCIe程序运行中 (PIDs: $$PCIE_PIDS)"; \
	else \
		echo "说明：✗ PCIe程序未运行"; \
	fi
	@UDP_PIDS=$$(pgrep -f "udp_sender" 2>/dev/null || true); \
	if [ ! -z "$$UDP_PIDS" ]; then \
		echo "说明：✓ UDP程序运行中 (PIDs: $$UDP_PIDS)"; \
	else \
		echo "说明：✗ UDP程序未运行"; \
	fi
	@echo ""
	@echo "说明：共享内存状态:"
	@SHARED_MEM=$$(ipcs -m | grep 0x12345679 2>/dev/null || true); \
	if [ ! -z "$$SHARED_MEM" ]; then \
		echo "说明：✓ 共享内存存在"; \
		echo "$$SHARED_MEM"; \
	else \
		echo "说明：✗ 共享内存不存在"; \
	fi
	@echo ""
	@echo "说明：安装状态:"
	@if [ -d build/bin ]; then \
		echo "说明：✓ 已安装在: build/bin/"; \
		echo "说明：文件: $$(ls build/bin/ 2>/dev/null || echo 'none')"; \
	else \
		echo "说明：✗ 未安装"; \
	fi

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
-include $(SHARED_MEMORY_OBJ:.o=.d)
