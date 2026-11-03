open_project "D:/Study_Object/image_recognition/image_recognition/output/rtl_refactor_20261103/pds_expanded/project_m_application_refactor_20261102/project_m_refactor.pds"
puts "PINEAPPLE_BEGIN compile"
compile -force_to_run -top_module image_pcie_capture_m_imu
puts "PINEAPPLE_END compile"
puts "PINEAPPLE_BEGIN synthesize"
synthesize -ads -force_to_run -selected_syn_tool_opt 2
puts "PINEAPPLE_END synthesize"
puts "PINEAPPLE_BEGIN device_map"
dev_map -force_to_run
puts "PINEAPPLE_END device_map"
puts "PINEAPPLE_BEGIN place_route"
pnr -force_to_run -gplace_seed 3 -groute_seed 3 -mode normal -optimize_multi_corner_timing -fix_hold_violation -slack_weight 90
puts "PINEAPPLE_END place_route"
puts "PINEAPPLE_BEGIN timing"
report_timing -force_to_run
puts "PINEAPPLE_END timing"
