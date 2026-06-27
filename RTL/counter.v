module counter (
    input wire clk,           // Clock signal
    input wire rst_n,         // Active-low asynchronous reset
    input wire on_switch,     // Switch to initiate/stop counting
    input wire count_en,      // Increment enable
    output reg [11:0] count   // 12-bit counter output
);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            // Reset to 0
            count <= 12'b0;
        end else if (!on_switch) begin
            // If the switch is off, reset the counter to 0 immediately
            count <= 12'b0;
        end else if (count_en) begin
            // If switch is on and enable is 1, increment the counter
            // It will naturally count up to 4095 and roll over to 0 if kept running
            count <= count + 1'b1;
        end
        // If count_en is 0, the counter holds its current value
    end

endmodule