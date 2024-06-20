set_global_assignment -entity "addf" -library "lib_addf" -name IP_TOOL_NAME "altera_fp_functions"
set_global_assignment -entity "addf" -library "lib_addf" -name IP_TOOL_VERSION "20.1"
set_global_assignment -entity "addf" -library "lib_addf" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_addf" -name SPD_FILE [file join $::quartus(sip_path) "addf.spd"]

set_global_assignment -library "lib_addf" -name MISC_FILE [file join $::quartus(sip_path) "addf_sim/addf.vo"]
