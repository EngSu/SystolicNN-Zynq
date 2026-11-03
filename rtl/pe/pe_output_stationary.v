
`timescale 1ns / 1ps

// Output-stationary Processing Element
// One activation and one weight per valid cycle.
// The partial sum stays in the local accumulator.

module pe_output_stationary (
    input  wire               clk,
    input  wire               reset_n,
    input  wire               clear,
    input  wire               input_valid,
    input  wire signed [15:0] activation_q15,
    input  wire signed [15:0] weight_q15,
    output wire signed [63:0] partial_sum_q30
);

    mac_q15 u_mac (
        .clk(clk),
        .reset_n(reset_n),
        .clear(clear),
        .enable(input_valid),
        .a_q15(activation_q15),
        .b_q15(weight_q15),
        .acc_q30(partial_sum_q30)
    );

endmodule

