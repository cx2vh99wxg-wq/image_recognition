`timescale 1ns/1ps
module compare_spi;
    reg clk = 0;
    always #5 clk = ~clk;
    reg rst_n = 0, cs = 1, valid = 0, done = 0;
    reg [1:0] cmd = 0;
    reg [7:0] data = 0;
    reg [3:0] view = 0;
    wire [63:0] old_out, new_out;
    spi_ram_crtl_reference old_dut(clk,rst_n,cs,cmd,done,data,valid,view,
        old_out[63:56],old_out[55:48],old_out[47:40],old_out[39:32],
        old_out[31:24],old_out[23:16],old_out[15:8],old_out[7:0]);
    spi_ram_crtl new_dut(clk,rst_n,cs,cmd,done,data,valid,view,
        new_out[63:56],new_out[55:48],new_out[47:40],new_out[39:32],
        new_out[31:24],new_out[23:16],new_out[15:8],new_out[7:0]);
    integer cycles = 0;
    always @(negedge clk) begin
        #1;
        cycles = cycles + 1;
        if (old_out !== new_out ||
            {old_dut.state,old_dut.ram_addr,old_dut.ram_wr_en,old_dut.byte_cnt}
            !== {new_dut.state,new_dut.ram_addr,new_dut.ram_wr_en,new_dut.byte_cnt} ||
            {old_dut.addr_high,old_dut.read_addr_high} !== {new_dut.addr_high,new_dut.read_addr_high})
            $fatal(1,"SPI mismatch cycle %0d old=%h new=%h",cycles,old_out,new_out);
    end
    task tick(input integer n);
        repeat(n) @(posedge clk);
    endtask
    task send_byte(input [7:0] value);
        data = value; valid = 1; tick(2);
        valid = 0; tick(2);
    endtask
    task transaction(input [1:0] direction,input [15:0] address,input integer length);
        cs = 0; cmd = direction; tick(3);
        if (length > 0) send_byte(address[15:8]);
        if (length > 1) send_byte(address[7:0]);
        if (length > 2) begin
            if (direction == 1) send_byte(8'ha5 ^ address[7:0]);
            else begin done = 1; tick(3); done = 0; tick(3); end
        end
        cs = 1; tick(4);
    endtask
    reg [31:0] prng = 32'h12110077;
    initial begin
        tick(4); rst_n = 1; tick(2);
        for (int a=0; a<12; a=a+1) begin
            view = a[3:0];
            for (int n=0; n<=3; n=n+1) begin
                transaction(1,a[15:0],n);
                transaction(2,a[15:0],n);
            end
        end
        transaction(1,16'hffff,3);
        transaction(2,16'h0100,3);
        // Exercise every address against the legacy decoder after predecode was added.
        for (int a=0; a<65536; a=a+1) begin
            view=a[3:0];
            transaction(1,a[15:0],3);
            transaction(2,a[15:0],3);
        end
        for(int i=0;i<20000;i=i+1) begin
            prng = {prng[30:0],prng[31]^prng[21]^prng[1]^prng[0]};
            {cs,cmd,done,data,valid,view} = prng[16:0];
            rst_n = prng[30:25] != 0;
            tick(1);
        end
        tick(3);
        $display("PASS SPI %0d falling edges",cycles);
        $finish;
    end
endmodule
