// Seed_Sweep_Top.v
//
// VERIFICATION HARNESS ONLY - not part of the chip RTL.
//
// Instantiates 256 independent LFSR + Comparator + counter pipelines, one per
// seed value from the 256-seed list documented in RTL/LFSR.v, all sharing the
// same clock, reset, enable, and comparison threshold. This lets a single
// simulation run collect a `total_count` result for every seed at once,
// instead of resimulating 256 times.
//
// Reuses the actual verified RTL modules unmodified: LFSR.v, Comparator.v,
// priority_encoder.v, counter.v.

`include "seed_list_256.vh"

module Seed_Sweep_Top (
    input  wire        clk,
    input  wire        rst_n,
    input  wire [11:0] threshold_B,  // shared comparison threshold for all 256 seeds
    input  wire        lfsr_en,
    input  wire        count_en,
    input  wire        on_switch,
    output wire [11:0] total_count [0:255]  // per-seed final counts
);

    genvar i;
    generate
        for (i = 0; i < 256; i = i + 1) begin : seed_inst
            wire [11:0] lfsr_out;
            wire        comp_result;

            LFSR #(.SEED(SEED_LIST[i])) u_lfsr (
                .clk   (clk),
                .rst_n (rst_n),
                .en    (lfsr_en),
                .q     (lfsr_out)
            );

            Comparator u_comp (
                .A (lfsr_out),
                .B (threshold_B),
                .x (comp_result)
            );

            counter u_counter (
                .clk       (clk),
                .rst_n     (rst_n),
                .on_switch (on_switch),
                .count_en  (count_en && comp_result),
                .count     (total_count[i])
            );
        end
    endgenerate

endmodule
