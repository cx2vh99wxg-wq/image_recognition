`timescale 1ns/1ps
module test_hmi_gear_feedback;
  reg clk=0,rst_n=0;always #1 clk=~clk;
  reg[3:0] status=0;reg slider_present=1;wire txd,valid;wire[7:0] data;
  reg autopilot_active=0;
  hmi_gear_feedback #(.CLK_FREQ(64000000),.UART_BPS(1000000),.REFRESH_CYCLES(20000),
      .HAS_AUTOPILOT(1'b1)) dut(
      .clk(clk),.rst_n(rst_n),.gear_async(status),.slider_present(slider_present),
      .autopilot_active_async(autopilot_active),.txd(txd));
  uart_rx #(.CLK_FREQ(64000000),.UART_BPS(1000000)) receiver(
      .clk(clk),.rst_n(rst_n),.uart_rxd(txd),.uart_rx_done(valid),.uart_rx_data(data));
  reg[7:0] captured[0:1023];integer count=0;
  always @(posedge clk)if(valid)begin captured[count]=data;count=count+1;end
  task frame(input integer first,input[7:0] gear);
    reg[191:0] expected;reg[215:0] number;integer k;
    begin
      expected={"page0.tGear.txt=",8'h22,gear,16'hb5b5,8'h22,24'hffffff};
      number={"page0.vaGearActual.val=",(gear=="?" ? 8'h30 : gear),24'hffffff};
      for(k=0;k<27;k=k+1)
        if(captured[first+24+k]!==number[215-k*8-:8])
          $fatal(1,"slider actual numeric byte %d",k);
      for(k=0;k<24;k=k+1)
        if(captured[first+k]!==expected[191-k*8-:8])
          $fatal(1,"HMI status frame byte %d: %h",k,captured[first+k]);
    end
  endtask
  task autopilot_frame(input integer first,input [7:0] flag);
    reg [79:0] expected; integer k;
    begin
      expected={"apAck=",flag,24'hffffff};
      for(k=0;k<10;k=k+1)
        if(captured[first+k]!==expected[79-k*8-:8])
          $fatal(1,"Autopilot acknowledgment byte %d: %h",k,captured[first+k]);
    end
  endtask
  task text_only(input integer first,input[7:0] gear);
    reg[191:0] expected;integer k;
    begin
      expected={"page0.tGear.txt=",8'h22,gear,16'hb5b5,8'h22,24'hffffff};
      for(k=0;k<24;k=k+1)
        if(captured[first+k]!==expected[191-k*8-:8])$fatal(1,"legacy TFT byte %d",k);
    end
  endtask
  initial begin
    #10;rst_n=1;wait(count==24);status=4'b1001;wait(count==61);frame(0,"?");
    autopilot_frame(51,"0");
    status=4'b1000;autopilot_active=1;wait(count==122);frame(61,"1");autopilot_frame(112,"1");
    status=4'b1001;wait(count==183);frame(122,"2");
    status=4'b1011;wait(count==244);frame(183,"3");
    status=4'b1010;wait(count==305);frame(244,"1");
    status=4'b1110;wait(count==366);frame(305,"2");
    status=4'b1100;wait(count==427);frame(366,"3");
    slider_present=0;autopilot_active=0;
    wait(count==495);text_only(427,"3");autopilot_frame(451,"0");
    text_only(461,"3");autopilot_frame(485,"0");
    #100;rst_n=0;#5;if(txd!==1'b1)$fatal(1,"reset must release UART TX high");
    $display("HMI feedback: GB2312 gear, optional slider, Autopilot acknowledgment and reset PASS");$finish;
  end
  initial begin #2000000;$fatal(1,"HMI UART feedback timeout count=%d",count);end
endmodule
