# TCL File Generated by Component Editor 20.1
# Fri Jun 14 16:17:20 BST 2024
# DO NOT MODIFY


# 
# dijkstra_check_step "dijkstra_check_step" v1.6
#  2024.06.14.16:17:20
# dist[min_index] + graph[min_index][i], checking dist[min_index] != INT_MAX
# 

# 
# request TCL package from ACDS 16.1
# 
package require -exact qsys 16.1


# 
# module dijkstra_check_step
# 
set_module_property DESCRIPTION "dist\[min_index] + graph\[min_index]\[i], checking dist\[min_index] != INT_MAX"
set_module_property NAME dijkstra_check_step
set_module_property VERSION 1.6
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property AUTHOR ""
set_module_property DISPLAY_NAME dijkstra_check_step
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL dijkstra_check_step
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE true
add_fileset_file dijkstra_check_step.sv SYSTEM_VERILOG PATH dijkstra_check_step_ip/dijkstra_check_step.sv TOP_LEVEL_FILE

add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL dijkstra_check_step
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property SIM_VERILOG ENABLE_FILE_OVERWRITE_MODE true
add_fileset_file dspba_library_package.vhd VHDL PATH dijkstra_check_step_ip/fp_add/dspba_library_package.vhd
add_fileset_file dijkstra_check_step.sv SYSTEM_VERILOG PATH dijkstra_check_step_ip/dijkstra_check_step.sv
add_fileset_file fp_add.v VERILOG PATH dijkstra_check_step_ip/fp_add.v
add_fileset_file fp_add_0002.vhd VHDL PATH dijkstra_check_step_ip/fp_add/fp_add_0002.vhd


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
add_interface_port nios_custom_instruction_slave clk_en clk_en Input 1
add_interface_port nios_custom_instruction_slave start start Input 1
add_interface_port nios_custom_instruction_slave done done Output 1
add_interface_port nios_custom_instruction_slave result result Output 32
add_interface_port nios_custom_instruction_slave clk clk Input 1
add_interface_port nios_custom_instruction_slave reset reset Input 1

