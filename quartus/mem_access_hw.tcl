# TCL File Generated by Component Editor 20.1
# Thu Jun 13 18:11:31 BST 2024
# DO NOT MODIFY


# 
# mem_access "mem_access" v1.4
#  2024.06.13.18:11:31
# 
# 

# 
# request TCL package from ACDS 16.1
# 
package require -exact qsys 16.1


# 
# module mem_access
# 
set_module_property DESCRIPTION ""
set_module_property NAME mem_access
set_module_property VERSION 1.4
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR ""
set_module_property DISPLAY_NAME mem_access
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL direct_dijkstra
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE true
add_fileset_file direct_dijkstra.sv SYSTEM_VERILOG PATH direct_dijkstra_ip/direct_dijkstra.sv TOP_LEVEL_FILE


# 
# parameters
# 


# 
# display items
# 


# 
# connection point nios_custom_instruction_slave
# 
add_interface nios_custom_instruction_slave nios_custom_instruction end
set_interface_property nios_custom_instruction_slave clockCycle 0
set_interface_property nios_custom_instruction_slave operands 2
set_interface_property nios_custom_instruction_slave ENABLED true
set_interface_property nios_custom_instruction_slave EXPORT_OF ""
set_interface_property nios_custom_instruction_slave PORT_NAME_MAP ""
set_interface_property nios_custom_instruction_slave CMSIS_SVD_VARIABLES ""
set_interface_property nios_custom_instruction_slave SVD_ADDRESS_GROUP ""

add_interface_port nios_custom_instruction_slave dataa dataa Input 32
add_interface_port nios_custom_instruction_slave datab datab Input 32
add_interface_port nios_custom_instruction_slave result result Output 32
add_interface_port nios_custom_instruction_slave done done Output 1
add_interface_port nios_custom_instruction_slave ci_clk_en clk_en Input 1
add_interface_port nios_custom_instruction_slave start start Input 1
add_interface_port nios_custom_instruction_slave ci_clk clk Input 1
add_interface_port nios_custom_instruction_slave ci_reset reset Input 1


# 
# connection point clock
# 
add_interface clock clock end
set_interface_property clock clockRate 0
set_interface_property clock ENABLED true
set_interface_property clock EXPORT_OF ""
set_interface_property clock PORT_NAME_MAP ""
set_interface_property clock CMSIS_SVD_VARIABLES ""
set_interface_property clock SVD_ADDRESS_GROUP ""

add_interface_port clock clk clk Input 1


# 
# connection point reset
# 
add_interface reset reset end
set_interface_property reset associatedClock clock
set_interface_property reset synchronousEdges DEASSERT
set_interface_property reset ENABLED true
set_interface_property reset EXPORT_OF ""
set_interface_property reset PORT_NAME_MAP ""
set_interface_property reset CMSIS_SVD_VARIABLES ""
set_interface_property reset SVD_ADDRESS_GROUP ""

add_interface_port reset reset reset Input 1


# 
# connection point data_slave
# 
add_interface data_slave avalon end
set_interface_property data_slave addressUnits WORDS
set_interface_property data_slave associatedClock clock
set_interface_property data_slave associatedReset reset
set_interface_property data_slave bitsPerSymbol 8
set_interface_property data_slave burstOnBurstBoundariesOnly false
set_interface_property data_slave burstcountUnits WORDS
set_interface_property data_slave explicitAddressSpan 0
set_interface_property data_slave holdTime 0
set_interface_property data_slave linewrapBursts false
set_interface_property data_slave maximumPendingReadTransactions 0
set_interface_property data_slave maximumPendingWriteTransactions 0
set_interface_property data_slave readLatency 0
set_interface_property data_slave readWaitTime 1
set_interface_property data_slave setupTime 0
set_interface_property data_slave timingUnits Cycles
set_interface_property data_slave writeWaitTime 0
set_interface_property data_slave ENABLED true
set_interface_property data_slave EXPORT_OF ""
set_interface_property data_slave PORT_NAME_MAP ""
set_interface_property data_slave CMSIS_SVD_VARIABLES ""
set_interface_property data_slave SVD_ADDRESS_GROUP ""

add_interface_port data_slave writedata writedata Input 32
add_interface_port data_slave write write Input 1
set_interface_assignment data_slave embeddedsw.configuration.isFlash 0
set_interface_assignment data_slave embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment data_slave embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment data_slave embeddedsw.configuration.isPrintableDevice 0

