cfg_set_tcl_break -flag true
cfg_connect -ip 127.0.0.1 -port 65420
cfg_scan_chain
cfg_jtag_flash_scan_device -device_index 0
cfg_jtag_flash_assign_file -device_index 0 -file "D:/Study_Object/image_recognition/image_recognition/output/hmi_autopilot_20261113/final/pineapple_s_autopilot_20261114.sfc"
cfg_jtag_flash_erase -device_index 0
cfg_jtag_flash_program -device_index 0
cfg_jtag_flash_verify -device_index 0
