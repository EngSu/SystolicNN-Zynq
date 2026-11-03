
`timescale 1ns / 1ps

// Two parallel output-stationary PEs.
// Both receive the same activation.
// Each receives an independent streamed weight.

module two_pe_array (
    input  wire               clk,
    input  wire               reset_n,
    input  wire               clear,
    input  wire               input_valid,
    input  wire signed [15:0] activation_q15,
    input  wire signed [15:0] weight0_q15,
    input  wire signed [15:0] weight1_q15,
    output wire signed [63:0] acc0_q30,
    output wire signed [63:0] acc1_q30
);

    pe_output_stationary pe0 (
        .clk(clk),
        .reset_n(reset_n),
        .clear(clear),
        .input_valid(input_valid),
        .activation_q15(activation_q15),
        .weight_q15(weight0_q15),
        .partial_sum_q30(acc0_q30)
    );

    pe_output_stationary pe1 (
        .clk(clk),
        .reset_n(reset_n),
        .clear(clear),
        .input_valid(input_valid),
        .activation_q15(activation_q15),
        .weight_q15(weight1_q15),
        .partial_sum_q30(acc1_q30)
    );

endmodule

