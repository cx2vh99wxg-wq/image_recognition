foreach command {cfg_read_register cfg_debug_operate cfg_execute_svf} {
 catch {$command -help} result
 puts $result
}
