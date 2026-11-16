`timescale 1ns/1ps
module test_hmi_gear_feedback;
  reg clk=0,rst_n=0;always #1 clk=~clk;
  reg[3:0] status=0;reg slider_present=1;wire txd,valid;wire[7:0] data;
  hmi_gear_feedback #(.CLK_FREQ(64000000),.UART_BPS(1000000),.REFRESH_CYCLES(20000)) dut(
      .clk(clk),.rst_n(rst_n),.gear_async(status),.slider_present(slider_present),.txd(txd));
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
  task text_only(input integer first,input[7:0] gear);
    reg[191:0] expected;integer k;
    begin
      expected={"page0.tGear.txt=",8'h22,gear,16'hb5b5,8'h22,24'hffffff};
      for(k=0;k<24;k=k+1)
        if(captured[first+k]!==expected[191-k*8-:8])$fatal(1,"legacy TFT byte %d",k);
    end
  endtask
  initial begin
    #10;rst_n=1;wait(count==24);status=4'b1001;wait(count==51);frame(0,"?");
    status=4'b1000;wait(count==102);frame(51,"1");
    status=4'b1001;wait(count==153);frame(102,"2");
    status=4'b1011;wait(count==204);frame(153,"3");
    status=4'b1010;wait(count==255);frame(204,"1");
    status=4'b1110;wait(count==306);frame(255,"2");
    status=4'b1100;wait(count==357);frame(306,"3");
    slider_present=0;wait(count==405);text_only(357,"3");text_only(381,"3");
    #100;rst_n=0;#5;if(txd!==1'b1)$fatal(1,"reset must release UART TX high");
    $display("HMI gear feedback: UART decoding, GB2312 1/2/3 gear, unknown, Gray rollover, reset PASS");$finish;
  end
  initial begin #2000000;$fatal(1,"HMI UART feedback timeout count=%d",count);end
endmodule
