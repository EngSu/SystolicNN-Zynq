
`timescale 1ns / 1ps

module tb_two_pe_array;

    localparam int N = 784;

    logic clk = 0;
    always #5 clk = ~clk;

    logic reset_n = 0;
    logic clear = 0;
    logic input_valid = 0;

    logic signed [15:0] activation = 0;
    logic signed [15:0] weight0 = 0;
    logic signed [15:0] weight1 = 0;

    wire signed [63:0] acc0;
    wire signed [63:0] acc1;

    logic signed [15:0] image_mem [0:N-1];
    logic signed [15:0] w0_mem [0:N-1];
    logic signed [15:0] w1_mem [0:N-1];

    logic signed [63:0] expected0 = 0;
    logic signed [63:0] expected1 = 0;
    logic signed [31:0] prod0, prod1;

    string vec_dir;
    integer fd;

    two_pe_array dut (
        .clk(clk),
        .reset_n(reset_n),
        .clear(clear),
        .input_valid(input_valid),
        .activation_q15(activation),
        .weight0_q15(weight0),
        .weight1_q15(weight1),
        .acc0_q30(acc0),
        .acc1_q30(acc1)
    );

    task automatic check_file(input string path);
        begin
            fd = $fopen(path, "r");
            if (fd == 0)
                $fatal(1, "Missing file: %s", path);
            $fclose(fd);
        end
    endtask

    task automatic check_acc(input integer sample);
        begin
            if (acc0 !== expected0 || acc1 !== expected1) begin
                $display("Mismatch at sample %0d", sample);
                $display("PE0: RTL=%0d expected=%0d",
                         acc0, expected0);
                $display("PE1: RTL=%0d expected=%0d",
                         acc1, expected1);
                $fatal(1, "Two-PE verification failed");
            end
        end
    endtask

    task automatic run_image(
        input string filename,
        input logic signed [63:0] golden0,
        input logic signed [63:0] golden1
    );
        integer i;
        string path;
        begin
            path = {vec_dir, "/", filename};
            check_file(path);
            $readmemh(path, image_mem);

            // Clear both processing elements.
            // Assert input_valid too, to check clear priority.
            @(negedge clk);
            clear = 1;
            input_valid = 1;
            activation = 16'sd16384;
            weight0 = 16'sd16384;
            weight1 = -16'sd16384;

            @(posedge clk);
            #1;
            expected0 = 0;
            expected1 = 0;
            check_acc(-1);

            // Check both accumulators hold when invalid.
            @(negedge clk);
            clear = 0;
            input_valid = 0;

            @(posedge clk);
            #1;
            check_acc(-2);

            // Stream 784 activation/weight pairs.
            for (i = 0; i < N; i = i + 1) begin
                @(negedge clk);
                activation = image_mem[i];
                weight0 = w0_mem[i];
                weight1 = w1_mem[i];
                input_valid = 1;

                prod0 = $signed(image_mem[i])
                      * $signed(w0_mem[i]);
                prod1 = $signed(image_mem[i])
                      * $signed(w1_mem[i]);

                @(posedge clk);
                #1;

                expected0 = expected0
                    + {{32{prod0[31]}}, prod0};
                expected1 = expected1
                    + {{32{prod1[31]}}, prod1};

                check_acc(i);
            end

            @(negedge clk);
            input_valid = 0;

            @(posedge clk);
            #1;
            check_acc(N);

            // Independent golden values from C.
            if (acc0 !== golden0 || acc1 !== golden1)
                $fatal(1, "C-to-RTL final result mismatch");

            $display("PASS: %s PE0=%0d PE1=%0d",
                     filename, acc0, acc1);
        end
    endtask

    initial begin
        if (!$value$plusargs("VEC_DIR=%s", vec_dir))
            vec_dir = "data/test_vectors/q15_baseline";

        check_file({vec_dir, "/hidden_w0_q15.hex"});
        check_file({vec_dir, "/hidden_w1_q15.hex"});

        $readmemh({vec_dir, "/hidden_w0_q15.hex"}, w0_mem);
        $readmemh({vec_dir, "/hidden_w1_q15.hex"}, w1_mem);

        repeat (2) @(posedge clk);
        @(negedge clk);
        reset_n = 1;

        run_image(
            "image_0001_q15.hex",
            -64'sd1703180844,
             64'sd1904845204
        );

        run_image(
            "image_0002_q15.hex",
             64'sd3102030662,
            -64'sd3472637570
        );

        // Check asynchronous reset.
        @(negedge clk);
        reset_n = 0;
        #1;

        if (acc0 !== 64'sd0 || acc1 !== 64'sd0)
            $fatal(1, "Asynchronous reset failed");

        $display("PASS: asynchronous reset");
        $display("ALL TWO-PE TESTS PASSED");
        $finish;
    end

endmodule

