module LFSR (
    input wire clk,
    input wire rst_n,          // Active low reset
    input wire en,             // Enable (1 = Run, 0 = Load seed)
    input wire [11:0] seed_in, // Seed input (from top-level or TB)
    output reg [11:0] seed_mem
);
    wire feedback;

    // Feedback polynomial: x^12 + x^7 + x^4 + x^3 + 1
    assign feedback = seed_mem[11] ^ seed_mem[6] ^ seed_mem[3] ^ seed_mem[2];

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            seed_mem <= seed_in;              // Load seed on reset
        else if (!en)
            seed_mem <= seed_in;              // Track seed when LFSR disabled
        else
            seed_mem <= {seed_mem[10:0], feedback}; // Run when enabled
    end
endmodule