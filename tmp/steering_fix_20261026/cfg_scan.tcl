foreach command {cfg_connect cfg_scan_chain} {
 puts "QUERY $command"
 if {[catch {$command} result]} { puts "ERROR $result" } else { puts "RESULT $result" }
}
