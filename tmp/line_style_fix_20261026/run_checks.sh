#!/bin/bash
set -eu
cd /mnt/d/Study_Object/image_recognition/image_recognition
root=$PWD
out=tmp/line_style_fix_20261026
inc=(-Iudp_rk_rk_yolo/M/include -Iudp_rk_rk_yolo/M/pcie/include)
g++ -std=c++11 -Wall -Wextra -Werror -O2 "${inc[@]}" udp_rk_rk_yolo/M/pcie/tests/test_line_style.cpp -o "$out/test_line_style"
"$out/test_line_style" >"$out/unit.log"
g++ -std=c++11 -Wall -Wextra -Werror -O1 -fsanitize=address,undefined "${inc[@]}" udp_rk_rk_yolo/M/pcie/tests/test_line_style.cpp -o "$out/test_line_style_asan"
"$out/test_line_style_asan" >"$out/asan.log"
g++ -std=c++11 -Wall -Wextra -Werror -O2 "${inc[@]}" udp_rk_rk_yolo/M/pcie/tools/line_style_replay.cpp -o "$out/replay"
g++ -std=c++11 -Wall -Wextra -Werror -O2 -pthread "${inc[@]}" tmp/m_worker_test.cpp -o "$out/test_worker"
"$out/test_worker" >"$out/worker.log"
g++ -std=c++11 -Wall -Wextra -Werror -fsyntax-only -Iudp_rk_rk_yolo/M/include -Iudp_rk_rk_yolo/M/udp/include udp_rk_rk_yolo/M/udp/csrc/main.cpp
g++ -std=c++11 -Wall -Wextra -Werror -fsyntax-only -Iudp_rk_rk_yolo/S/include -Iudp_rk_rk_yolo/S/udp/include udp_rk_rk_yolo/S/udp/csrc/main.cpp
make -C udp_rk_rk_yolo/S/x11/csrc BUILD_DIR="$root/$out/host_renderer" -j2 >"$out/renderer-build.log" 2>&1
echo 'Unit, ASan/UBSan, async worker, UDP compile and renderer build PASS'
