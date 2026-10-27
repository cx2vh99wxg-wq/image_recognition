cfg_connect -ip 127.0.0.1 -port 65420
cfg_scan_chain
cfg_jtag_flash_scan_device -device_index 0
cfg_jtag_flash_assign_file -device_index 0 -file "D:/Study_Object/image_recognition/image_recognition/fpga/fpga_pcie_ov5640/project/generate_bitstream/image_pcie_capture_line_style_20261026.sfc"
cfg_jtag_flash_erase -device_index 0
cfg_jtag_flash_program -device_index 0
cfg_jtag_flash_verify -device_index 0
