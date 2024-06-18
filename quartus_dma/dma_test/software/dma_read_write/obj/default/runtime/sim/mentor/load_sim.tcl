# ------------------------------------------------------------------------------
# Top Level Simulation Script to source msim_setup.tcl
# ------------------------------------------------------------------------------
set QSYS_SIMDIR obj/default/runtime/sim
source msim_setup.tcl
# Copy generated memory initialization hex and dat file(s) to current directory
file copy -force C:/Users/vrnan/FYP_Pathfinder/QuartusTest/dma_test/software/dma_read_write/mem_init/hdl_sim/sdram.dat ./ 
