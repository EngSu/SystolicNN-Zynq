
`timescale 1ns / 1ps

module rom_q15_sync #(
    parameter integer DEPTH = 784,
    parameter integer ADDR_WIDTH = 10,
    parameter INIT_FILE = ""
)(
    input  wire clk,
    input  wire read_en,
    input  wire [ADDR_WIDTH-1:0] addr,
    output reg signed [15:0] data_q15
);

    // Request block RAM implementation in Vivado.
    (* ram_style = "block" *)
    reg signed [15:0] mem [0:DEPTH-1];

    initial begin
        if (INIT_FILE != "")
            $readmemh(INIT_FILE, mem);
    end

    // Synchronous read:
    // data_q15 updates after the requesting clock edge.
    always @(posedge clk) begin
        if (read_en)
            data_q15 <= mem[addr];
    end

endmodule

