cfg_set_tcl_break -break true
cfg_connect -ip 127.0.0.1 -port 65420
cfg_scan_chain
cfg_jtag_flash_scan_device -device_index 0
cfg_jtag_flash_readback -device_index 0 -file "D:/Study_Object/image_recognition/image_recognition/output/hmi_autopilot_20261113/final/S_flash_before_autopilot_20261114_192513.sfc"
