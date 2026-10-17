`timescale 1ns / 1ps

// SystolicNN-Zynq
// Module: multiplier_q15
// Description: Signed Q1.15 x Q1.15 multiplier
// Product: 32-bit signed with 30 fractional bits

module multiplier_q15 (
    input  wire signed [15:0] a_q15,
    input  wire signed [15:0] b_q15,
    output wire signed [31:0] product_q30
);

    assign product_q30 = a_q15 * b_q15;

endmodule