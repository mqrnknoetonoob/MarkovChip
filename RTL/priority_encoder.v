module priority_encoder (
    input [11:0] in,
    output reg [3:0] out
);

    integer i;

    always @(*) begin
        // Default output value when no bit is set
        out = 4'd0;

        // Loop through all 12 bits to find the highest priority set bit
        for (i = 0; i < 12; i = i + 1) begin
            if (in[i] == 1'b1) begin
                // Cast the 32-bit integer 'i' to 4-bits to prevent Verilator width warnings
                out = i[3:0] + 4'd1;
            end
        end
    end
endmodule