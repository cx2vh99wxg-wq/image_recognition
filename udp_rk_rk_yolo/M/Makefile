# Makefile for M-End Project
# Author: 辉哥大盗
# Date: 2025年10月17日
# Description: 管理PCIe模块和UDP模块的构建

#############################################################################
# 基本配置
#############################################################################
PROJECT_NAME = M-End_FPGA_System
TOPDIR = $(shell pwd)

# 源代码目录
SRCDIR = csrc
INCDIR = include
BUILDDIR = build

# 子模块目录
PCIE_DIR = pcie
UDP_DIR = udp

# 渲染功能控制
ENABLE_RENDERING ?= 1

# 共享内存模块
SHARED_MEMORY_SRC = $(SRCDIR)/shared_memory.c
SHARED_MEMORY_OBJ = $(BUILDDIR)/shared_memory.o

# 编译器设置
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c99 -fPIC
CXXFLAGS = -Wall -Wextra -std=c++11 -fPIC
INCLUDES = -I$(INCDIR)

#############################################################################
# 默认目标
#############################################################################
.PHONY: all clean install help info pcie udp shared_memory debug release test status run run-background run-pcie run-udp run-both stop debug-shared-memory check-system distclean

all: info shared_memory pcie udp

#############################################################################
# 项目信息
#############################################################################
info:
	@echo "======================================"
	@echo "$(PROJECT_NAME) Build System"
	@echo "======================================"
	@echo "Author: 辉哥大盗"
	@echo "Date: 2025年10月17日"
	@echo "======================================"
	@echo "Modules:"
	@echo "  - Shared Memory Module (csrc/)"
	@echo "  - PCIe Capture Module ($(PCIE_DIR)/)"
	@echo "  - UDP Network Module ($(UDP_DIR)/)"
	@echo "Rendering Support: $(ENABLE_RENDERING)"
	@echo "YOLO Detection: Enabled"
	@echo "======================================"

#############################################################################
# 子模块构建
#############################################################################
shared_memory: | $(BUILDDIR)
	@echo "Building Shared Memory Module..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SHARED_MEMORY_SRC) -o $(SHARED_MEMORY_OBJ)
	@echo "Shared Memory module build complete"

pcie: shared_memory
	@echo "Building PCIe Capture Module..."
	@$(MAKE) -C $(PCIE_DIR) ENABLE_RENDERING=$(ENABLE_RENDERING) TOPDIR="$(TOPDIR)"
	@echo "PCIe module build complete"

udp: shared_memory
	@echo "Building UDP Network Module..."
	@$(MAKE) -C $(UDP_DIR) TOPDIR="$(TOPDIR)"
	@echo "UDP module build complete"

# 创建build目录
$(BUILDDIR):
	@echo "Creating build directory"
	mkdir -p $(BUILDDIR)

#############################################################################
# 构建模式
#############################################################################
debug:
	@echo "Building debug versions..."
	@$(MAKE) -C $(PCIE_DIR) debug ENABLE_RENDERING=$(ENABLE_RENDERING) TOPDIR="$(TOPDIR)"
	@$(MAKE) -C $(UDP_DIR) debug TOPDIR="$(TOPDIR)"

release:
	@echo "Building release versions..."
	@$(MAKE) -C $(PCIE_DIR) release ENABLE_RENDERING=$(ENABLE_RENDERING) TOPDIR="$(TOPDIR)"
	@$(MAKE) -C $(UDP_DIR) release TOPDIR="$(TOPDIR)"

#############################################################################
# 安装
#############################################################################
install: all
	@echo "Installing M-End system..."
	@mkdir -p build/bin
	@$(MAKE) -C $(PCIE_DIR) install
	@$(MAKE) -C $(UDP_DIR) install
	@echo "Copying executables to main build directory..."
	@cp $(PCIE_DIR)/build/pcie_capture build/bin/ 2>/dev/null || true
	@cp $(UDP_DIR)/build/udp_sender build/bin/ 2>/dev/null || true
	@cp -r $(PCIE_DIR)/model build/ 2>/dev/null || true
	@echo "Installation complete"
	@echo "Executables available in: build/bin/"

