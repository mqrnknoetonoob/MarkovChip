module Markov_Chain_Accelerator (
    input wire clk,
    input wire rst_n,
    input wire spi_in0, 
    input wire spi_in1,
    input wire spi_in2,
    input wire spi_in3,
    input wire input_ready,
    input wire load_row_or_col,
    input wire output_valid,   // Host pulses this once it's counted the desired number of output_ready pulses
    output wire output_ready,
    output wire spi_out0,
    output wire spi_out1,
    output wire spi_out2,
    output wire spi_out3
);

    wire lfsr_en;
    wire timer_done; // fed back from Stochastic_Timer so LFSR_Enabler can re-arm
    wire active_count_en; // precise 4096-cycle counting window from Stochastic_Timer
    LFSR_Enabler lfsr_enabler (
        .clk(clk),
        .rst_n(rst_n),
        .input_ready(input_ready),
        .load_row_or_col(load_row_or_col),
        .timer_done(timer_done),
        .lfsr_en(lfsr_en)
    );

    // Hardcoded seeds
    wire [16*12-1:0] seeds_row_flat = {
        12'h85D, 12'hC7C, 12'h555, 12'h574,
        12'hE17, 12'hE37, 12'hA17, 12'hC5D,
        12'h73F, 12'h31F, 12'h8F8, 12'hCDC,
        12'h79E, 12'h31A, 12'hAB7, 12'hA96
    };
    wire [16*12-1:0] seeds_col_flat = {
        12'hE36, 12'h155, 12'h174, 12'h87C,
        12'hC7D, 12'hA36, 12'h33E, 12'h71F,
        12'h73E, 12'hA12, 12'hAE8, 12'h3BA,
        12'h3BF, 12'hAB2, 12'h79B, 12'h33B
    };

    // ----------------------------------------------------
    // 1. Memory Enable Decoder (existing module, as-is)
    // ----------------------------------------------------
    wire [3:0] en_out;
    wire [1:0] active_select;

    memory_enable_decoder dec_inst (
        .clk(clk),
        .rst_n(rst_n),
        .inc(input_ready),
        .en_out(en_out),
        .active_select(active_select)
    );

    // en_out from memory_enable_decoder:
    // en_out[3] = Group 0 (BGC 0,1,2,3)
    // en_out[2] = Group 1 (BGC 4,5,6,7)
    // en_out[1] = Group 2 (BGC 8,9,10,11)
    // en_out[0] = Group 3 (BGC 12,13,14,15)

        // ----------------------------------------------------
    // 2. Routing Mode Control to Row or Column
    // ----------------------------------------------------
    
    // Group 0 Mode Selection (States 0, 1, 2, 3)
    wire [1:0] mode_row_g0 = (input_ready && load_row_or_col && en_out[3]) ? 2'b01 : 2'b00;
    wire [1:0] mode_col_g0 = (!output_ready && input_ready && !load_row_or_col && en_out[3]) ? 2'b01 : 2'b00;

    // Group 1 Mode Selection (States 4, 5, 6, 7)
    wire [1:0] mode_row_g1 = (input_ready && load_row_or_col && en_out[2]) ? 2'b01 : 2'b00;
    wire [1:0] mode_col_g1 = (!output_ready && input_ready && !load_row_or_col && en_out[2]) ? 2'b01 : 2'b00;

    // Group 2 Mode Selection (States 8, 9, 10, 11)
    wire [1:0] mode_row_g2 = (input_ready && load_row_or_col && en_out[1]) ? 2'b01 : 2'b00;
    wire [1:0] mode_col_g2 = (!output_ready && input_ready && !load_row_or_col && en_out[1]) ? 2'b01 : 2'b00;

    // Group 3 Mode Selection (States 12, 13, 14, 15)
    wire [1:0] mode_row_g3 = (input_ready && load_row_or_col && en_out[0]) ? 2'b01 : 2'b00;
    wire [1:0] mode_col_g3 = (!output_ready && input_ready && !load_row_or_col && en_out[0]) ? 2'b01 : 2'b00;
    
    // ----------------------------------------------------
    // 3. Instantiating the 16 Row_Col_BGC Blocks
    // ----------------------------------------------------

    
    wire [16*12-1:0] results_flat;
    wire [16*12-1:0] stored_results_flat;

    // --- Group 0 (BGC 0, 1, 2, 3) ---
    Row_Col_BGC bgc_0 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in0),
        .prev_step_output(stored_results_flat[0*12 +: 12]), // Feedback from previous step for BGC 0
        .mode_col(mode_col_g0),
        .mode_row(mode_row_g0),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[0*12 +: 12]),
        .seed_col(seeds_col_flat[0*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[0*12 +: 12])
    );
    Row_Col_BGC bgc_1 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in1),
        .prev_step_output(stored_results_flat[1*12 +: 12]), // Feedback from previous step for BGC 1
        .mode_col(mode_col_g0),
        .mode_row(mode_row_g0),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[1*12 +: 12]),
        .seed_col(seeds_col_flat[1*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[1*12 +: 12])
    );
    Row_Col_BGC bgc_2 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in2),
        .prev_step_output(stored_results_flat[2*12 +: 12]), // Feedback from previous step for BGC 2
        .mode_col(mode_col_g0),
        .mode_row(mode_row_g0),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[2*12 +: 12]),
        .seed_col(seeds_col_flat[2*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[2*12 +: 12])
    );
    Row_Col_BGC bgc_3 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in3),
        .prev_step_output(stored_results_flat[3*12 +: 12]), // Feedback from previous step for BGC 3
        .mode_col(mode_col_g0),
        .mode_row(mode_row_g0),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[3*12 +: 12]),
        .seed_col(seeds_col_flat[3*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[3*12 +: 12])
    );

    // --- Group 1 (BGC 4, 5, 6, 7) ---
    Row_Col_BGC bgc_4 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in0),
        .prev_step_output(stored_results_flat[4*12 +: 12]), // Feedback from previous step for BGC 4
        .mode_col(mode_col_g1),
        .mode_row(mode_row_g1),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[4*12 +: 12]),
        .seed_col(seeds_col_flat[4*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[4*12 +: 12])
    );
    Row_Col_BGC bgc_5 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in1),
        .prev_step_output(stored_results_flat[5*12 +: 12]), // Feedback from previous step for BGC 5
        .mode_col(mode_col_g1),
        .mode_row(mode_row_g1),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[5*12 +: 12]),
        .seed_col(seeds_col_flat[5*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[5*12 +: 12])
    );
    Row_Col_BGC bgc_6 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in2),
        .prev_step_output(stored_results_flat[6*12 +: 12]), // Feedback from previous step for BGC 6
        .mode_col(mode_col_g1),
        .mode_row(mode_row_g1),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[6*12 +: 12]),
        .seed_col(seeds_col_flat[6*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[6*12 +: 12])
    );
    Row_Col_BGC bgc_7 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in3),
        .prev_step_output(stored_results_flat[7*12 +: 12]), // Feedback from previous step for BGC 7
        .mode_col(mode_col_g1),
        .mode_row(mode_row_g1),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[7*12 +: 12]),
        .seed_col(seeds_col_flat[7*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[7*12 +: 12])
    );

    // --- Group 2 (BGC 8, 9, 10, 11) ---
    Row_Col_BGC bgc_8 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in0),
        .prev_step_output(stored_results_flat[8*12 +: 12]), // Feedback from previous step for BGC 8
        .mode_col(mode_col_g2),
        .mode_row(mode_row_g2),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[8*12 +: 12]),
        .seed_col(seeds_col_flat[8*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[8*12 +: 12])
    );
    Row_Col_BGC bgc_9 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in1),
        .prev_step_output(stored_results_flat[9*12 +: 12]), // Feedback from previous step for BGC 9
        .mode_col(mode_col_g2),
        .mode_row(mode_row_g2),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[9*12 +: 12]),
        .seed_col(seeds_col_flat[9*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[9*12 +: 12])
    );
    Row_Col_BGC bgc_10 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in2),
        .prev_step_output(stored_results_flat[10*12 +: 12]), // Feedback from previous step for BGC 10
        .mode_col(mode_col_g2),
        .mode_row(mode_row_g2),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[10*12 +: 12]),
        .seed_col(seeds_col_flat[10*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[10*12 +: 12])
    );
    Row_Col_BGC bgc_11 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in3),
        .prev_step_output(stored_results_flat[11*12 +: 12]), // Feedback from previous step for BGC 11
        .mode_col(mode_col_g2),
        .mode_row(mode_row_g2),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[11*12 +: 12]),
        .seed_col(seeds_col_flat[11*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[11*12 +: 12])
    );

    // --- Group 3 (BGC 12, 13, 14, 15) ---
    Row_Col_BGC bgc_12 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in0),
        .prev_step_output(stored_results_flat[12*12 +: 12]), // Feedback from previous step for BGC 12
        .mode_col(mode_col_g3),
        .mode_row(mode_row_g3),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[12*12 +: 12]),
        .seed_col(seeds_col_flat[12*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[12*12 +: 12])
    );
    Row_Col_BGC bgc_13 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in1),
        .prev_step_output(stored_results_flat[13*12 +: 12]), // Feedback from previous step for BGC 13
        .mode_col(mode_col_g3),
        .mode_row(mode_row_g3),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[13*12 +: 12]),
        .seed_col(seeds_col_flat[13*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[13*12 +: 12])
    );
    Row_Col_BGC bgc_14 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in2),
        .prev_step_output(stored_results_flat[14*12 +: 12]), // Feedback from previous step for BGC 14
        .mode_col(mode_col_g3),
        .mode_row(mode_row_g3),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[14*12 +: 12]),
        .seed_col(seeds_col_flat[14*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[14*12 +: 12])
    );
    Row_Col_BGC bgc_15 (
        .clk(clk),
        .rst_n(rst_n),
        .in(spi_in3),
        .prev_step_output(stored_results_flat[15*12 +: 12]), // Feedback from previous step for BGC 15
        .mode_col(mode_col_g3),
        .mode_row(mode_row_g3),
        .lfsr_en(lfsr_en),
        .on_switch(lfsr_en),
        .count_en(active_count_en),
        .seed_row(seeds_row_flat[15*12 +: 12]),
        .seed_col(seeds_col_flat[15*12 +: 12]),
        .output_ready(output_ready),
        .result_ij(results_flat[15*12 +: 12])
    );



    // ----------------------------------------------------
    // 4. Adding 16 outputs from Row_Col_BGC blocks to get the final result of a state
    // ----------------------------------------------------
    
    wire [11:0] instant_sum;

    Adder_Tree_16in adder_tree (
        .results_flat(results_flat),
        .sum_out(instant_sum)
    );

    Stochastic_Timer timer_inst (
        .clk(clk),
        .rst_n(rst_n),
        .calc_start(lfsr_en),
        .active_count_en(active_count_en),
        .timer_done(timer_done)
    );

    // ----------------------------------------------------
    // 5. Storing the results of each state in a memory for feedback to the next state
    // ----------------------------------------------------

    wire output_ready_sig;

    Result_Shift_Memory res_mem_inst (
        .clk(clk),
        .rst_n(rst_n),
        .step_done_trigger(lfsr_en & timer_done),
        .sum_in(instant_sum[11:0]),
        .stored_results_flat(stored_results_flat),
        .output_ready(output_ready_sig)
        // .active_write_index()
    );

    assign output_ready = output_ready_sig;

    // ----------------------------------------------------
    // 6. Shifting the 16 stored results back out, 4 lines at a time,
    //    mirroring the input-loading scheme (round-robin groups, LSB->MSB).
    //    Triggered by the host, not automatically every round -- the host
    //    counts output_ready pulses itself and pulses output_valid once
    //    it's done enough steps.
    // ----------------------------------------------------
    Result_Output_Shifter out_shifter_inst (
        .clk(clk),
        .rst_n(rst_n),
        .output_valid(output_valid),
        .stored_results_flat(stored_results_flat),
        .out0(spi_out0),
        .out1(spi_out1),
        .out2(spi_out2),
        .out3(spi_out3)
    );

endmodule