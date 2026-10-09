#!/bin/bash
# stop_all.sh — 停止全部进程并清理共享内存（【人员 B】集成）
#
# 双端各跑一次。先杀进程，再 ipcrm 删除共享段（key 与 driving_config.h 一一对应）。
# 注意：IPC_RMID 会立即删除段，其他仍映射该段的进程将失去数据——请在停止场景使用。

echo "==== 停止进程 ===="
sudo pkill -f perception_main 2>/dev/null || true
sudo pkill -f udp_m_send_main 2>/dev/null || true
sudo pkill -f planning_main    2>/dev/null || true
sleep 1

echo "==== 清理共享内存 ===="
for key in 0x12345679 0x1234567A 0x1234567B 0x1234567C 0x1234567D \
           0x1234567E 0x1234567F 0x12345680 0x12345681; do
    sudo ipcrm -M $((key)) 2>/dev/null && echo "  删除 key $key" || true
done

echo "==== 停止完成 ===="
