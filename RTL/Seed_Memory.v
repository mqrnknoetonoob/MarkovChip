module Seed_Memory (
    input wire clk,
    input wire rst_n,      // Active low reset
    input wire shift_en,   // Shift enable signal
    input wire data_in,    // Input bit from external source
    output reg [11:0] q    // Parallel output (the stored seed)
);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            q <= 12'd0; // Reset clears all bits to 0
        else if (shift_en)
            // Shift left: current bits move left, new bit enters at index 0
            q <= {q[10:0], data_in};
    end
endmodule