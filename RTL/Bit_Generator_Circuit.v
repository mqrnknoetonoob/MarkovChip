module Bit_Generator_Circuit (
    input wire clk,
    input wire rst_n,
    input wire [11:0] seed_in,       // Seed: hardcoded from top-level or driven by TB
    input wire [11:0] par_in,        // Parallel input for Shift Register (B value)
    input wire [1:0] mode,           // Mode selection for Shift Register
    input wire lfsr_en,              // LFSR enable signal
    input wire count_en,             // Enable signal for the counter
    input wire on_switch,            // Switch to turn the counter on/off
    output wire [11:0] total_count   // Final count of 1s
);
    wire [11:0] lfsr_out;
    wire [11:0] shift_reg_out;
    wire comp_result;

    // 1. LFSR: seed loaded from seed_in when rst_n or lfsr_en=0
    LFSR u_lfsr (
        .clk(clk),
        .rst_n(rst_n),
        .en(lfsr_en),
        .seed_in(seed_in),
        .seed_mem(lfsr_out)
    );

    // 2. Shift Register: Stores the reference value B
    Shift_Register sr (
        .clk(clk),
        .reset(~rst_n),
        .mode(mode),
        .serial_in(1'b0),
        .parallel_in(par_in),
        .q(shift_reg_out)
    );

    // 3. Comparator: Compares LFSR output (A) and reference B
    Comparator comp (
        .A(lfsr_out),
        .B(shift_reg_out),
        .x(comp_result)
    );

    // 4. Counter: Counts the number of 1s from comparator output
    counter final_counter (
        .clk(clk),
        .rst_n(rst_n),
        .on_switch(on_switch),
        .count_en(count_en && comp_result),
        .count(total_count)
    );

endmodule