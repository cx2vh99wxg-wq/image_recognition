# planning/Makefile — 决策+通信+显示（【人员 B】）构建
#
#   make                              本机编译纯逻辑 + 跑单测（decision/udp 重组）
#   make USE_STUB=1                   桩模式编译（person_detect 走桩）
#   make CROSS=aarch64-linux-gnu- bin 板端：planning_main(S) + udp_m_send_main(M)
#   make clean
#
# 本机（Windows/非Linux）：socket/X11/rknn 代码自动走桩分支（文件内 #if），
# 因此 person_detect/render_lcd/udp_sender/udp_receiver 也可在本机编译通过。
# 板端 bin 链接 System V 共享内存（common obj/shm_ipc.o）。

CC     ?= gcc
CROSS  ?=
ARCHCC := $(CROSS)$(CC)

INCLUDE := -Iinclude -I../common/include -I../perception/include
# 用 gnu11 而非严格 c11：严格 ISO 模式下 glibc 会隐藏 POSIX 声明，
# 导致 clock_gettime/usleep/useconds_t 在板端（gcc + -Werror）编译失败。
# -D_DEFAULT_SOURCE 作为兜底（即使有人改回 -std=c11 也能拿到 POSIX 声明）。
CFLAGS  := -std=gnu11 -D_DEFAULT_SOURCE -Wall -Wextra -Werror -O2 $(INCLUDE)

COMMON_SRCS := ../common/csrc/udp_proto.c ../common/csrc/log.c
NATIVE_SRCS := csrc/decision.c csrc/person_detect.c csrc/render_lcd_x11.c \
               csrc/udp_sender.c csrc/udp_receiver.c csrc/stub_pattern.c csrc/frame_compose.c
TEST_SRCS   := test/test_decision.c test/test_udp.c

.PHONY: all native_objs tests bin clean

all: native_objs tests

native_objs: $(NATIVE_SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test/%.o: test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

tests: test/test_decision test/test_udp test/test_compose
	@echo "---- 运行 planning 单元测试 ----"
	./test/test_decision
	./test/test_udp
	./test/test_compose

test/test_decision: test/test_decision.o csrc/decision.o
	$(CC) -o $@ $^

test/test_udp: test/test_udp.o csrc/udp_receiver.o ../common/csrc/udp_proto.o
	$(CC) -o $@ $^

test/test_compose: test/test_compose.o csrc/frame_compose.o csrc/stub_pattern.o csrc/udp_receiver.o ../common/csrc/udp_proto.o
	$(CC) -o $@ $^

# ---- 板端构建（S 端决策进程 + M 端 UDP 发送器） ----
BOARD_SRCS := csrc/main_planning.c csrc/decision.c csrc/person_detect.c \
              csrc/render_lcd_x11.c csrc/udp_sender.c csrc/udp_receiver.c \
              ../common/csrc/shm_ipc.c ../common/csrc/udp_proto.c ../common/csrc/log.c

# bin: -L../lib 指向仓库根 lib/librknnrt.so（RKNN 运行时，板端链接用）
bin:
	@mkdir -p obj
	$(ARCHCC) $(CFLAGS) -L../lib -o planning_main csrc/main_planning.c csrc/decision.c \
	    csrc/person_detect.c csrc/render_lcd_x11.c csrc/udp_sender.c \
	    csrc/udp_receiver.c csrc/stub_pattern.c csrc/frame_compose.c ../perception/csrc/pcie_capture.c ../common/csrc/shm_ipc.c \
	    ../common/csrc/udp_proto.c \
	    ../common/csrc/log.c -lm -lrt -lX11 -lrknnrt
	$(ARCHCC) $(CFLAGS) -o udp_m_send_main csrc/udp_m_send_main.c csrc/udp_sender.c \
	    csrc/stub_pattern.c ../common/csrc/shm_ipc.c ../common/csrc/udp_proto.c \
	    ../common/csrc/log.c \
	    -lrt
	@echo "board binaries: planning_main(S) udp_m_send_main(M)"

clean:
	rm -f csrc/*.o test/*.o obj/*.o test/test_decision test/test_udp \
	      test/test_compose planning_main udp_m_send_main
