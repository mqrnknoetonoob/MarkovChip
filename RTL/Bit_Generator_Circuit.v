// for verification of seeds

module Bit_Generator_Circuit (
    input wire clk,
    input wire rst_n,
    input wire shift_en,         // Enable for Seed Memory shift
    input wire data_in,          // Serial data input for Seed Memory
    input wire [11:0] par_in,    // Parallel input for Shift Register
    input wire [1:0] mode,       // Mode selection for Shift Register
    input wire lfsr_en,          // LFSR enable signal
    input wire count_en,         // Enable signal for the counter
    input wire on_switch,        // Switch to turn the counter on/off
    output wire [11:0] total_count // Final count of 1s
);

    // Internal signals for connecting the modules
    wire [11:0] seed;
    wire [11:0] lfsr_out;
    wire [11:0] shift_reg_out;
    wire comp_result;

    // 1. Seed Memory: Stores the seed value
    Seed_Memory sm (
        .clk(clk),
        .rst_n(rst_n),
        .shift_en(shift_en),
        .data_in(data_in),
        .q(seed)
    );

    // 2. LFSR: Generates pseudo-random numbers based on the seed
    LFSR #(.SEED(12'd1)) u_lfsr (
        .clk(clk),
        .rst_n(rst_n),
        .en(lfsr_en),
        .q(lfsr_out)
    );

    // 3. Shift Register: Stores the reference value B
    Shift_Register sr (
        .clk(clk),
        .reset(~rst_n),
        .mode(mode),
        .serial_in(data_in), 
        .parallel_in(par_in),
        .q(shift_reg_out)
    );

    // 4. Comparator: Compares LFSR (A) and Shift Register (B)
    // If LFSR > Register, x = 0; otherwise, x = 1
    Comparator comp (
        .A(lfsr_out),       
        .B(shift_reg_out),  
        .x(comp_result)     
    );

    // 5. Counter: Counts the number of times comp_result is 1
    counter final_counter (
        .clk(clk),
        .rst_n(rst_n),
        .on_switch(on_switch),
        // Increment only if counter is enabled AND comparator output is 1
        .count_en(count_en && comp_result), 
        .count(total_count)
    );

endmodule