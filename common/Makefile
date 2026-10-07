# common/Makefile — 公共库（【人员 B】主责）构建
#
#   make                              本机编译纯逻辑模块 + 跑协议单测
#   make CROSS=aarch64-linux-gnu-      板端交叉编译（含 shm_ipc.c，依赖 sys/shm.h）
#   make clean
#
# 注意：shm_ipc.c 使用 System V 共享内存（sys/shm.h），本机（Windows/非Linux）
# 不编译；板端（Linux/aarch64）才编。纯逻辑模块（log/udp_proto）两端都编。

CC     ?= gcc
CROSS  ?=
ARCHCC := $(CROSS)$(CC)

INCLUDE := -Iinclude
# gnu11：严格 c11 会隐藏 glibc 的 POSIX 声明（clock_gettime/usleep 等）
CFLAGS  := -std=gnu11 -D_DEFAULT_SOURCE -Wall -Wextra -Werror -O2 $(INCLUDE)

# 本机可编（纯逻辑）
NATIVE_SRCS := csrc/log.c csrc/udp_proto.c
# 板端额外（System V 共享内存）
BOARD_SRCS  := csrc/shm_ipc.c

.PHONY: all native_objs board tests clean

all: native_objs tests

native_objs: $(NATIVE_SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tests: test/test_udp_proto
	@echo "---- 运行 common 单元测试 ----"
	./test/test_udp_proto

test/test_udp_proto: test/test_udp_proto.o csrc/udp_proto.o csrc/log.o
	$(CC) -o $@ $^

test/%.o: test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

board:
	@mkdir -p obj
	$(ARCHCC) $(CFLAGS) -c csrc/shm_ipc.c -o obj/shm_ipc.o
	$(ARCHCC) $(CFLAGS) -c csrc/udp_proto.c -o obj/udp_proto.o
	$(ARCHCC) $(CFLAGS) -c csrc/log.c -o obj/log.o
	@echo "common board objects done (obj/)"

clean:
	rm -f csrc/*.o test/*.o obj/*.o test/test_udp_proto
