set_global_assignment -entity "lef" -library "lib_lef" -name IP_TOOL_NAME "altera_fp_functions"
set_global_assignment -entity "lef" -library "lib_lef" -name IP_TOOL_VERSION "20.1"
set_global_assignment -entity "lef" -library "lib_lef" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_lef" -name SPD_FILE [file join $::quartus(sip_path) "lef.spd"]

set_global_assignment -library "lib_lef" -name MISC_FILE [file join $::quartus(sip_path) "lef_sim/lef.vo"]
