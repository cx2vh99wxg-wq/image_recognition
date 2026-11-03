# 默认生成转向工具，实板和测试构建各有独立入口。
PYTHON ?= python3
.DEFAULT_GOAL := all
.PHONY: all actuator hardware test
all: actuator
actuator:
	$(PYTHON) tools/build.py
hardware:
	$(PYTHON) tools/build.py --hardware
test:
	$(PYTHON) tools/build.py --test
