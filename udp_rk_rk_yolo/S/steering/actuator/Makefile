# 从执行器目录调用转向构建驱动。
PYTHON ?= python3
.DEFAULT_GOAL := all
.PHONY: all hardware
all:
	$(PYTHON) ../tools/build.py
hardware:
	$(PYTHON) ../tools/build.py --hardware
