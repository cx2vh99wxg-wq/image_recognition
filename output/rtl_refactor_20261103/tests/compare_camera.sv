`timescale 1ns/1ps
module compare_camera;
    reg clk = 0;
    always #5 clk = ~clk;
    reg rst_n = 0, vsync = 0, href = 0;
    reg [7:0] data = 0;
    wire [38:0] old_out, new_out;
    wire [38:0] old_full, new_full;
    cmos_capture_data_reference #(.H_PIXEL(5),.V_PIXEL(3)) old_dut(
        rst_n,clk,vsync,href,data,old_out[38:29],old_out[28:19],
        old_out[18],old_out[17],old_out[16],old_out[15:0]);
    cmos_capture_data #(.H_PIXEL(5),.V_PIXEL(3)) new_dut(
        rst_n,clk,vsync,href,data,new_out[38:29],new_out[28:19],
        new_out[18],new_out[17],new_out[16],new_out[15:0]);
    cmos_capture_data_reference old_default(
        rst_n,clk,vsync,href,data,old_full[38:29],old_full[28:19],
        old_full[18],old_full[17],old_full[16],old_full[15:0]);
    cmos_capture_data new_default(
        rst_n,clk,vsync,href,data,new_full[38:29],new_full[28:19],
        new_full[18],new_full[17],new_full[16],new_full[15:0]);
    integer cycles=0, blank_valid=0, wraps=0, full_wraps=0;
    reg [19:0] previous_xy=0;
    reg [19:0] previous_full_xy=0;
    always @(posedge clk) begin
        #1;
        cycles=cycles+1;
        if (new_out !== old_out) $fatal(1,"camera cycle=%0d old=%h new=%h",cycles,old_out,new_out);
        if (new_full !== old_full) $fatal(1,"default camera cycle=%0d old=%h new=%h",cycles,old_full,new_full);
        if (!href && new_out[16]) blank_valid=blank_valid+1;
        if (rst_n && previous_xy=={10'd4,10'd2} && new_out[38:19]==0) wraps=wraps+1;
        previous_xy=new_out[38:19];
        if (rst_n && previous_full_xy=={10'd639,10'd479} && new_full[38:19]==0)
            full_wraps=full_wraps+1;
        previous_full_xy=new_full[38:19];
    end
    task tick(input integer n);
        repeat(n) @(negedge clk);
    endtask
    task frame;
        vsync=1; tick(3); vsync=0; tick(3);
    endtask
    initial begin
        tick(3); rst_n=1;
        repeat(11) frame();
        for(int row=0;row<80;row=row+1) begin
            href=1;
            for(int i=0;i<row%13+1;i=i+1) begin
                data=data+8'h5b; tick(1);
            end
            if(row==40) begin
                rst_n=0; tick(2); rst_n=1;
                href=0; repeat(11) frame();
            end
            href=0; tick(3);
            if(row%7==0) frame();
        end
        rst_n=0; tick(2); rst_n=1; repeat(11) frame();
        for(int row=0;row<480;row=row+1) begin
            href=1;
            for(int i=0;i<1280;i=i+1) begin
                data=data+8'h5b; tick(1);
            end
            href=0; tick(3);
        end
        if(blank_valid==0 || wraps==0 || full_wraps!=1) $fatal(1,"camera coverage missing");
        $display("PASS camera %0d cycles, blanking valid %0d, wraps %0d, default wraps %0d",
                 cycles,blank_valid,wraps,full_wraps);
        $finish;
    end
endmodule
