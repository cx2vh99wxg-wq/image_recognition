# 构建 IMU 工具；测试入口由同一 Python 驱动执行。
PYTHON ?= python3
.DEFAULT_GOAL := all
.PHONY: all test
all:
	$(PYTHON) tools/build.py
test:
	$(PYTHON) tools/build.py --test
