foreach command {cfg_assign_file cfg_jtag_flash_scan_device cfg_jtag_flash_assign_file cfg_jtag_flash_program cfg_jtag_flash_verify cfg_flash_program cfg_reset_fpga cfg_verify} {
    catch {$command -help} result
    puts "COMMAND $command: $result"
}
