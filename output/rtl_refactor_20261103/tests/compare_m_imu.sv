`timescale 1ns/1ps

// Model the PLL output clock only. Both buses use the actual M-side transport.
module clk_fspi(input wire clkin1,output wire clkout0,lock);
    assign clkout0=clkin1;
    assign lock=1'b1;
endmodule

module compare_m_imu;
    reg sys=0;
    always #1.25 sys=~sys;
    reg rst_n=0, sclk=1, cs=1, master=1;
    reg [3:0] out=0, view=4'hc;
    reg [511:0] snapshot={16{32'hcafeba13}};
    localparam [511:0] FIRST_SNAPSHOT={16{32'hcafeba13}};
    tri [3:0] old_bus,new_bus;
    assign old_bus=master ? out : 4'bz;
    assign new_bus=master ? out : 4'bz;
    wire [55:0] old_regs,new_regs;
    fspi_slave_top_m_imu_reference old_dut(snapshot,sys,rst_n,sclk,cs,old_bus,
        view,1'b1,6'b100001,16'ha56b,old_regs[55:48],old_regs[47:40],old_regs[39:32],
        old_regs[31:24],old_regs[23:16],old_regs[15:8],old_regs[7:0]);
    fspi_slave_top_m_imu new_dut(snapshot,sys,rst_n,sclk,cs,new_bus,
        view,1'b1,6'b100001,16'ha56b,new_regs[55:48],new_regs[47:40],new_regs[39:32],
        new_regs[31:24],new_regs[23:16],new_regs[15:8],new_regs[7:0]);
    integer edges=0;
    always @(negedge sys) begin
        #0.2;
        edges=edges+1;
        if (old_regs !== new_regs || old_bus !== new_bus)
            $fatal(1,"M FSPI mismatch at wr edge %0d",edges);
    end
    task write_nibble(input [3:0] n);
        sclk=0; out=n; #20.833; sclk=1; #20.833;
    endtask
    task write_byte(input [7:0] b);
        write_nibble(b[7:4]); write_nibble(b[3:0]);
    endtask
    task read_nibble(output [3:0] n);
        sclk=0; #20.833; sclk=1; #1;
        if (old_bus !== new_bus) $fatal(1,"M sample mismatch");
        n=new_bus; #19.833;
    endtask
    task read_byte(output [7:0] b);
        reg [3:0] hi,lo;
        read_nibble(hi); read_nibble(lo); b={hi,lo};
    endtask
    task begin_transaction;
        cs=1; master=1; #100; cs=0;
    endtask
    task end_transaction;
        cs=1; master=1; #100;
    endtask
    reg [7:0] value;
    initial begin
        #100; rst_n=1; #100;
        for(int a=0;a<9;a=a+1) begin
            begin_transaction();
            write_byte(0); write_byte(0); write_byte(a[7:0]); write_byte(a[7:0]^8'h99);
            end_transaction();
        end
        if (new_regs !== 56'h99989b9a9d9c9f) $fatal(1,"M register bank mismatch %h",new_regs);
        begin_transaction();
        // SMDT inserts 00 before the ARM payload 4D 01 A5.
        write_byte(0); write_byte(8'h4d); write_byte(1); write_byte(8'ha5);
        end_transaction();
        if (new_regs !== 56'h99989b9a9d9c9f) $fatal(1,"IMU command modified motion bank");
        begin_transaction(); write_byte(8'h80); master=0;
        for(int i=0;i<72;i=i+1) begin
            read_byte(value);
            if(i==0 && value!==8'hcc) $fatal(1,"legacy HMI");
            if(i==1 && value!==8'ha7) $fatal(1,"gear header");
            if(i==4 && value!==8'hd7) $fatal(1,"control header");
            if(i==8) snapshot=0;
            if(i>=8 && value!==FIRST_SNAPSHOT[511-(i-8)*8 -: 8])
                $fatal(1,"IMU snapshot byte %0d: %h",i,value);
        end
        end_transaction();
        begin_transaction(); write_byte(8'h81); write_byte(0); write_byte(5);
        master=0; read_byte(value); end_transaction();
        if(value!==8'ha1) $fatal(1,"M AUTO status %h",value);
        begin_transaction(); write_byte(0); write_byte(0); end_transaction();
        if (new_regs !== 56'h99989b9a9d9c9f) $fatal(1,"interrupted transaction committed");
        $display("PASS M FSPI %0d wr edges and 72-byte snapshot",edges);
        $finish;
    end
endmodule
