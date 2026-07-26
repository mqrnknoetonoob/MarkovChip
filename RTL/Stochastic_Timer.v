module Stochastic_Timer (
    input wire clk,
    input wire rst_n,
    input wire calc_start,         // High while a round is running (== lfsr_en)

    output reg active_count_en,    // Active for 4096 cycles (0 to 4095)
    output reg timer_done          // High for 1 cycle when count reaches 4095
);

    // 12-bit counter (0 to 4095 = exactly 4096 cycles)
    reg [11:0] cycle_cnt;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cycle_cnt       <= 12'd0;
            active_count_en <= 1'b0;
            timer_done      <= 1'b0;
        end else if (calc_start) begin
            if (cycle_cnt < 12'd4095) begin
                cycle_cnt       <= cycle_cnt + 1'b1;
                active_count_en <= 1'b1; // Keep BGC counters running
                timer_done      <= 1'b0;
            end else begin
                // cycle_cnt == 4095: exactly 4096 counts have completed.
                // Hold cycle_cnt here (don't wrap), pause the BGC counters,
                // and report done. LFSR_Enabler will drop calc_start shortly
                // after seeing timer_done, which brings us to the else branch
                // below and resets everything for the next round.
                active_count_en <= 1'b0;
                timer_done      <= 1'b1;
            end
        end else begin
            // calc_start == 0: idle / just finished a round -> reset and
            // wait for the next round to be triggered.
            cycle_cnt       <= 12'd0;
            active_count_en <= 1'b0;
            timer_done      <= 1'b0;
        end
    end

endmodule