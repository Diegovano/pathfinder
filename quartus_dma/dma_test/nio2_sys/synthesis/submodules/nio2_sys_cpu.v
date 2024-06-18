// nio2_sys_cpu.v

// This file was auto-generated from altera_nios2_hw.tcl.  If you edit it your changes
// will probably be lost.
// 
// Generated using ACDS version 18.1 625

`timescale 1 ps / 1 ps
module nio2_sys_cpu (
		input  wire        clk,                                 //                       clk.clk
		input  wire        reset_n,                             //                     reset.reset_n
		input  wire        reset_req,                           //                          .reset_req
		output wire [24:0] d_address,                           //               data_master.address
		output wire [3:0]  d_byteenable,                        //                          .byteenable
		output wire        d_read,                              //                          .read
		input  wire [31:0] d_readdata,                          //                          .readdata
		input  wire        d_waitrequest,                       //                          .waitrequest
		output wire        d_write,                             //                          .write
		output wire [31:0] d_writedata,                         //                          .writedata
		input  wire        d_readdatavalid,                     //                          .readdatavalid
		output wire        debug_mem_slave_debugaccess_to_roms, //                          .debugaccess
		output wire [24:0] i_address,                           //        instruction_master.address
		output wire        i_read,                              //                          .read
		input  wire [31:0] i_readdata,                          //                          .readdata
		input  wire        i_waitrequest,                       //                          .waitrequest
		input  wire        i_readdatavalid,                     //                          .readdatavalid
		input  wire [31:0] irq,                                 //                       irq.irq
		output wire        debug_reset_request,                 //       debug_reset_request.reset
		input  wire [8:0]  debug_mem_slave_address,             //           debug_mem_slave.address
		input  wire [3:0]  debug_mem_slave_byteenable,          //                          .byteenable
		input  wire        debug_mem_slave_debugaccess,         //                          .debugaccess
		input  wire        debug_mem_slave_read,                //                          .read
		output wire [31:0] debug_mem_slave_readdata,            //                          .readdata
		output wire        debug_mem_slave_waitrequest,         //                          .waitrequest
		input  wire        debug_mem_slave_write,               //                          .write
		input  wire [31:0] debug_mem_slave_writedata,           //                          .writedata
		input  wire        A_ci_multi_done,                     // custom_instruction_master.done
		input  wire [31:0] A_ci_multi_result,                   //                          .multi_result
		output wire [4:0]  A_ci_multi_a,                        //                          .multi_a
		output wire [4:0]  A_ci_multi_b,                        //                          .multi_b
		output wire [4:0]  A_ci_multi_c,                        //                          .multi_c
		output wire        A_ci_multi_clk_en,                   //                          .clk_en
		output wire        A_ci_multi_clock,                    //                          .clk
		output wire        A_ci_multi_reset,                    //                          .reset
		output wire        A_ci_multi_reset_req,                //                          .reset_req
		output wire [31:0] A_ci_multi_dataa,                    //                          .multi_dataa
		output wire [31:0] A_ci_multi_datab,                    //                          .multi_datab
		output wire [7:0]  A_ci_multi_n,                        //                          .multi_n
		output wire        A_ci_multi_readra,                   //                          .multi_readra
		output wire        A_ci_multi_readrb,                   //                          .multi_readrb
		output wire        A_ci_multi_start,                    //                          .start
		output wire        A_ci_multi_writerc                   //                          .multi_writerc
	);

	nio2_sys_cpu_cpu cpu (
		.clk                                 (clk),                                 //                       clk.clk
		.reset_n                             (reset_n),                             //                     reset.reset_n
		.reset_req                           (reset_req),                           //                          .reset_req
		.d_address                           (d_address),                           //               data_master.address
		.d_byteenable                        (d_byteenable),                        //                          .byteenable
		.d_read                              (d_read),                              //                          .read
		.d_readdata                          (d_readdata),                          //                          .readdata
		.d_waitrequest                       (d_waitrequest),                       //                          .waitrequest
		.d_write                             (d_write),                             //                          .write
		.d_writedata                         (d_writedata),                         //                          .writedata
		.d_readdatavalid                     (d_readdatavalid),                     //                          .readdatavalid
		.debug_mem_slave_debugaccess_to_roms (debug_mem_slave_debugaccess_to_roms), //                          .debugaccess
		.i_address                           (i_address),                           //        instruction_master.address
		.i_read                              (i_read),                              //                          .read
		.i_readdata                          (i_readdata),                          //                          .readdata
		.i_waitrequest                       (i_waitrequest),                       //                          .waitrequest
		.i_readdatavalid                     (i_readdatavalid),                     //                          .readdatavalid
		.irq                                 (irq),                                 //                       irq.irq
		.debug_reset_request                 (debug_reset_request),                 //       debug_reset_request.reset
		.debug_mem_slave_address             (debug_mem_slave_address),             //           debug_mem_slave.address
		.debug_mem_slave_byteenable          (debug_mem_slave_byteenable),          //                          .byteenable
		.debug_mem_slave_debugaccess         (debug_mem_slave_debugaccess),         //                          .debugaccess
		.debug_mem_slave_read                (debug_mem_slave_read),                //                          .read
		.debug_mem_slave_readdata            (debug_mem_slave_readdata),            //                          .readdata
		.debug_mem_slave_waitrequest         (debug_mem_slave_waitrequest),         //                          .waitrequest
		.debug_mem_slave_write               (debug_mem_slave_write),               //                          .write
		.debug_mem_slave_writedata           (debug_mem_slave_writedata),           //                          .writedata
		.A_ci_multi_done                     (A_ci_multi_done),                     // custom_instruction_master.done
		.A_ci_multi_result                   (A_ci_multi_result),                   //                          .multi_result
		.A_ci_multi_a                        (A_ci_multi_a),                        //                          .multi_a
		.A_ci_multi_b                        (A_ci_multi_b),                        //                          .multi_b
		.A_ci_multi_c                        (A_ci_multi_c),                        //                          .multi_c
		.A_ci_multi_clk_en                   (A_ci_multi_clk_en),                   //                          .clk_en
		.A_ci_multi_clock                    (A_ci_multi_clock),                    //                          .clk
		.A_ci_multi_reset                    (A_ci_multi_reset),                    //                          .reset
		.A_ci_multi_reset_req                (A_ci_multi_reset_req),                //                          .reset_req
		.A_ci_multi_dataa                    (A_ci_multi_dataa),                    //                          .multi_dataa
		.A_ci_multi_datab                    (A_ci_multi_datab),                    //                          .multi_datab
		.A_ci_multi_n                        (A_ci_multi_n),                        //                          .multi_n
		.A_ci_multi_readra                   (A_ci_multi_readra),                   //                          .multi_readra
		.A_ci_multi_readrb                   (A_ci_multi_readrb),                   //                          .multi_readrb
		.A_ci_multi_start                    (A_ci_multi_start),                    //                          .start
		.A_ci_multi_writerc                  (A_ci_multi_writerc)                   //                          .multi_writerc
	);

endmodule
