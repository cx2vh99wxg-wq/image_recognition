# 顶层 Makefile — 一键构建（【人员 B】集成负责人维护）
#
#   make                本机：全模块编译自检 + 全部单元测试
#   make USE_STUB=1     桩模式（person_detect 走桩）
#   make board          板端交叉编译（common + perception + planning）
#   make clean
#
# 依赖模块：common(公共库) → perception(人员A) → planning(人员B)。
# control(人员C) 加入后在此追加。

.PHONY: all board clean tests common perception planning \
        run-m run-s run stop status check deps deploy

all: common perception planning

common:
	$(MAKE) -C common all

perception:
	$(MAKE) -C perception all

planning:
	$(MAKE) -C planning all

tests: common perception planning
	@echo "==== 全部单元测试通过 ===="

# 板端交叉编译 + 部署布局 staging 到 bin/
# （bin/ 下生成 perception_main / planning_main / udp_m_send_main，
#   连同 drivers/、lib/、model/、scripts/ 一起拷到板卡即可）
board:
	$(MAKE) -C common board
	$(MAKE) -C perception CROSS=aarch64-linux-gnu- bin
	$(MAKE) -C planning CROSS=aarch64-linux-gnu- bin
	@mkdir -p bin
	@cp -f perception/perception_main bin/ 2>/dev/null || true
	@cp -f planning/planning_main planning/udp_m_send_main bin/ 2>/dev/null || true
	@echo "==== 板端构建完成，部署布局在 bin/ ===="
	@echo "板卡部署：拷贝 bin/ drivers/ lib/ model/ scripts/ 到板卡后运行"
	@echo "  sudo ./scripts/board/deploy_board.sh [m|s]   # 一次性装自启+驱动+网络"
	@echo "  sudo ./scripts/start_m.sh / start_s.sh       # 启动"

# ---- 板端运行/运维命令（在板卡上执行，等价参考工程 make run/stop/status）----
run-m: 
	./scripts/start_m.sh

run-s:
	./scripts/start_s.sh

run:
	@echo "双板分别执行：M 板 make run-m；S 板 make run-s"

stop:
	./scripts/stop_all.sh

status:
	./scripts/check_system.sh

# 环境/依赖检查与安装
check:
	@echo "编译链: aarch64-linux-gnu-gcc（未安装则 apt install gcc-aarch64-linux-gnu）"
	@echo "板端依赖: drivers/pango_pci_driver.ko + lib/librknnrt.so + libX11 + librknnrt"
	@echo "板端动态诊断: ./scripts/check_system.sh"

deps:
	@echo "=== 板端/宿主机依赖安装 ==="
	@echo "宿主机（交叉编译）: sudo apt-get install -y gcc-aarch64-linux-gnu"
	@echo "板端运行库: lib/librknnrt.so 已随仓库分发（运行时 export LD_LIBRARY_PATH）"
	@echo "板端显示库: sudo apt-get install -y libx11-6（libx11-dev 用于编译）"

# 板卡部署别名（等价 deploy_board.sh）
deploy:
	@echo "在板卡上执行: sudo ./scripts/board/deploy_board.sh [m|s]"

clean:
	$(MAKE) -C common clean
	$(MAKE) -C perception clean
	$(MAKE) -C planning clean
	@rm -rf bin
