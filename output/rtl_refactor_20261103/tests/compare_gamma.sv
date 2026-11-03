`timescale 1ns/1ps
module compare_gamma;
    reg clk=0;
    always #5 clk=~clk;
    reg rst_n=0, href=0, vsync=0;
    reg [7:0] r=0,g=0,b=0,y=0;
    reg [9:0] score=0;
    wire [41:0] old_out,new_out;
    gamma_judge_reference old_dut(clk,rst_n,r,g,b,
        old_out[41:34],old_out[33:26],old_out[25:18],vsync,href,y,
        old_out[17],old_out[16],old_out[15:8],old_out[7:0],score);
    gamma_judge new_dut(clk,rst_n,r,g,b,
        new_out[41:34],new_out[33:26],new_out[25:18],vsync,href,y,
        new_out[17],new_out[16],new_out[15:8],new_out[7:0],score);
    integer cycles=0;
    always @(posedge clk) begin
        #1;
        cycles=cycles+1;
        if (old_out !== new_out)
            $fatal(1,"gamma cycle %0d score=%0d Y=%0d old=%h new=%h",cycles,score,y,old_out,new_out);
    end
    initial begin
        repeat(4) @(negedge clk);
        rst_n=1; href=1;
        for(int s=0;s<1024;s=s+1)
            for(int l=0;l<256;l=l+1) begin
                score=s[9:0]; y=l[7:0]+s[7:0];
                r=y^8'h19; g=y+8'h47; b=~y; vsync=score[0];
                @(negedge clk);
            end
        for(int i=0;i<1024;i=i+1) begin
            href=i[0]; score=10'd1023-i[9:0]; y=y+8'haf; rst_n=(i%53)!=0;
            @(negedge clk);
        end
        $display("PASS gamma %0d cycles, 1024 scores x 256 luminances plus blanking/reset",cycles);
        $finish;
    end
endmodule
