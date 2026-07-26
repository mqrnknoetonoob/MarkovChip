module Result_Shift_Memory (
    input wire clk,
    input wire rst_n,
    input wire step_done_trigger,              // Driven by (lfsr_en & timer_done)
    input wire [11:0] sum_in,                  // Lower 12 bits of Adder Tree result
    output wire [16*12-1:0] stored_results_flat,// 16 stored state results (12 bits each)
    output reg output_ready,                   // 1 for 1 cycle when full 16-state round completes (0->15->0)
    output wire [3:0] active_write_index       // Debug port to monitor current 0-15 counter index
);

    // ----------------------------------------------------
    // Part 1: 0 to 15 Modulo-16 Counter & output_ready Pulse
    // ----------------------------------------------------
    reg [3:0] write_counter;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            write_counter <= 4'd0;
            output_ready  <= 1'b0;
        end else if (step_done_trigger) begin
            write_counter <= write_counter + 1'b1;
            
            // Trigger output_ready when counter completes 15 and wraps back to 0
            if (write_counter == 4'd15) begin
                output_ready <= 1'b1;
            end else begin
                output_ready <= 1'b0;
            end
        end else begin
            output_ready <= 1'b0; // Output ready is a 1-cycle pulse
        end
    end

    assign active_write_index = write_counter;

    // ----------------------------------------------------
    // Part 2: 1-to-16 One-Hot Decoder Logic
    // ----------------------------------------------------
    reg [15:0] write_enable;

    always @(*) begin
        write_enable = 16'd0;
        write_enable[write_counter] = 1'b1;
    end

    // ----------------------------------------------------
    // Part 3: 16 Registers of 12-bit Each
    // ----------------------------------------------------
    reg [11:0] reg_mem [15:0];
    integer k;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (k = 0; k < 16; k = k + 1) begin
                reg_mem[k] <= 12'd0;
            end
        end else if (step_done_trigger) begin
            for (k = 0; k < 16; k = k + 1) begin
                if (write_enable[k]) begin
                    reg_mem[k] <= sum_in;
                end
            end
        end
    end

    // ----------------------------------------------------
    // Flatten Array for Output Bus Mapping (16 * 12 bits)
    // ----------------------------------------------------
    genvar i;
    generate
        for (i = 0; i < 16; i = i + 1) begin : flatten_output
            assign stored_results_flat[i*12 +: 12] = reg_mem[i];
        end
    endgenerate

endmodule