#############################################################################
# 测试
#############################################################################
test-pcie: pcie
	@echo "Testing PCIe module..."
	@echo "Run: ./$(PCIE_DIR)/build/pcie_capture"

test-udp: udp
	@echo "Testing UDP module..."
	@$(MAKE) -C $(UDP_DIR) test

test: test-pcie test-udp

#############################################################################
# 运行
#############################################################################
run-pcie: pcie
	@echo "Running PCIe capture module..."
	cd $(PCIE_DIR) && ./build/pcie_capture

run-udp: udp
	@echo "Running UDP sender module..."
	cd $(UDP_DIR) && ./build/udp_sender

# 启动完整M端系统
run: all
	@echo "=== 启动M端FPGA系统 ==="
	@echo "系统: $$(uname -s) $$(uname -m)"
	@echo "时间: $$(date)"
	@echo ""
	@# 检查程序是否存在
	@if [ ! -f "$(PCIE_DIR)/build/pcie_capture" ]; then \
		echo "错误: PCIe程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(UDP_DIR)/build/udp_sender" ]; then \
		echo "错误: UDP程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@# 清理之前的共享内存
	@echo "清理共享内存..."
	@-ipcs -m | grep 0x12345679 | awk '{print $$2}' | xargs -r ipcrm -m 2>/dev/null || true
	@-ipcs -s | grep $$(whoami) | awk '{print $$2}' | xargs -r ipcrm -s 2>/dev/null || true
	@echo "共享内存清理完成"
	@echo ""
	@# 启动PCIe模块 (后台运行，输出到终端)
	@echo "启动PCIe图像采集模块..."
	@echo "运行PCIe程序需要sudo权限..."
	@cd $(PCIE_DIR) && sudo ./build/pcie_capture &
	@echo "PCIe模块已启动，等待共享内存创建..."
	@# 等待共享内存创建（最多等待30秒）
	@WAIT_COUNT=0; \
	while [ $$WAIT_COUNT -lt 30 ]; do \
		if ipcs -m 2>/dev/null | grep -q 0x12345679; then \
			echo "✓ 共享内存已创建"; \
			break; \
		fi; \
		sleep 1; \
		WAIT_COUNT=$$((WAIT_COUNT + 1)); \
		if [ $$((WAIT_COUNT % 5)) -eq 0 ]; then \
			echo "等待中... ($$WAIT_COUNT秒)"; \
		fi; \
	done
	@# 检查共享内存是否成功创建
	@if ! ipcs -m 2>/dev/null | grep -q 0x12345679; then \
		echo "✗ 错误: PCIe程序未能创建共享内存"; \
		echo "请检查PCIe程序的终端输出"; \
		exit 1; \
	fi
	@echo ""
	@# 启动UDP模块 (前台运行)
	@echo "启动UDP网络发送模块..."
	@echo "UDP发送程序将在前台运行，按Ctrl+C退出"
	@cd $(UDP_DIR) && ./build/udp_sender

