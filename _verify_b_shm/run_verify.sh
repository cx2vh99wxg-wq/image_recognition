#!/bin/bash
# 端到端验证：A 端进程写共享内存 → B 端读取（人员 A 自查脚本）
set -e
ROOT="/mnt/d/学习用品/image_recognition_孪生系统(1)/image_recognition"
VERIFY="$ROOT/_verify_b_shm"

echo "==== 编译 A 端进程（桩模式，含 shm 写入）===="
gcc -std=c11 -O2 -I"$ROOT/perception/include" -I"$ROOT/common/include" \
  "$ROOT/perception/csrc/pcie_capture.c" "$ROOT/perception/csrc/lane_detect.c" \
  "$ROOT/perception/csrc/turn_decide.c" "$ROOT/perception/csrc/lane_mark.c" \
  "$ROOT/perception/csrc/zebra_detect.c" "$ROOT/perception/csrc/traffic_light.c" \
  "$ROOT/perception/csrc/isp_params.c" "$ROOT/perception/csrc/perception_pipeline.c" \
  "$ROOT/perception/csrc/lane_stub.c" "$ROOT/common/csrc/shm_ipc.c" \
  "$ROOT/perception/test/rknn_dummy.c" \
  "$ROOT/perception/csrc/main_perception.c" -o /tmp/perception_stub -lm
echo "编译 OK"

echo "==== 编译 B 端读取器 ===="
gcc -std=c11 -O2 -I"$ROOT/common/include" -I"$ROOT/perception/include" \
  "$VERIFY/test_b_side.c" "$ROOT/common/csrc/shm_ipc.c" -o /tmp/b_side_reader
echo "编译 OK"

echo "==== 编译 真实识别注入器 ===="
gcc -std=c11 -O2 -I"$ROOT/perception/include" -I"$ROOT/common/include" \
  "$VERIFY/demo_real_detect_write.c" "$ROOT/perception/csrc/traffic_light.c" \
  "$ROOT/perception/csrc/zebra_detect.c" "$ROOT/perception/csrc/lane_mark.c" \
  "$ROOT/perception/csrc/lane_detect.c" "$ROOT/perception/test/rknn_dummy.c" \
  "$ROOT/common/csrc/shm_ipc.c" -o /tmp/demo_write -lm
echo "编译 OK"

echo ""
echo "==== 清理旧共享内存段 ===="
for k in 0x12345679 0x1234567e 0x1234567f 0x12345680 0x12345681; do
  ipcrm -M "$k" 2>/dev/null || true
done

echo ""
echo "==== 场景1：A 端进程实时写帧，B 端边跑边读 ===="
/tmp/perception_stub --stub > /tmp/stub.log 2>&1 &
PID=$!
sleep 1
/tmp/b_side_reader || true
RC1=$?
kill "$PID" 2>/dev/null || true

echo ""
echo "==== 场景2：真实识别结果（红灯+斑马线+实线）注入共享内存，B 端读取 ===="
/tmp/demo_write
/tmp/b_side_reader once || true
RC2=$?

echo ""
echo "==== 清理 ===="
for k in 0x12345679 0x1234567e 0x1234567f 0x12345680 0x12345681; do
  ipcrm -M "$k" 2>/dev/null || true
done
rm -f /tmp/perception_stub /tmp/b_side_reader /tmp/demo_write /tmp/stub.log
echo "exit codes: scene1=$RC1 scene2=$RC2"
