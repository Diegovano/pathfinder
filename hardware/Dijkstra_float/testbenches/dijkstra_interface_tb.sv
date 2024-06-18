`include "../hdl/constants.v"
`timescale 1ps/1ps

`define NUMBER_OF_NODES 128
`define TEST_VECTOR_FILE "testvectors/testvectors.txt"

function string get_tv_path();
    int i;
    int str_index;
    logic found_path;
    static string fullpath_filename = `__FILE__;
    static string test_vector_file = `TEST_VECTOR_FILE;
    automatic string ret="";


	$display("~~~~~~~~~~~~~~~~~~~~~~~~");
	$display("%s", fullpath_filename);
    for (i = fullpath_filename.len()-1; i>0; i=i-1) begin
        if (fullpath_filename[i] == "/" || fullpath_filename[i] == "\\")
        begin
            found_path=1;
            str_index=i;
            break;
        end
    end
    if (found_path===1) begin
        ret=fullpath_filename.substr(0,str_index);
    end else begin
       	$fatal("Bleeaaargh");
    end
        

    return {ret, test_vector_file};
endfunction

integer row;
integer column;
integer i;
integer j;
integer num_test_cases;
integer seed;
integer testvectors;
integer _;



module DijkstraTopTestbench
();
    reg reset = 0;
    reg clock = 0;
    reg clock_enable = 0;
    reg start = 0;

    reg [7:0] select_n;
    reg [31:0] dataa;
    reg [31:0] datab;
    reg [31:0] result;
    reg ready = 0;

    reg [16:0] number_of_nodes = `NUMBER_OF_NODES;
    reg [31:0] tb_write_data;

    reg [6:0] path_vector [127:0];
    reg [31:0] shortest_distance;

    DijkstraInterface DUT(
        .reset(reset),
        .start(start),
        .clock(clock),
        .clock_enable(1),
        .select_n(select_n),
        .dataa(dataa),
        .datab(datab),
        .result(result),
        .ready(ready)
    );
    always #300000 clock_enable = ~clock_enable;
    always #10000 clock = ~clock;

    initial begin
        testvectors = $fopen(get_tv_path(), "r");
		if(testvectors == 0)
			$fatal(1, "Couldn't load test vectors");
		_  = $fscanf(testvectors, "%d", num_test_cases);
        for(i=0;i<num_test_cases;i=i+1)
		begin
			_ = $fscanf(testvectors, "%d", number_of_nodes);
			_ = $fscanf(testvectors, "%d", seed);
			@(posedge clock);
			@(posedge clock);
            reset = 1;
            @(posedge clock);
            reset = 0;
            for(row=0;row<number_of_nodes;row=row+1)
				for(column=0;column<number_of_nodes;column=column+1)
				begin
					// Load edge value into graph
					_ = $fscanf(testvectors, "%f", tb_write_data);
                    //$display("writing data %d \n", tb_write_data);
					// Write to edge cache
					select_n = 0;
                    dataa = {column[15:0],row[15:0]};
                    datab = tb_write_data;
                    //clock_enable = 1;
                    start = 1;
                    @(posedge clock);
                    start = 0;
                    wait(ready);
                    @(posedge clock);
                    select_n = 1;
                    dataa = {column[15:0],row[15:0]};
                    start = 1;
                    @(posedge clock);
                    start = 0;
                    wait(ready);
                    @(posedge clock);
                    if (result !== tb_write_data) begin
                        $display("failed to read back edge value, expecting: %f, got: %f", tb_write_data, result);
                        $finish;
                    end
				end
            //start dijkstra
            select_n = 2;
            dataa = {{number_of_nodes-1}[15:0],16'b0};
            datab = number_of_nodes;
            start = 1;
            @(posedge clock);
            start = 0;
            wait(ready);

            $display("Shortest distance: %f", result);
            //wait for dijkstra to finish
            for(row=0;row<number_of_nodes;row=row+1)begin
                _ = $fscanf(testvectors, "%d", path_vector[row]);
            end
            _ = $fscanf(testvectors, "%f", shortest_distance);
            if (result !== shortest_distance) begin
                $display("shortest distance not correct, expecting: %f, got: %f", shortest_distance, result);
                $finish;
            end
            // for (row=0;row<number_of_nodes;row=row+1) begin
            //     select_n = 3;
            //     dataa = {16'b0, row[15:0]};
            //     datab = 0;
            //     start = 1; 
            //     @(posedge clock);
            //     start = 0;
            //     wait(ready);
            //     if (result !== path_vector[row]) begin
            //         $display("path not patched, expecting: %d, got: %d", path_vector[row], result);
            //         $finish;
            //     end
            // end
        end

        $finish;

    end

endmodule
    