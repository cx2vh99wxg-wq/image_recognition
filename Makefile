# 顶层 Makefile — 一键构建（【人员 B】集成负责人维护）
#
#   make                本机：全模块编译自检 + 全部单元测试
#   make USE_STUB=1     桩模式（person_detect 走桩）
#   make board          板端交叉编译（common + perception + planning）
#   make clean
#
# 依赖模块：common(公共库) → perception(人员A) → planning(人员B)。
# control(人员C) 加入后在此追加。

.PHONY: all board clean tests common perception planning

all: common perception planning

common:
	$(MAKE) -C common all

perception:
	$(MAKE) -C perception all

planning:
	$(MAKE) -C planning all

tests: common perception planning
	@echo "==== 全部单元测试通过 ===="

board:
	$(MAKE) -C common board
	$(MAKE) -C perception CROSS=aarch64-linux-gnu- bin
	$(MAKE) -C planning CROSS=aarch64-linux-gnu- bin
	@echo "==== 板端构建完成（perception_main / planning_main / udp_m_send_main）===="

clean:
	$(MAKE) -C common clean
	$(MAKE) -C perception clean
	$(MAKE) -C planning clean