# 启动后台程序（PCIe + UDP后台）
run-background: all
	@echo "=== 启动M端后台程序 ==="
	@echo "系统: $$(uname -s) $$(uname -m)"
	@echo "时间: $$(date)"
	@echo ""
	@# 检查程序是否存在
	@if [ ! -f "$(PCIE_DIR)/build/pcie_capture" ]; then \
		echo "错误: PCIe程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@if [ ! -f "$(UDP_DIR)/build/udp_sender" ]; then \
		echo "错误: UDP程序不存在，请先运行 make"; \
		exit 1; \
	fi
	@# 清理之前的共享内存
	@echo "清理共享内存..."
	@-ipcs -m | grep 0x12345679 | awk '{print $$2}' | xargs -r ipcrm -m 2>/dev/null || true
	@-ipcs -s | grep $$(whoami) | awk '{print $$2}' | xargs -r ipcrm -s 2>/dev/null || true
	@echo "共享内存清理完成"
	@echo ""
	@# 启动PCIe后台程序
	@echo "启动PCIe图像采集后台程序..."
	@echo "运行PCIe程序需要sudo权限..."
	@cd $(PCIE_DIR) && sudo nohup ./build/pcie_capture > pcie.log 2>&1 &
	@echo "PCIe程序已启动，等待共享内存创建..."
	@# 等待共享内存创建（最多等待30秒）
	@WAIT_COUNT=0; \
	while [ $$WAIT_COUNT -lt 30 ]; do \
		if ipcs -m 2>/dev/null | grep -q 0x12345679; then \
			echo "✓ 共享内存已创建"; \
			break; \
		fi; \
		sleep 1; \
		WAIT_COUNT=$$((WAIT_COUNT + 1)); \
		if [ $$((WAIT_COUNT % 5)) -eq 0 ]; then \
			echo "等待中... ($$WAIT_COUNT秒)"; \
		fi; \
	done
	@# 检查共享内存是否成功创建
	@if ! ipcs -m 2>/dev/null | grep -q 0x12345679; then \
		echo "✗ 错误: PCIe程序未能创建共享内存"; \
		echo "请检查 $(PCIE_DIR)/pcie.log 查看错误信息"; \
		tail -n 20 $(PCIE_DIR)/pcie.log 2>/dev/null || true; \
		exit 1; \
	fi
	@echo ""
	@# 启动UDP后台程序
	@echo "启动UDP网络发送后台程序..."
	@cd $(UDP_DIR) && nohup ./build/udp_sender > udp.log 2>&1 &
	@echo "UDP程序已启动，等待初始化..."
	@sleep 2
	@echo ""
	@echo "M端后台程序启动完成！"
	@echo "日志文件:"
	@echo "  PCIe: $(PCIE_DIR)/pcie.log"
	@echo "  UDP:  $(UDP_DIR)/udp.log"

# 兼容旧接口
run-both: run-background

# 调试相关
debug-shared-memory:
	@echo "=== 共享内存调试 ==="
	@echo "使用ipcs -m查看共享内存状态"
	@echo "使用ipcrm -m <shmid>清理共享内存"

check-system:
	@echo "=== 系统状态检查 ==="
	@echo "PCIe模块: $(shell test -f pcie/build/pcie_capture && echo '已构建' || echo '未构建')"
	@echo "UDP模块: $(shell test -f udp/build/udp_sender && echo '已构建' || echo '未构建')"
	@echo "共享内存模块: $(shell test -f build/shared_memory.o && echo '已构建' || echo '未构建')"

# 停止M端系统
stop:
	@echo "=== 停止M端FPGA系统 ==="
	@# 停止PCIe程序
	@PCIE_PIDS=$$(pgrep -f "pcie_capture" 2>/dev/null || true); \
	if [ ! -z "$$PCIE_PIDS" ]; then \
		echo "停止PCIe程序 (PIDs: $$PCIE_PIDS)"; \
		echo $$PCIE_PIDS | xargs kill -TERM 2>/dev/null || true; \
		sleep 2; \
		PCIE_PIDS=$$(pgrep -f "pcie_capture" 2>/dev/null || true); \
		if [ ! -z "$$PCIE_PIDS" ]; then \
			echo "强制终止PCIe程序"; \
			echo $$PCIE_PIDS | xargs kill -KILL 2>/dev/null || true; \
		fi; \
	else \
		echo "未找到运行中的PCIe程序"; \
	fi
	@# 停止UDP程序
	@UDP_PIDS=$$(pgrep -f "udp_sender" 2>/dev/null || true); \
	if [ ! -z "$$UDP_PIDS" ]; then \
		echo "停止UDP程序 (PIDs: $$UDP_PIDS)"; \
		echo $$UDP_PIDS | xargs kill -TERM 2>/dev/null || true; \
		sleep 2; \
		UDP_PIDS=$$(pgrep -f "udp_sender" 2>/dev/null || true); \
		if [ ! -z "$$UDP_PIDS" ]; then \
			echo "强制终止UDP程序"; \
			echo $$UDP_PIDS | xargs kill -KILL 2>/dev/null || true; \
		fi; \
	else \
		echo "未找到运行中的UDP程序"; \
	fi
	@# 清理共享内存和信号量
	@echo "清理共享内存..."
	@-ipcs -m | grep 0x12345679 | awk '{print $$2}' | xargs -r ipcrm -m 2>/dev/null || true
	@echo "清理信号量..."
	@-ipcs -s | grep $$(whoami) | awk '{print $$2}' | xargs -r ipcrm -s 2>/dev/null || true
	@echo "M端系统停止完成"

