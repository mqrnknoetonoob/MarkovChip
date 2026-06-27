module counter_12bit (
    input wire clk,           // ক্লক সিগন্যাল
    input wire rst_n,         // রিসেট (Active low)
    input wire on_switch,     // সুইচ অন থাকলে কাউন্ট হবে
    input wire count_en,      // ইনপুট ১ হলে কাউন্ট বাড়বে
    output reg [11:0] count   // ১২-বিট কাউন্টার আউটপুট
);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            count <= 12'b0;
        end else if (!on_switch) begin
            // সুইচ অফ থাকলে কাউন্টার অটোমেটিক ০ হয়ে যাবে
            count <= 12'b0;
        end else if (count_en) begin
            // সুইচ অন এবং ইনপুট ১ হলে কাউন্ট বাড়বে
            count <= count + 1'b1;
        end
        // যদি count_en ০ হয়, তবে কাউন্ট আগের মতোই থাকবে
    end

endmodule