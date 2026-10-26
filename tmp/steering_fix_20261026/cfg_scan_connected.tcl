if {[catch {cfg_connect -ip 127.0.0.1 -port 65420} result]} {puts "CONNECT_ERROR $result"} else {
 puts "CONNECT_RESULT $result"
 if {[catch {cfg_scan_chain} result]} {puts "SCAN_ERROR $result"} else {puts "SCAN_RESULT $result"}
}
