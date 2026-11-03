
`timescale 1ns / 1ps

module tb_hidden_layer_rom_top;

    localparam int N = 784;

    logic clk = 0;
    always #5 clk = ~clk;

    logic reset_n = 0;
    logic start = 0;

    wire busy1, done1, busy2, done2;
    wire signed [63:0] sum10, sum11;
    wire signed [63:0] sum20, sum21;

    wire [9:0] addr1, addr2;
    wire read1, read2, valid1, valid2;

    logic signed [15:0] img1 [0:N-1];
    logic signed [15:0] img2 [0:N-1];
    logic signed [15:0] w0 [0:N-1];
    logic signed [15:0] w1 [0:N-1];

    logic signed [63:0] exp10 = 0;
    logic signed [63:0] exp11 = 0;
    logic signed [63:0] exp20 = 0;
    logic signed [63:0] exp21 = 0;

    logic signed [31:0] p10, p11, p20, p21;

    integer count1 = 0;
    integer count2 = 0;
    integer reads1 = 0;
    integer reads2 = 0;

    hidden_layer_rom_top #(
        .IMAGE_FILE(
          "data/test_vectors/q15_baseline/image_0001_q15.hex")
    ) dut1 (
        .clk(clk), .reset_n(reset_n), .start(start),
        .busy(busy1), .done(done1),
        .hidden_sum0_q30(sum10),
        .hidden_sum1_q30(sum11),
        .debug_mem_addr(addr1),
        .debug_read_en(read1),
        .debug_pe_valid(valid1)
    );

    hidden_layer_rom_top #(
        .IMAGE_FILE(
          "data/test_vectors/q15_baseline/image_0002_q15.hex")
    ) dut2 (
        .clk(clk), .reset_n(reset_n), .start(start),
        .busy(busy2), .done(done2),
        .hidden_sum0_q30(sum20),
        .hidden_sum1_q30(sum21),
        .debug_mem_addr(addr2),
        .debug_read_en(read2),
        .debug_pe_valid(valid2)
    );

    initial begin
        $readmemh(
          "data/test_vectors/q15_baseline/image_0001_q15.hex",
          img1);
        $readmemh(
          "data/test_vectors/q15_baseline/image_0002_q15.hex",
          img2);
        $readmemh(
          "data/test_vectors/q15_baseline/hidden_w0_q15.hex",
          w0);
        $readmemh(
          "data/test_vectors/q15_baseline/hidden_w1_q15.hex",
          w1);

        repeat (3) @(negedge clk);
        reset_n = 1;

        @(negedge clk);
        start = 1;

        @(negedge clk);
        start = 0;

        wait (done1 && done2);
        #1;

        if (count1 != N || count2 != N ||
            reads1 != N || reads2 != N)
            $fatal(1,
                "Cycle count mismatch: MACs=%0d,%0d reads=%0d,%0d",
                count1, count2, reads1, reads2);

        if (sum10 !== -64'sd1703180844 ||
            sum11 !==  64'sd1904845204)
            $fatal(1, "Image 1 C reference mismatch");

        if (sum20 !==  64'sd3102030662 ||
            sum21 !== -64'sd3472637570)
            $fatal(1, "Image 2 C reference mismatch");

        if (busy1 || busy2)
            $fatal(1, "Busy asserted during done");

        $display("PASS: Image 1 hidden sums match C");
        $display("PASS: Image 2 hidden sums match C");
        $display("PASS: 784 reads and 784 MACs per image");

        @(negedge clk);
        reset_n = 0;
        #1;

        if (sum10 !== 0 || sum11 !== 0 ||
            sum20 !== 0 || sum21 !== 0)
            $fatal(1, "Asynchronous reset failed");

        $display("PASS: asynchronous reset");
        $display("ALL HIDDEN-LAYER ROM TESTS PASSED");
        $finish;
    end

    // Check that the memory requests are sequential.
    always @(posedge clk) begin
        if (reset_n) begin
            if (read1) begin
                if (addr1 !== reads1[9:0])
                    $fatal(1, "Image 1 read address mismatch");
                reads1 = reads1 + 1;
            end

            if (read2) begin
                if (addr2 !== reads2[9:0])
                    $fatal(1, "Image 2 read address mismatch");
                reads2 = reads2 + 1;
            end
        end
    end

    // Image 1 independent running MAC reference.
    always @(posedge clk) begin
        if (reset_n && valid1) begin
            if (count1 >= N)
                $fatal(1, "Too many image 1 MAC cycles");

            p10 = $signed(img1[count1]) * $signed(w0[count1]);
            p11 = $signed(img1[count1]) * $signed(w1[count1]);

            exp10 = exp10 + {{32{p10[31]}}, p10};
            exp11 = exp11 + {{32{p11[31]}}, p11};

            count1 = count1 + 1;
            #1;

            if (sum10 !== exp10 || sum11 !== exp11)
                $fatal(1,
                    "Image 1 running MAC mismatch at sample %0d",
                    count1 - 1);
        end
    end

    // Image 2 independent running MAC reference.
    always @(posedge clk) begin
        if (reset_n && valid2) begin
            if (count2 >= N)
                $fatal(1, "Too many image 2 MAC cycles");

            p20 = $signed(img2[count2]) * $signed(w0[count2]);
            p21 = $signed(img2[count2]) * $signed(w1[count2]);

            exp20 = exp20 + {{32{p20[31]}}, p20};
            exp21 = exp21 + {{32{p21[31]}}, p21};

            count2 = count2 + 1;
            #1;

            if (sum20 !== exp20 || sum21 !== exp21)
                $fatal(1,
                    "Image 2 running MAC mismatch at sample %0d",
                    count2 - 1);
        end
    end

endmodule

