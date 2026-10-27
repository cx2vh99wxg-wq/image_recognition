foreach command {cfg_help cfg_gen_sfc cfg_add_device cfg_set_device_property cfg_download cfg_program cfg_load_config_file} {
    catch {$command -help} result
    puts "COMMAND $command: $result"
}
puts [join [lsort [info commands cfg_*]] "\n"]
