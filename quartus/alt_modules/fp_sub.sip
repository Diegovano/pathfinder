set_global_assignment -entity "fp_sub" -library "lib_fp_sub" -name IP_TOOL_NAME "altera_fp_functions"
set_global_assignment -entity "fp_sub" -library "lib_fp_sub" -name IP_TOOL_VERSION "20.1"
set_global_assignment -entity "fp_sub" -library "lib_fp_sub" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_fp_sub" -name SPD_FILE [file join $::quartus(sip_path) "fp_sub.spd"]

set_global_assignment -library "lib_fp_sub" -name MISC_FILE [file join $::quartus(sip_path) "fp_sub_sim/fp_sub.vo"]
