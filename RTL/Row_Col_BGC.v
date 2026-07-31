module Row_Col_BGC (
    input wire clk,
    input wire rst_n,
    input wire in,                   // Input detecting signal for row
    //input wire [11:0] row_in,        // Parallel input for row
    input wire [11:0] prev_step_output,   // Feedback value from the sum of another state (Step 1 output)
    input wire [1:0] mode_col,       // Mode control for Column (driven by top FSM)
    input wire [1:0] mode_row,       // Mode control for Row (driven by top FSM)
    input wire lfsr_en,              // LFSR run enable
    input wire on_switch,            // Switch to turn the counter on/off
    input wire count_en,             // Enable signal for the counter
    input wire [11:0] seed_row,      // Seed for row LFSR
    input wire [11:0] seed_col,      // Seed for column LFSR
    input wire output_ready,         // 1 = Step 1 complete, load feedback for Step 2
    output wire [11:0] result_ij     // Final counter output / result of this block
);

    // Internal Wires
    wire [11:0] row_reg_out;
    wire [11:0] col_reg_out;
    
    
    // =========================================================================
    // New 2 wires: for storing scrambled or whitened random numbers
    // =========================================================================
    wire [11:0] lfsr_row_scrambled;
    wire [11:0] lfsr_col_scrambled;
    
    wire comp_row_out;
    wire comp_col_out;
    wire multiplication_result;

    // --- Column MUX & Mode Selection ---
    wire [1:0] col_reg_mode  = output_ready ? 2'b10 : mode_col;
    wire [11:0] col_data_in  = output_ready ? prev_step_output : 12'd0;

    // --- Row Mode Selection ---
    wire [1:0] row_reg_mode  = output_ready ? 2'b00 : mode_row;

    // 1. Row Shift Register
    Shift_Register row_reg (
        .clk(clk),
        .reset(~rst_n),
        .mode(row_reg_mode),
        .serial_in(in),
        .parallel_in(12'd0),
        .q(row_reg_out)
    );

    // 2. Column Shift Register
    Shift_Register col_reg (
        .clk(clk),
        .reset(~rst_n),
        .mode(col_reg_mode),
        .serial_in(in),
        .parallel_in(col_data_in),
        .q(col_reg_out)
    );

    // =========================================================================
    // lint off PINCONNECTEMPTY
    // =========================================================================
    /* verilator lint_off PINCONNECTEMPTY */

    // 3. Row LFSR
    LFSR lfsr_row (
        .clk          (clk),
        .rst_n        (rst_n),
        .en           (lfsr_en),
        .seed_in      (seed_row),
        .seed_mem     (),       
        .scrambled_out(lfsr_row_scrambled)
    );

    // 4. Column LFSR
    LFSR lfsr_col (
        .clk          (clk),
        .rst_n        (rst_n),
        .en           (lfsr_en),
        .seed_in      (seed_col),
        .seed_mem     (),
        .scrambled_out(lfsr_col_scrambled)
    );

    /* verilator lint_on PINCONNECTEMPTY */

    // 5. Comparators 
    // (.A gets scrambled LFSR output, .B gets Shift Register output)
    Comparator comp_row (
        .A(lfsr_row_scrambled),
        .B(row_reg_out),
        .x(comp_row_out)
    );

    Comparator comp_col (
        .A(lfsr_col_scrambled),
        .B(col_reg_out),
        .x(comp_col_out)
    );

    // 6. Stochastic Multiplication
    assign multiplication_result = comp_row_out & comp_col_out;

    // 7. Counter
    counter final_counter (
        .clk(clk),
        .rst_n(rst_n),
        .on_switch(on_switch),
        .count_en(count_en && multiplication_result),
        .count(result_ij)
    );

endmodule