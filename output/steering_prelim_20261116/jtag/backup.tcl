cfg_set_tcl_break -flag true
cfg_connect -ip 127.0.0.1 -port 65420
cfg_scan_chain
cfg_jtag_flash_scan_device -device_index 0
cfg_jtag_flash_assign_file -device_index 0 -file "D:/Study_Object/image_recognition/image_recognition/output/steering_prelim_20261116/jtag/previous_known_image.sfc"
cfg_jtag_flash_readback -device_index 0 -file "D:/Study_Object/image_recognition/image_recognition/output/steering_prelim_20261116/jtag/S_flash_before_prelim.sfc"
