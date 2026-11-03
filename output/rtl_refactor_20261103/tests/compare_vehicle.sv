`timescale 1ns/1ps
module compare_vehicle;
    reg clk=0;
    always #5 clk=~clk;
    reg rst_n=0, manual=0, physical=0, toggle=0, slider=0;
    reg [1:0] target=1;
    reg [7:0] sync=0;
    wire [5:0] old_speed,new_speed;
    wire old_uart,new_uart;
    wire key_n=!(physical||old_speed[0]);
    remote_speed_controller_reference #(.DEBOUNCE_CYCLES(3),.PULSE_CYCLES(12),.GAP_CYCLES(8))
        old_dut(clk,rst_n,key_n,manual,toggle,target,sync,
                old_speed[5:3],old_speed[2],old_speed[1],old_speed[0]);
    remote_speed_controller #(.DEBOUNCE_CYCLES(3),.PULSE_CYCLES(12),.GAP_CYCLES(8))
        new_dut(clk,rst_n,key_n,manual,toggle,target,sync,
                new_speed[5:3],new_speed[2],new_speed[1],new_speed[0]);
    hmi_gear_feedback_reference #(.CLK_FREQ(64000000),.UART_BPS(1000000),.REFRESH_CYCLES(18))
        old_hmi(clk,rst_n,{old_speed[2],old_speed[5:3]},slider,old_uart);
    hmi_gear_feedback #(.CLK_FREQ(64000000),.UART_BPS(1000000),.REFRESH_CYCLES(18))
        new_hmi(clk,rst_n,{new_speed[2],new_speed[5:3]},slider,new_uart);
    integer cycles=0, bytes_sent=0, presses=0, collisions=0;
    always @(posedge clk) begin
        if (rst_n && new_dut.calibrate && new_dut.target_event) collisions=collisions+1;
        #1;
        cycles=cycles+1;
        if ({old_speed,old_uart,old_hmi.tx_enable,old_hmi.tx_data}
            !== {new_speed,new_uart,new_hmi.tx_enable,new_hmi.tx_data})
            $fatal(1,"vehicle cycle=%0d old=%h new=%h",cycles,old_speed,new_speed);
        if(new_hmi.tx_enable) bytes_sent=bytes_sent+1;
        if(new_speed[0]) presses=presses+1;
    end
    task tick(input integer n);
        repeat(n) @(negedge clk);
    endtask
    reg [31:0] prng=32'h8cf197a3;
    initial begin
        for(int n=0;n<64;n=n+1)
            for(int v=0;v<256;v=v+1)
                if(old_hmi.frame_byte(n[5:0],v[7:0]) !== new_hmi.frame_byte(n[5:0],v[7:0]))
                    $fatal(1,"HMI byte n=%0d gear=%0d",n,v);
        tick(5); rst_n=1; tick(20); slider=1;
        toggle=1; target=1; tick(35);
        if(presses!=0) $fatal(1,"same gear drove contact");
        toggle=0; target=3; tick(65);
        manual=1; tick(110); manual=0; tick(20);
        rst_n=0; toggle=0; tick(5); rst_n=1; tick(20);
        toggle=1; target=2; tick(1); sync=8'ha5; tick(18); sync=0;
        tick(40000); slider=0; tick(40000);
        for(int i=0;i<8000;i=i+1) begin
            prng={prng[30:0],prng[31]^prng[21]^prng[1]^prng[0]};
            if(prng[3:0]==0) begin toggle=~toggle; target=prng[8:7]; end
            if(prng[5:0]==1) manual=~manual;
            if(prng[5:0]==2) physical=~physical;
            if(prng[4:0]==3) sync=8'ha5;
            else if(prng[4:0]==4) sync=0;
            rst_n=prng[31:23]!=0;
            tick(1);
        end
        if(bytes_sent<75 || presses==0 || collisions==0)
            $fatal(1,"vehicle coverage missing: bytes=%0d drive cycles=%0d collisions=%0d",
                   bytes_sent,presses,collisions);
        $display("PASS vehicle %0d cycles, %0d UART bytes, %0d A5/target collisions",cycles,bytes_sent,collisions);
        $finish;
    end
endmodule
