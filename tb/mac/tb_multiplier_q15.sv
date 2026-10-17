
`timescale 1ns / 1ps

module tb_multiplier_q15;

    logic signed [15:0] a_q15;
    logic signed [15:0] b_q15;
    wire  signed [31:0] product_q30;

    integer errors = 0;

    // Device Under Test (DUT)
    multiplier_q15 dut (
        .a_q15(a_q15),
        .b_q15(b_q15),
        .product_q30(product_q30)
    );

    // Check one multiplication
    task automatic check_product(
        input logic signed [15:0] a,
        input logic signed [15:0] b,
        input logic signed [31:0] expected
    );
    begin
        a_q15 = a;
        b_q15 = b;

        #1; // Allow combinational output to settle

        if (product_q30 !== expected) begin
            $display("FAIL: A=%0d B=%0d Got=%0d Expected=%0d",
                     a, b, product_q30, expected);
            errors = errors + 1;
        end
        else begin
            $display("PASS: A=%0d B=%0d Product=%0d",
                     a, b, product_q30);
        end
    end
    endtask

    initial begin

        $display("Starting Q1.15 multiplier verification");

        check_product(16'sd16384,  16'sd16384,  32'sd268435456);
        check_product(16'sd16384, -16'sd16384, -32'sd268435456);
        check_product(-16'sd16384, -16'sd16384, 32'sd268435456);

        check_product(16'sd0, 16'sd16384, 32'sd0);

        // Signed boundary tests
        check_product(16'sh8000, 16'sh8000, 32'sd1073741824);
        check_product(16'sh7fff, 16'sh7fff, 32'sd1073676289);
        check_product(16'sh8000, 16'sh7fff, -32'sd1073709056);

        if (errors == 0)
            $display("ALL MULTIPLIER TESTS PASSED");
        else
            $fatal(1, "MULTIPLIER TEST FAILED: %0d errors", errors);

        $finish;
    end

endmodule
