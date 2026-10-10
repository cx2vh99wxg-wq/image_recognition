# Run in the PDS Tcl console AFTER creating an empty PG2L100H/FBG484 project.
# Paths are relative to this script, so the checkout can be moved.
# Change only the path in your source {...} command on another computer.
# PGL50H/Logos is NOT a substitute for PG2L100H/Logos2.
set stereo_src_root [file dirname [file normalize [info script]]]
set stereo_design_files {
    {image_pcie_capture.v}
    {drive_camera_top.v}
    {hmi_regs.v}
    {night_isp.v}
    {camera_health.v}
    {safe_fspi.v}
    {../source/uart_lcd/uart_rx.v}
    {../stereo_j8/stereo_axi_ctrl.v}
    {../stereo_j8/stereo_reg_config.v}
    {../stereo_j8/stereo_i2c_com.v}
    {../source/pcie/pcie_dma_ctrl/ipm_distributed_sdpram_v1_2.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_controller.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_cpld_tx_ctrl.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_mrd_tx_ctrl.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_mwr_tx_ctrl.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_rd_ctrl.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_rx_cpld_wr_ctrl.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_rx_mwr_wr_ctrl.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_rx_top.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_tlp_rcv.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_tlp_tx_mux.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_tx_cpld_rd_ctrl.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_tx_mwr_rd_ctrl.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_tx_top.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_wr_ctrl.v}
    {../source/pcie/pcie_dma_ctrl/pgs_pciex4_prefetch_fifo_v1_2.v}
    {../source/pcie/pcie_dma_ctrl/fifo/ipm_distributed_sdpram_v1_2_distributed_fifo.v}
    {../source/pcie/pcie_dma_ctrl/fifo/pgs_pciex4_fifo_ctrl.v}
    {../source/pcie/pcie_dma_ctrl/fifo/pgs_pciex4_fifo_v1_2.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_ram/ips2l_pcie_dma_ram.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_ram/rtl/ipm2l_sdpram_v1_1_ips2l_pcie_dma_ram.v}
    {../source/pcie/pcie_dma_ctrl/ips2l_pcie_dma_ram/rtl/ips2l_pcie_dma_ram_init_param.v}
    {../source/ov5640/rtl/cmos_capture_data.v}
    {../project/ipcore/ddr3/ddr3.idf}
    {../project/ipcore/clk_1080p_gen/clk_1080p_gen.idf}
    {../project/ipcore/pcie_test/pcie_test.idf}
    {../project/ipcore/W_FIFO_16i_128o/W_FIFO_16i_128o.idf}
    {../project/ipcore/R_FIFO_128i_128o/R_FIFO_128i_128o.idf}
}
# Fail before changing the project if the copied source tree is incomplete.
set stereo_missing {}
foreach rel [concat $stereo_design_files {drive_6ch.fdc}] {
    set path [file normalize [file join $stereo_src_root $rel]]
    if {![file isfile $path] || ![file readable $path]} {
        lappend stereo_missing $path
    }
}
if {[llength $stereo_missing] != 0} {
    error "Incomplete stereo source tree. Copy drive_6ch, stereo_j8, source and project/ipcore together. Missing/unreadable:\n[join $stereo_missing \n]"
}
foreach command {set_arch add_design add_constraint} {
    if {[llength [info commands $command]] == 0} {
        error "PDS command '$command' is unavailable. Run this script inside PDS Tcl Console with an empty project open."
    }
}
if {[catch {set_arch -family Logos2 -device PG2L100H -speedgrade -6 -package FBG484} stereo_arch_error]} {
    error "Cannot select the required PG2L100H-6/FBG484 (Logos2). Check the open project, PDS version, device support and license. Do NOT substitute PGL50H/Logos. Original PDS error: $stereo_arch_error"
}
foreach rel $stereo_design_files {
    add_design [file normalize [file join $stereo_src_root $rel]]
}
add_constraint [file join $stereo_src_root "drive_6ch.fdc"]
puts "Full drive_6ch import complete: [llength $stereo_design_files] design entries + 1 FDC. Verify PG2L100H, FBG484, -6 in Project Settings."
# Next: compile -top_module image_pcie_capture, then synthesize, map, place/route.
