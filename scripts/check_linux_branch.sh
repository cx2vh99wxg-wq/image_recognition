#!/bin/bash
# check_linux_branch.sh — 本机检查「板端才会编译」的分支能否过 -Werror（【人员 B】）
#
# 为什么需要它：
#   项目里大量代码在 `#if defined(__linux__)` 之内（X11 渲染、POSIX socket）。
#   在 Windows 上跑 make/local 构建时**这些分支根本不参与编译**，于是出现
#   "本机全绿、上板编译失败"——典型症状：静态函数改成没人调用后触发
#   -Wunused-function、整数/指针宽度不匹配、隐式函数声明。
#   每次上板失败 = 一次板卡往返，所以提前在本机用最小桩头文件把 __linux__
#   分支强制打开编译一遍，把这类问题挡在提交之前。
#
# 桩头文件在 scripts/x11stub/（X11/Xlib.h、X11/Xutil.h、X11/keysym.h、prelude.h），
# 只声明本项目用到的类型与函数，位宽按 aarch64 语义对齐（GC 是指针类型、
# Window/KeySym 是 64 位整型），以便复现真实编译期的类型诊断。
#
# 用法： bash scripts/check_linux_branch.sh
set -u

cd "$(dirname "$0")/.." || exit 1          # 切到仓库根，之后一律用相对路径
CC="${CC:-gcc}"
FLAGS="-std=gnu11 -D_DEFAULT_SOURCE -D__linux__ -Wall -Wextra -Werror -O2"
INC="-include scripts/x11stub/prelude.h -Iscripts/x11stub -Iplanning/include -Icommon/include"

rm -rf _linuxchk; mkdir -p _linuxchk
fail=0
for f in planning/csrc/render_lcd_x11.c; do
    printf '  %-40s ' "$f"
    if $CC $FLAGS $INC -c "$f" -o _linuxchk/x.o 2>_linuxchk/e.txt; then
        echo OK
    else
        echo FAIL
        sed 's/^/      /' _linuxchk/e.txt | head -20
        fail=1
    fi
done
rm -rf _linuxchk

if [ "$fail" = 0 ]; then
    echo "==== Linux-only 分支编译通过（与板端同款 -O2 -Wall -Wextra -Werror）===="
else
    echo "==== 有文件未通过，请修正后再上板 ===="
    exit 1
fi
