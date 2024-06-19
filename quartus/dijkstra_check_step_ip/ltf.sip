set_global_assignment -entity "ltf" -library "lib_ltf" -name IP_TOOL_NAME "altera_fp_functions"
set_global_assignment -entity "ltf" -library "lib_ltf" -name IP_TOOL_VERSION "20.1"
set_global_assignment -entity "ltf" -library "lib_ltf" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_ltf" -name SPD_FILE [file join $::quartus(sip_path) "ltf.spd"]

set_global_assignment -library "lib_ltf" -name MISC_FILE [file join $::quartus(sip_path) "ltf_sim/ltf.vo"]
