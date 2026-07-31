module Adder_Tree_16in (
    input wire [16*12-1:0] results_flat, // 16 inputs of 12-bit each
    output wire [11:0] sum_out           // Pure combinational 16-bit sum
);

    // Unflattening 16 inputs
    wire [11:0] r [15:0];
    
    genvar i;
    generate
        for (i = 0; i < 16; i = i + 1) begin : unflatten
            assign r[i] = results_flat[i*12 +: 12];
        end
    endgenerate

    // Pure combinational addition (Always active)
    assign sum_out = r[0]  + r[1]  + r[2]  + r[3]  +
                     r[4]  + r[5]  + r[6]  + r[7]  +
                     r[8]  + r[9]  + r[10] + r[11] +
                     r[12] + r[13] + r[14] + r[15];

endmodule