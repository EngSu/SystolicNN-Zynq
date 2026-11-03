
`timescale 1ns / 1ps

module hidden_layer_controller #(
    parameter integer N_INPUTS = 784,
    parameter integer ADDR_WIDTH = 10
)(
    input wire clk,
    input wire reset_n,
    input wire start,

    output reg [ADDR_WIDTH-1:0] mem_addr,
    output reg mem_read_en,
    output reg pe_clear,
    output reg pe_valid,

    output wire busy,
    output wire done
);

    localparam [2:0]
        IDLE  = 3'd0,
        CLEAR = 3'd1,
        FETCH = 3'd2,
        RUN   = 3'd3,
        DONE  = 3'd4;

    reg [2:0] state;
    reg [ADDR_WIDTH-1:0] index;

    assign busy = (state == CLEAR) ||
                  (state == FETCH) ||
                  (state == RUN);

    assign done = (state == DONE);

    // Combinational control outputs
    always @(*) begin

        mem_addr = {ADDR_WIDTH{1'b0}};
        mem_read_en = 1'b0;
        pe_clear = 1'b0;
        pe_valid = 1'b0;

        case (state)

            CLEAR: begin
                pe_clear = 1'b1;
            end

            FETCH: begin
                mem_addr = {ADDR_WIDTH{1'b0}};
                mem_read_en = 1'b1;
            end

            RUN: begin
                pe_valid = 1'b1;

                // Prefetch the next sample while the
                // current sample is accumulated.
                if (index < N_INPUTS - 1) begin
                    mem_addr = index + 1'b1;
                    mem_read_en = 1'b1;
                end
            end

            default: begin
            end

        endcase
    end

    // Sequential state transitions
    always @(posedge clk or negedge reset_n) begin

        if (!reset_n) begin
            state <= IDLE;
            index <= {ADDR_WIDTH{1'b0}};
        end
        else begin

            case (state)

                IDLE: begin
                    if (start) begin
                        index <= {ADDR_WIDTH{1'b0}};
                        state <= CLEAR;
                    end
                end

                CLEAR: begin
                    state <= FETCH;
                end

                FETCH: begin
                    state <= RUN;
                end

                RUN: begin
                    if (index == N_INPUTS - 1) begin
                        state <= DONE;
                    end
                    else begin
                        index <= index + 1'b1;
                    end
                end

                DONE: begin
                    state <= IDLE;
                end

                default: begin
                    state <= IDLE;
                end

            endcase
        end
    end

endmodule

