module Result_Shift_Memory (
    input wire clk,
    input wire rst_n,
    input wire step_done_trigger,   // Driven by (lfsr_en & timer_done)
    input wire [11:0] sum_in,       // Lower 12 bits of Adder Tree result
    input wire output_valid,        // Host pulse: start shifting the 16 stored results out

    output reg output_ready,        // 1 for 1 cycle when full 16-state round completes (0->15->0)
    output wire [16*12-1:0] stored_results_flat, // Feedback bus to each BGC's prev_step_output
    output wire out0, out1, out2, out3  // Serial output lines (mirrors spi_in0..3)
);

    // ----------------------------------------------------
    // Part 1: 0 to 15 Modulo-16 Counter & output_ready Pulse
    // (unchanged from before)
    // ----------------------------------------------------
    reg [3:0] write_counter;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            write_counter <= 4'd0;
            output_ready  <= 1'b0;
        end else if (step_done_trigger) begin
            write_counter <= write_counter + 1'b1;
            if (write_counter == 4'd15) begin
                output_ready <= 1'b1;
            end else begin
                output_ready <= 1'b0;
            end
        end else begin
            output_ready <= 1'b0; // 1-cycle pulse
        end
    end

    // ----------------------------------------------------
    // Part 2: 1-to-16 One-Hot Write Decoder (unchanged)
    // ----------------------------------------------------
    reg [15:0] write_enable;

    always @(*) begin
        write_enable = 16'd0;
        write_enable[write_counter] = 1'b1;
    end

    // ----------------------------------------------------
    // Part 3: Output-shift control (moved in from Result_Output_Shifter)
    // ----------------------------------------------------
    reg        shift_active;
    reg  [5:0] shift_cycle_count; // 0..47

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            shift_active      <= 1'b0;
            shift_cycle_count <= 6'd0;
        end else if (output_valid) begin
            shift_active      <= 1'b1;
            shift_cycle_count <= 6'd0;
        end else if (shift_active) begin
            if (shift_cycle_count == 6'd47) begin
                shift_active      <= 1'b0;
                shift_cycle_count <= 6'd0;
            end else begin
                shift_cycle_count <= shift_cycle_count + 1'b1;
            end
        end
    end

    wire [3:0] out_en; // out_en[3]=Group0(0-3) out_en[2]=Group1(4-7)
                        // out_en[1]=Group2(8-11) out_en[0]=Group3(12-15)
    /* verilator lint_off UNUSED */
    wire [1:0] out_active_select;
    /* verilator lint_on UNUSED */

    memory_enable_decoder out_dec_inst (
        .clk(clk),
        .rst_n(rst_n),
        .inc(shift_active),
        .en_out(out_en),
        .active_select(out_active_select)
    );

    wire group_active [3:0];
    assign group_active[0] = out_en[3];
    assign group_active[1] = out_en[2];
    assign group_active[2] = out_en[1];
    assign group_active[3] = out_en[0];

    // ----------------------------------------------------
    // Part 4: The 16 state slots -- ONE register set doing double duty:
    //   write (mode 10)  -- normal operation, captures sum_in each round
    //   shift (mode 01)  -- output read-out, drains via out0..3
    //   hold  (mode 00)  -- everywhere else, incl. while a slot is
    //                       feeding Row_Col_BGC's prev_step_output
    // Write always takes priority over shift (they can never coincide on
    // the same slot in practice -- shifting only starts well after the
    // round that produced these values -- but priority keeps it safe).
    // ----------------------------------------------------
    wire [11:0] q [15:0];

    genvar gi;
    generate
        for (gi = 0; gi < 16; gi = gi + 1) begin : slot
            wire this_group_active = group_active[gi/4];
            wire [1:0] this_mode =
                (write_enable[gi] && step_done_trigger) ? 2'b10 :
                (shift_active && this_group_active)     ? 2'b01 :
                                                            2'b00;
            Shift_Register slot_reg (
                .clk(clk),
                .reset(~rst_n),
                .mode(this_mode),
                .serial_in(1'b0),        // don't-care filler bit during shift-out
                .parallel_in(sum_in),
                .q(q[gi])
            );
        end
    endgenerate

    // (stored_results_flat is now a top-level output port -- flattened below)
    genvar fi;
    generate
        for (fi = 0; fi < 16; fi = fi + 1) begin : flatten_output
            assign stored_results_flat[fi*12 +: 12] = q[fi];
        end
    endgenerate

    // ----------------------------------------------------
    // Output mux: each line taps q[0] (current LSB) of whichever group
    // is active this cycle, for its position within the group.
    // ----------------------------------------------------
    assign out0 = out_en[3] ? q[0][0]  : out_en[2] ? q[4][0]  : out_en[1] ? q[8][0]  : out_en[0] ? q[12][0] : 1'b0;
    assign out1 = out_en[3] ? q[1][0]  : out_en[2] ? q[5][0]  : out_en[1] ? q[9][0]  : out_en[0] ? q[13][0] : 1'b0;
    assign out2 = out_en[3] ? q[2][0]  : out_en[2] ? q[6][0]  : out_en[1] ? q[10][0] : out_en[0] ? q[14][0] : 1'b0;
    assign out3 = out_en[3] ? q[3][0]  : out_en[2] ? q[7][0]  : out_en[1] ? q[11][0] : out_en[0] ? q[15][0] : 1'b0;

endmodule