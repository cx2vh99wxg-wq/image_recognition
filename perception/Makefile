# perception/Makefile —【人员 A · 感知】模块构建
#
# 默认目标（本机 / WSL x86）：编译全部源（语法/可链接性自检）+ 运行纯逻辑单元测试。
# 板端可执行程序（perception_main）用交叉工具链单独编译到 obj/，与本地对象文件
# 完全隔离，避免“先 make（x86 对象）再 make CROSS=... bin”时把 x86 .o 错误链进
# aarch64 程序。
#
#   make                              本机编译 + 跑 6 套单测
#   make CROSS=aarch64-linux-gnu- bin  板端构建（需 B 的 common/csrc/shm_ipc.c + 板端 librknnrt）
#   make clean

CC     ?= gcc
CROSS  ?=
ARCHCC := $(CROSS)$(CC)

INCLUDE := -Iinclude -I../common/include
CFLAGS  := -std=c11 -Wall -Wextra -O2 $(INCLUDE)

# 本地（本机 / WSL x86）编译的源
MODULE_SRCS := csrc/pcie_capture.c csrc/lane_detect.c csrc/turn_decide.c \
               csrc/lane_mark.c csrc/zebra_detect.c csrc/traffic_light.c \
               csrc/isp_params.c \
               csrc/perception_pipeline.c csrc/lane_stub.c
TEST_SRCS   := test/test_capture.c test/test_lane.c test/test_turn.c \
               test/test_lane_mark.c test/test_zebra.c test/test_traffic_light.c \
               test/test_isp_params.c \
               test/test_stub.c \
               test/rknn_dummy.c

.PHONY: all native_objs tests bin clean

all: native_objs tests

# 本地对象（仅编译，不链接 main：main 需要板端 B 的 shm_ipc.c 才能链接）
native_objs: $(MODULE_SRCS:.c=.o) csrc/main_perception.o

csrc/%.o: csrc/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test/%.o: test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

tests: test/test_capture test/test_lane test/test_turn \
       test/test_lane_mark test/test_zebra test/test_traffic_light \
       test/test_isp_params test/test_stub
	@echo "---- 运行单元测试 ----"
	./test/test_capture
	./test/test_lane
	./test/test_turn
	./test/test_lane_mark
	./test/test_zebra
	./test/test_traffic_light
	./test/test_isp_params
	./test/test_stub

test/test_capture: test/test_capture.o csrc/pcie_capture.o test/rknn_dummy.o
	$(CC) -o $@ $^ -lm

test/test_lane: test/test_lane.o csrc/lane_detect.o test/rknn_dummy.o
	$(CC) -o $@ $^ -lm

test/test_turn: test/test_turn.o csrc/turn_decide.o csrc/lane_detect.o test/rknn_dummy.o
	$(CC) -o $@ $^ -lm

test/test_lane_mark: test/test_lane_mark.o csrc/lane_mark.o csrc/lane_detect.o test/rknn_dummy.o
	$(CC) -o $@ $^ -lm

test/test_zebra: test/test_zebra.o csrc/zebra_detect.o
	$(CC) -o $@ $^ -lm

test/test_traffic_light: test/test_traffic_light.o csrc/traffic_light.o
	$(CC) -o $@ $^ -lm

test/test_isp_params: test/test_isp_params.o csrc/isp_params.o
	$(CC) -o $@ $^ -lm

test/test_stub: test/test_stub.o csrc/perception_pipeline.o csrc/lane_stub.o \
                csrc/lane_detect.o csrc/turn_decide.o csrc/lane_mark.o \
                csrc/zebra_detect.o csrc/traffic_light.o csrc/pcie_capture.o \
                test/rknn_dummy.o
	$(CC) -o $@ $^ -lm

# ---- 板端构建：交叉编译到 obj/，与本地对象完全隔离 ----
BOARD_OBJS := obj/main_perception.o obj/pcie_capture.o obj/lane_detect.o \
              obj/turn_decide.o obj/lane_mark.o obj/zebra_detect.o \
              obj/traffic_light.o obj/isp_params.o obj/perception_pipeline.o \
              obj/lane_stub.o \
              obj/shm_ipc.o

obj/%.o: csrc/%.c
	@mkdir -p obj
	$(ARCHCC) $(CFLAGS) -c $< -o $@

obj/shm_ipc.o: ../common/csrc/shm_ipc.c
	@mkdir -p obj
	$(ARCHCC) $(CFLAGS) -c $< -o $@

bin: $(BOARD_OBJS)
	$(ARCHCC) -o perception_main $^ -lm -lrknnrt

clean:
	rm -f csrc/*.o test/*.o obj/*.o test/test_capture test/test_lane \
	      test/test_turn test/test_lane_mark test/test_zebra test/test_stub \
	      test/test_traffic_light test/test_isp_params \
	      perception_main
