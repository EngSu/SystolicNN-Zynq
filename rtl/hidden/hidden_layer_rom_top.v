
`timescale 1ns / 1ps

module hidden_layer_rom_top #(
    parameter IMAGE_FILE =
        "data/test_vectors/q15_baseline/image_0001_q15.hex",
    parameter W0_FILE =
        "data/test_vectors/q15_baseline/hidden_w0_q15.hex",
    parameter W1_FILE =
        "data/test_vectors/q15_baseline/hidden_w1_q15.hex"
)(
    input  wire clk,
    input  wire reset_n,
    input  wire start,

    output wire busy,
    output wire done,

    output wire signed [63:0] hidden_sum0_q30,
    output wire signed [63:0] hidden_sum1_q30,

    // Debug signals used by the testbench.
    output wire [9:0] debug_mem_addr,
    output wire debug_read_en,
    output wire debug_pe_valid
);

    wire [9:0] mem_addr;
    wire mem_read_en;
    wire pe_clear;
    wire pe_valid;

    wire signed [15:0] activation;
    wire signed [15:0] weight0;
    wire signed [15:0] weight1;

    hidden_layer_controller #(
        .N_INPUTS(784),
        .ADDR_WIDTH(10)
    ) u_controller (
        .clk(clk),
        .reset_n(reset_n),
        .start(start),
        .mem_addr(mem_addr),
        .mem_read_en(mem_read_en),
        .pe_clear(pe_clear),
        .pe_valid(pe_valid),
        .busy(busy),
        .done(done)
    );

    rom_q15_sync #(
        .DEPTH(784),
        .ADDR_WIDTH(10),
        .INIT_FILE(IMAGE_FILE)
    ) u_image_rom (
        .clk(clk),
        .read_en(mem_read_en),
        .addr(mem_addr),
        .data_q15(activation)
    );

    rom_q15_sync #(
        .DEPTH(784),
        .ADDR_WIDTH(10),
        .INIT_FILE(W0_FILE)
    ) u_weight0_rom (
        .clk(clk),
        .read_en(mem_read_en),
        .addr(mem_addr),
        .data_q15(weight0)
    );

    rom_q15_sync #(
        .DEPTH(784),
        .ADDR_WIDTH(10),
        .INIT_FILE(W1_FILE)
    ) u_weight1_rom (
        .clk(clk),
        .read_en(mem_read_en),
        .addr(mem_addr),
        .data_q15(weight1)
    );

    two_pe_array u_pe_array (
        .clk(clk),
        .reset_n(reset_n),
        .clear(pe_clear),
        .input_valid(pe_valid),
        .activation_q15(activation),
        .weight0_q15(weight0),
        .weight1_q15(weight1),
        .acc0_q30(hidden_sum0_q30),
        .acc1_q30(hidden_sum1_q30)
    );

    assign debug_mem_addr = mem_addr;
    assign debug_read_en = mem_read_en;
    assign debug_pe_valid = pe_valid;

endmodule

