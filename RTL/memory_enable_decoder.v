module memory_enable_decoder (
    input wire clk,
    input wire rst_n,                // Active-low asynchronous reset
    input wire inc,                  // Increment trigger for the modulo-3 counter
    
    // 4-bit one-hot enable outputs acting as input_enable signals
    // en_out[3] -> Enable for Register 0
    // en_out[2] -> Enable for Register 1
    // en_out[1] -> Enable for Register 2
    // en_out[0] -> Enable for Register 3
    output reg [3:0] en_out,
    
    // Debug port to monitor current counter value (0, 1, or 2)
    output wire [1:0] active_select
);

    // 2-bit register to store counter state (valid values: 0, 1, 2)
    reg [1:0] sel_counter;

    // ----------------------------------------------------
    // Modulo-4 Counter (0 -> 1 -> 2 -> 3 -> 0)
    // ----------------------------------------------------
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            sel_counter <= 2'd0;
        end else if (inc) begin
            if (sel_counter == 2'd3) begin
                sel_counter <= 2'd0; // Roll back to 0
            end else begin
                sel_counter <= sel_counter + 1'b1;
            end
        end
    end

    // Expose the counter state externally
    assign active_select = sel_counter;

    // ----------------------------------------------------
    // Decoder Logic: Map counter value to One-Hot Enable
    // ----------------------------------------------------
    always @(*) begin
        case (sel_counter)
            2'd0:    en_out = 4'b1000; // Enable Register 0, disable others
            2'd1:    en_out = 4'b0100; // Enable Register 1, disable others
            2'd2:    en_out = 4'b0010; // Enable Register 2, disable others
            2'd3:    en_out = 4'b0001; // Enable Register 3, disable others
            default: en_out = 4'b0000; // Safe default (all disabled)
        endcase
    end

endmodule