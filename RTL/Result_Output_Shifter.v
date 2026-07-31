module Result_Output_Shifter (
    input  wire clk,
    input  wire rst_n,
    input  wire output_valid,           // Host-driven, 1-cycle pulse: "I've counted enough output_ready pulses, give me the results now"
    input  wire [16*12-1:0] stored_results_flat,

    output wire out0, out1, out2, out3    // Serial output lines (mirrors spi_in0..3)
);

    // ----------------------------------------------------
    // Control: load once, then shift for 48 cycles (12 bits x 4 groups,
    // round-robin every cycle -- same cadence as the input side)
    // ----------------------------------------------------
    reg        shift_active;
    reg  [5:0] shift_cycle_count; // 0..47

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            shift_active      <= 1'b0;
            shift_cycle_count <= 6'd0;
        end else if (output_valid) begin
            // This is the load cycle; shifting begins next cycle.
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

    // shift_active stays internal now -- the host doesn't need a status
    // flag since the protocol is deterministic: exactly 48 cycles after
    // output_valid, all the data has been shifted out.

    // ----------------------------------------------------
    // Group round-robin selector (own instance -- independent of the
    // input-side decoder's counter state)
    // ----------------------------------------------------
    wire [3:0] out_en; // out_en[3]=Group0(0-3) out_en[2]=Group1(4-7)
                        // out_en[1]=Group2(8-11) out_en[0]=Group3(12-15)
    /* verilator lint_off UNUSED */
    wire [1:0] out_active_select;
    /* verilator lint_off UNUSED */
    
    memory_enable_decoder out_dec_inst (
        .clk(clk),
        .rst_n(rst_n),
        .inc(shift_active),
        .en_out(out_en),
        .active_select(out_active_select)
    );

    // ----------------------------------------------------
    // Per-group mode control (10 = load, 01 = shift while it's this
    // group's turn, 00 = hold otherwise)
    // ----------------------------------------------------
    wire [1:0] mode_g0 = output_valid ? 2'b10 : (shift_active && out_en[3]) ? 2'b01 : 2'b00;
    wire [1:0] mode_g1 = output_valid ? 2'b10 : (shift_active && out_en[2]) ? 2'b01 : 2'b00;
    wire [1:0] mode_g2 = output_valid ? 2'b10 : (shift_active && out_en[1]) ? 2'b01 : 2'b00;
    wire [1:0] mode_g3 = output_valid ? 2'b10 : (shift_active && out_en[0]) ? 2'b01 : 2'b00;

    // ----------------------------------------------------
    // 16 output shift registers (reusing Shift_Register), grouped exactly
    // like the input side: BGC 0,1,2,3 / 4,5,6,7 / 8,9,10,11 / 12,13,14,15
    // ----------------------------------------------------
    wire [11:0] q [15:0];

    genvar gi;
    generate
        for (gi = 0; gi < 16; gi = gi + 1) begin : out_regs
            wire [1:0] this_mode = (gi < 4)  ? mode_g0 :
                                    (gi < 8)  ? mode_g1 :
                                    (gi < 12) ? mode_g2 : mode_g3;
            Shift_Register out_reg (
                .clk(clk),
                .reset(~rst_n),
                .mode(this_mode),
                .serial_in(1'b0),                          // don't-care filler bit
                .parallel_in(stored_results_flat[gi*12 +: 12]),
                .q(q[gi])
            );
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