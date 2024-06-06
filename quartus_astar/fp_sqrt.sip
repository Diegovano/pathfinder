set_global_assignment -entity "fp_sqrt" -library "lib_fp_sqrt" -name IP_TOOL_NAME "altera_fp_functions"
set_global_assignment -entity "fp_sqrt" -library "lib_fp_sqrt" -name IP_TOOL_VERSION "18.1"
set_global_assignment -entity "fp_sqrt" -library "lib_fp_sqrt" -name IP_TOOL_ENV "mwpim"
set_global_assignment -library "lib_fp_sqrt" -name SPD_FILE [file join $::quartus(sip_path) "fp_sqrt.spd"]

set_global_assignment -library "lib_fp_sqrt" -name MISC_FILE [file join $::quartus(sip_path) "fp_sqrt_sim/fp_sqrt_memoryC0_uid62_sqrtTables_lutmem.hex"]
set_global_assignment -library "lib_fp_sqrt" -name MISC_FILE [file join $::quartus(sip_path) "fp_sqrt_sim/fp_sqrt_memoryC1_uid65_sqrtTables_lutmem.hex"]
set_global_assignment -library "lib_fp_sqrt" -name MISC_FILE [file join $::quartus(sip_path) "fp_sqrt_sim/fp_sqrt_memoryC2_uid68_sqrtTables_lutmem.hex"]
set_global_assignment -library "lib_fp_sqrt" -name MISC_FILE [file join $::quartus(sip_path) "fp_sqrt_sim/fp_sqrt.vo"]
