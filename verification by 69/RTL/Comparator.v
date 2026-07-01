module Comparator (
    input [11:0] A,          // 12-bit input A
    input [11:0] B,          // 12-bit input B
    output x
);

    wire [11:0] GL_bits;
    wire [11:0] GR_bits;
    
    // Generate Greater-Than bits for both paths
    assign GL_bits = A & ~B;
    assign GR_bits = B & ~A;

    wire [3:0] GL_index;     // 4-bit index matching the encoder output
    wire [3:0] GR_index;     // 4-bit index matching the encoder output

    // Instantiate Priority Encoder for Left Path
    priority_encoder pe1 (
        .in(GL_bits),
        .out(GL_index)
    );

    // Instantiate Priority Encoder for Right Path
    priority_encoder pe2 (
        .in(GR_bits),
        .out(GR_index)
    );

    // Final comparison logic: Output 1 if A > B, else 0
    assign x = (GL_index > GR_index) ? 1'b0 : 1'b1;

endmodule
