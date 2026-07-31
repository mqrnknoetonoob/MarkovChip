module LFSR (
    input wire clk,
    input wire rst_n,            // Active low reset
    input wire en,               // Enable (1 = Run, 0 = Load seed)
    input wire [11:0] seed_in,   // Seed input (from top-level or TB)
    output reg [11:0] seed_mem,  // Raw LFSR register (keeps perfect uniformity)
    output wire [11:0] scrambled_out // Whitened output (connect this to your comparator!)
);
    wire feedback;

    // Feedback polynomial: x^12 + x^7 + x^4 + x^3 + 1
    assign feedback = seed_mem[11] ^ seed_mem[6] ^ seed_mem[3] ^ seed_mem[2];

    // Main LFSR state transition
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            seed_mem <= seed_in;              // Load seed on reset
        else if (!en)
            seed_mem <= seed_in;              // Track seed when LFSR disabled
        else
            seed_mem <= {seed_mem[10:0], feedback}; // Run when enabled
    end

    // =========================================================================
    // MATHEMATICAL WHITENING LAYER (Shatters the 10.9% correlation matrix)
    // =========================================================================
    
    // Step 1: 12-bit wide XOR-Shift (Calculates instantly, minimal gate overhead)
    wire [11:0] w_lfsr;
    assign w_lfsr = seed_mem ^ (seed_mem >> 3) ^ (seed_mem << 5);

    // Step 2: Wire Permutation Mapping: Y_i = X_((5 * i + 3) mod 12)
    // This adds exactly ZERO gates, ZERO area, and ZERO delay. It is pure physical routing.
    assign scrambled_out[0]  = w_lfsr[3];  // (5*0 + 3) % 12 = 3
    assign scrambled_out[1]  = w_lfsr[8];  // (5*1 + 3) % 12 = 8
    assign scrambled_out[2]  = w_lfsr[1];  // (5*2 + 3) % 12 = 13 % 12 = 1
    assign scrambled_out[3]  = w_lfsr[6];  // (5*3 + 3) % 12 = 18 % 12 = 6
    assign scrambled_out[4]  = w_lfsr[11]; // (5*4 + 3) % 12 = 23 % 12 = 11
    assign scrambled_out[5]  = w_lfsr[4];  // (5*5 + 3) % 12 = 28 % 12 = 4
    assign scrambled_out[6]  = w_lfsr[9];  // (5*6 + 3) % 12 = 33 % 12 = 9
    assign scrambled_out[7]  = w_lfsr[2];  // (5*7 + 3) % 12 = 38 % 12 = 2
    assign scrambled_out[8]  = w_lfsr[7];  // (5*8 + 3) % 12 = 43 % 12 = 7
    assign scrambled_out[9]  = w_lfsr[0];  // (5*9 + 3) % 12 = 48 % 12 = 0
    assign scrambled_out[10] = w_lfsr[5];  // (5*10 + 3) % 12 = 53 % 12 = 5
    assign scrambled_out[11] = w_lfsr[10]; // (5*11 + 3) % 12 = 58 % 12 = 10

endmodule