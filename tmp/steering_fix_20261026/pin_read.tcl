if {[catch {cfg_connect -ip 127.0.0.1 -port 65420} result]} {puts "CONNECT_ERROR $result"; exit}
if {[catch {cfg_scan_chain} result]} {puts "SCAN_ERROR $result"; exit}
puts "PIN_READ_START"
if {[catch {cfg_debug_operate -mode 5 -device_index 0} result]} {puts "PIN_READ_ERROR $result"} else {puts "PIN_READ_RESULT $result"}
