module LFSR_Enabler (
    input wire clk,
    input wire rst_n,
    input wire input_ready,
    input wire load_row_or_col,   // 1 = currently loading a row, 0 = loading a column
    input wire timer_done,        // from Stochastic_Timer: pulses when the 4096-cycle round finishes
    output reg lfsr_en
);

    // A full row load (4 groups, round-robin, 12 bits each) takes 48 input_ready
    // pulses while load_row_or_col == 1. Change this if the group-switch timing
    // ends up being different from that assumption.
    localparam [5:0] ROW_LOAD_CYCLES = 6'd48;

    reg [5:0] row_load_count;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            row_load_count <= 6'd0;
            lfsr_en        <= 1'b0;
        end else if (lfsr_en) begin
            // A round is currently running (LFSR free-running, timer counting).
            // Stay enabled until the timer reports this round is done, then
            // drop back to idle so we can be re-armed for the next row.
            if (timer_done) begin
                lfsr_en        <= 1'b0;
                row_load_count <= 6'd0;
            end
            // else: hold lfsr_en high, ignore input_ready/load_row_or_col here.
        end else if (input_ready && load_row_or_col) begin
            // Idle, and currently in a row-loading phase: count pulses toward
            // a full row load.
            if (row_load_count == ROW_LOAD_CYCLES - 1'b1) begin
                row_load_count <= 6'd0;
                lfsr_en        <= 1'b1;   // row is fully loaded -> start this round
            end else begin
                row_load_count <= row_load_count + 1'b1;
            end
        end
        // Column-loading phase (load_row_or_col == 0) while idle: nothing to
        // count here, just wait.
    end

endmodule