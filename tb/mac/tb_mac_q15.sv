
`timescale 1ns / 1ps

module tb_mac_q15;

    localparam int N = 784;

    logic clk = 1'b0;
    always #5 clk = ~clk;

    logic reset_n;
    logic clear;
    logic enable;

    logic signed [15:0] activation;
    logic signed [15:0] weight0;
    logic signed [15:0] weight1;

    wire signed [63:0] acc0;
    wire signed [63:0] acc1;

    logic signed [15:0] image_mem [0:N-1];
    logic signed [15:0] w0_mem [0:N-1];
    logic signed [15:0] w1_mem [0:N-1];

    logic signed [63:0] expected0;
    logic signed [63:0] expected1;
    logic signed [31:0] product0;
    logic signed [31:0] product1;

    string vec_dir;
    integer fd;

    mac_q15 pe0 (
        .clk(clk),
        .reset_n(reset_n),
        .clear(clear),
        .enable(enable),
        .a_q15(activation),
        .b_q15(weight0),
        .acc_q30(acc0)
    );

    mac_q15 pe1 (
        .clk(clk),
        .reset_n(reset_n),
        .clear(clear),
        .enable(enable),
        .a_q15(activation),
        .b_q15(weight1),
        .acc_q30(acc1)
    );

    task automatic require_file(input string path);
        begin
            fd = $fopen(path, "r");
            if (fd == 0)
                $fatal(1, "Cannot open vector file: %s", path);
            $fclose(fd);
        end
    endtask

    task automatic check_acc(input integer cycle);
        begin
            if (acc0 !== expected0 || acc1 !== expected1) begin
                $display("FAIL at sample %0d", cycle);
                $display("PE0: got=%0d expected=%0d",
                         acc0, expected0);
                $display("PE1: got=%0d expected=%0d",
                         acc1, expected1);
                $fatal(1, "MAC accumulator mismatch");
            end
        end
    endtask

    task automatic run_image(
        input string filename,
        input logic signed [63:0] final0,
        input logic signed [63:0] final1
    );
        integer i;
        string path;
        begin
            path = {vec_dir, "/", filename};
            require_file(path);
            $readmemh(path, image_mem);

            // Clear both accumulators
            @(negedge clk);
            enable = 0;
            clear = 1;

            @(posedge clk);
            #1;
            expected0 = 0;
            expected1 = 0;
            check_acc(-1);

            // Disable accumulation and verify hold
            @(negedge clk);
            clear = 0;
            enable = 0;
            activation = 16'sd1000;
            weight0 = 16'sd1000;
            weight1 = -16'sd1000;

            @(posedge clk);
            #1;
            check_acc(-1);

            // Stream all 784 MNIST input/weight pairs
            for (i = 0; i < N; i = i + 1) begin
                @(negedge clk);

                activation = image_mem[i];
                weight0 = w0_mem[i];
                weight1 = w1_mem[i];
                enable = 1;

                product0 = $signed(image_mem[i])
                         * $signed(w0_mem[i]);

                product1 = $signed(image_mem[i])
                         * $signed(w1_mem[i]);

                @(posedge clk);
                #1;

                expected0 = expected0 +
                    {{32{product0[31]}}, product0};

                expected1 = expected1 +
                    {{32{product1[31]}}, product1};

                check_acc(i);
            end

            // Verify that accumulators hold their results
            @(negedge clk);
            enable = 0;

            @(posedge clk);
            #1;
            check_acc(N);

            // Compare with independent C reference values
            if (acc0 !== final0 || acc1 !== final1) begin
                $display("C reference PE0=%0d RTL=%0d",
                         final0, acc0);
                $display("C reference PE1=%0d RTL=%0d",
                         final1, acc1);
                $fatal(1, "Final C-to-RTL comparison failed");
            end

            $display(
                "PASS: %s PE0=%0d PE1=%0d",
                filename, acc0, acc1
            );
        end
    endtask

    initial begin
        reset_n = 0;
        clear = 0;
        enable = 0;
        activation = 0;
        weight0 = 0;
        weight1 = 0;

        if (!$value$plusargs("VEC_DIR=%s", vec_dir))
            vec_dir = "data/test_vectors/q15_baseline";

        require_file({vec_dir, "/hidden_w0_q15.hex"});
        require_file({vec_dir, "/hidden_w1_q15.hex"});

        $readmemh(
            {vec_dir, "/hidden_w0_q15.hex"}, w0_mem);
        $readmemh(
            {vec_dir, "/hidden_w1_q15.hex"}, w1_mem);

        repeat (2) @(posedge clk);

        @(negedge clk);
        reset_n = 1;

        // Golden values from the verified C CSV
        run_image(
            "image_0001_q15.hex",
            -64'sd1703180844,
             64'sd1904845204
        );

        // Image 2 includes accumulator values
        // outside the signed 32-bit range.
        run_image(
            "image_0002_q15.hex",
             64'sd3102030662,
            -64'sd3472637570
        );

        // Verify asynchronous reset
        @(negedge clk);
        reset_n = 0;
        #1;

        if (acc0 !== 64'sd0 || acc1 !== 64'sd0)
            $fatal(1, "Asynchronous reset failed");

        $display("PASS: asynchronous reset");
        $display("ALL MAC TESTS PASSED");
        $finish;
    end

endmodule
ø

