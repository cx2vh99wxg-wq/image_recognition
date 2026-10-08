# control/Makefile — 控制+执行（【人员 C · 控制与 FPGA】）构建
#
#   make                              本机编译纯逻辑 + 桩 driver（语法/链接自检）+ 跑单测
#   make USE_STUB=1                   桩模式（fspi_driver 非 Linux 走桩分支）
#   make CROSS=aarch64-linux-gnu- bin 板端：control_main（S 板控制进程）
#   make clean
#
# 编译规范（分工方案 7.4）：统一 -std=c11 -Wall -Wextra -Werror。
# 注意：严格 c11（非 gnu11）会隐藏 glibc 的 POSIX 声明（usleep/clock_gettime/
# useconds_t），故加 -D_DEFAULT_SOURCE 兜底——与 B 在 planning/Makefile 中记录的
# 结论一致。板端 bin 链接 System V 共享内存（common obj/shm_ipc.o）。

CC     ?= gcc
CROSS  ?=
ARCHCC := $(CROSS)$(CC)

INCLUDE := -Iinclude -I../common/include
CFLAGS  := -std=c11 -D_DEFAULT_SOURCE -Wall -Wextra -Werror -O2 $(INCLUDE)

# 桩模式：make USE_STUB=1 时强制 fspi_driver 走桩分支（无硬件联调）
ifeq ($(USE_STUB),1)
CFLAGS += -DUSE_STUB
endif

# 本机可编（纯逻辑 + 桩 driver）：driver 非 Linux 或 USE_STUB 走 #if 桩分支
NATIVE_SRCS := csrc/control_logic.c csrc/fspi_driver.c
TEST_SRCS   := test/test_control_logic.c

.PHONY: all native_objs test tests bin clean

all: native_objs tests

test: tests

native_objs: $(NATIVE_SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test/%.o: test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

tests: test/test_control_logic
	@echo "---- 运行 control 单元测试 ----"
	./test/test_control_logic

test/test_control_logic: test/test_control_logic.o csrc/control_logic.o
	$(CC) -o $@ $^

# ---- 板端构建（S 端控制进程 control_main） ----
BOARD_OBJS := obj/control_logic.o obj/fspi_driver.o obj/main_control.o \
              obj/shm_ipc.o obj/log.o

obj/%.o: csrc/%.c
	@mkdir -p obj
	$(ARCHCC) $(CFLAGS) -c $< -o $@

obj/shm_ipc.o: ../common/csrc/shm_ipc.c
	@mkdir -p obj
	$(ARCHCC) $(CFLAGS) -c $< -o $@

obj/log.o: ../common/csrc/log.c
	@mkdir -p obj
	$(ARCHCC) $(CFLAGS) -c $< -o $@

bin: $(BOARD_OBJS)
	$(ARCHCC) -o control_main $^ -lrt
	@echo "board binary: control_main(S)"

clean:
	rm -f csrc/*.o test/*.o obj/*.o test/test_control_logic control_main
