module LFSR #(
    parameter [11:0] SEED
)(
    input wire clk,
    input wire rst_n, // Active low reset
    input wire en,    // Enable signal
    output reg [11:0] q
);

    wire feedback;

    // Feedback polynomial: x^12 + x^7 + x^4 + x^3 + 1
    // Tap positions: 12, 7, 4, 3 (Zero-indexed: 11, 6, 3, 2)
    assign feedback = q[11] ^ q[6] ^ q[3] ^ q[2];

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            q <= SEED; // Load seed on reset
        else if (en)
            q <= {q[10:0], feedback}; // Shift and update
    end
endmodule
