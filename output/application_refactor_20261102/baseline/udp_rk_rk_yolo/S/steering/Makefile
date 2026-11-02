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
