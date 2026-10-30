# Run prepare_project.ps1 once after a fresh checkout.
# This builds files only; it never programs a board.
set imu_project_dir [file dirname [file normalize [info script]]]
open_project [file join $imu_project_dir project_m_imu.pds]
compile -force_to_run -top_module image_pcie_capture_m_imu
synthesize -ads -force_to_run -selected_syn_tool_opt 2
dev_map -force_to_run
pnr -force_to_run -gplace_seed 3 -groute_seed 3 -mode normal -optimize_multi_corner_timing -fix_hold_violation -slack_weight 90
report_timing -force_to_run
# Review the timing report before programming any newly rebuilt image.
gen_bit_stream -force_to_run -compress_bitstream true -master_configuration_clock_frequency {40M}
