cfg_set_tcl_break -flag true
cfg_connect -ip 127.0.0.1 -port 65420
cfg_scan_chain
cfg_assign_file -device_index 0 -file "D:/Study_Object/image_recognition/image_recognition/output/front_person_sr_20261116/final/project/generate_bitstream/image_pcie_capture.sbit"
cfg_program -device_index 0
cfg_scan_chain
cfg_verify -device_index 0
