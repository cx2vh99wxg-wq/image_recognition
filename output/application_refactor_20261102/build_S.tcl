open_project "D:/Study_Object/image_recognition/image_recognition/fpga/fpga_pcie_ov5640/project_s_application_refactor_20261102/project_s_refactor.pds"
compile -force_to_run -top_module image_pcie_capture
synthesize -ads -force_to_run -selected_syn_tool_opt 2
dev_map -force_to_run
pnr -force_to_run -gplace_seed 3 -groute_seed 3 -mode normal -optimize_multi_corner_timing -fix_hold_violation -slack_weight 90
report_timing -force_to_run
gen_bit_stream -force_to_run -compress_bitstream true -master_configuration_clock_frequency {40M}