#############################################################################
# 清理
#############################################################################
clean:
	@echo "Cleaning all modules..."
	@$(MAKE) -C $(PCIE_DIR) clean
	@$(MAKE) -C $(UDP_DIR) clean
	@rm -rf build
	@echo "Clean complete"

# 深度清理（包括停止进程和清理共享内存）
distclean: clean stop
	@echo "=== 深度清理 ==="
	@rm -f $(PCIE_DIR)/pcie.log $(UDP_DIR)/udp.log
	@echo "深度清理完成"

#############################################################################
# 帮助
#############################################################################
help:
	@echo "M-End FPGA System Build System"
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
	@echo "典型工作流程:"
	@echo "  1. make run-background         # 启动M端后台系统"
	@echo "  2. make status                 # 检查运行状态"
	@echo "  3. make stop                   # 停止所有进程"
	@echo ""
	@echo "或者一键运行:"
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
	@echo "  PCIe → Shared Memory → UDP → Network → S端"
	@echo "  PCIe → X11 Display (if ENABLE_RENDERING=1)"

#############################################################################
# 状态检查
#############################################################################
status:
	@echo "=== M端系统状态 ==="
	@echo "编译状态:"
	@if [ -f "$(PCIE_DIR)/build/pcie_capture" ]; then \
		echo "✓ PCIe程序: $(PCIE_DIR)/build/pcie_capture"; \
		ls -lh $(PCIE_DIR)/build/pcie_capture; \
	else \
		echo "✗ PCIe程序: 未编译"; \
	fi
	@if [ -f "$(UDP_DIR)/build/udp_sender" ]; then \
		echo "✓ UDP程序: $(UDP_DIR)/build/udp_sender"; \
		ls -lh $(UDP_DIR)/build/udp_sender; \
	else \
		echo "✗ UDP程序: 未编译"; \
	fi
	@echo ""
	@echo "运行状态:"
	@PCIE_PIDS=$$(pgrep -f "pcie_capture" 2>/dev/null || true); \
	if [ ! -z "$$PCIE_PIDS" ]; then \
		echo "✓ PCIe程序运行中 (PIDs: $$PCIE_PIDS)"; \
	else \
		echo "✗ PCIe程序未运行"; \
	fi
	@UDP_PIDS=$$(pgrep -f "udp_sender" 2>/dev/null || true); \
	if [ ! -z "$$UDP_PIDS" ]; then \
		echo "✓ UDP程序运行中 (PIDs: $$UDP_PIDS)"; \
	else \
		echo "✗ UDP程序未运行"; \
	fi
	@echo ""
	@echo "共享内存状态:"
	@SHARED_MEM=$$(ipcs -m | grep 0x12345679 2>/dev/null || true); \
	if [ ! -z "$$SHARED_MEM" ]; then \
		echo "✓ 共享内存存在"; \
		echo "$$SHARED_MEM"; \
	else \
		echo "✗ 共享内存不存在"; \
	fi
	@echo ""
	@echo "安装状态:"
	@if [ -d build/bin ]; then \
		echo "✓ 已安装在: build/bin/"; \
		echo "文件: $$(ls build/bin/ 2>/dev/null || echo 'none')"; \
	else \
		echo "✗ 未安装"; \
	fi