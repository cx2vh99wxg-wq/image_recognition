open_project "D:/Study_Object/image_recognition/image_recognition/output/hmi_autopilot_20261113/final/project/project.pds"
compile -force_to_run -top_module image_pcie_capture
synthesize -ads -force_to_run -selected_syn_tool_opt 2
dev_map -force_to_run
pnr -force_to_run -gplace_seed 3 -groute_seed 3 -mode normal -optimize_multi_corner_timing -fix_hold_violation -slack_weight 90
report_timing -force_to_run