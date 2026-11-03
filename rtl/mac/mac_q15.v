
`timescale 1ns / 1ps

// SystolicNN-Zynq
// Signed Q1.15 Multiply-Accumulate Unit
//
// Inputs:       signed 16-bit Q1.15
// Product:      signed 32-bit, 30 fractional bits
// Accumulator:  signed 64-bit, 30 fractional bits
//
// Priority: reset > clear > enable > hold

module mac_q15 (
    input  wire               clk,
    input  wire               reset_n,
    input  wire               clear,
    input  wire               enable,

    input  wire signed [15:0] a_q15,
    input  wire signed [15:0] b_q15,

    output reg  signed [63:0] acc_q30
);

    wire signed [31:0] product_q30;

    wire signed [63:0] product_extended =
        {{32{product_q30[31]}}, product_q30};

    multiplier_q15 u_multiplier (
        .a_q15       (a_q15),
        .b_q15       (b_q15),
        .product_q30 (product_q30)
    );

    always @(posedge clk or negedge reset_n) begin
        if (!reset_n)
            acc_q30 <= 64'sd0;
        else if (clear)
            acc_q30 <= 64'sd0;
        else if (enable)
            acc_q30 <= acc_q30 + product_extended;
    end

endmodule

