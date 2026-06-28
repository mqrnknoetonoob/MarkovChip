module Shift_Register (
    input clk,                // Clock signal
    input reset,              // Reset to clear registers
    input [1:0] mode,         // Control signal (00, 01, 10)
    input serial_in,          // Input for shifting (Condition 1)
    input [11:0] parallel_in, // Input for loading (Condition 2)
    output reg [11:0] q       // 12-bit Register output
);

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            q <= 12'b0;
        end else begin
            case (mode)
                // Condition 3: Data remains unchanged (No operation)
                2'b00: q <= q; 

                // Condition 1: Shift right and serial input at MSB
                // MSB (q[11]) takes serial_in, rest shift right
                2'b01: q <= {serial_in, q[11:1]}; 

                // Condition 2: Parallel load of 12-bit input
                2'b10: q <= parallel_in; 
                
                default: q <= q;
            endcase
        end
    end

endmodule