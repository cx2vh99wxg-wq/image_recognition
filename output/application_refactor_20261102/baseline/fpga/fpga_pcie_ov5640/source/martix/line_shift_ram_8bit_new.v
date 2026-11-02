module line_shift_ram_8bit_new (
    input clock,
    input clken,
    input rst_n,
    input [7:0] shiftin,
    output [7:0] taps0x,
    output [7:0] taps1x
);

  //reg define
  reg [2:0] clken_dly;
  reg [9:0] ram_rd_addr;
  reg [9:0] ram_rd_addr_d0;
  reg [9:0] ram_rd_addr_d1;
  reg [7:0] shiftin_d0;
  reg [7:0] shiftin_d1;
  reg [7:0] shiftin_d2;
  reg [7:0] taps0x_d0;

  //*****************************************************
  //**                    main code
  //*****************************************************

  //����������ʱ��ram��ַ�ۼ�
  always @(posedge clock) begin
    if (!rst_n) begin
      ram_rd_addr <= 0;
    end else if (ram_rd_addr == 10'd640) begin
      ram_rd_addr <= 0;
    end else if (clken) ram_rd_addr <= ram_rd_addr + 1;
      else ram_rd_addr <= ram_rd_addr;
    end

  //ʱ��ʹ���ź��ӳ�����
  always @(posedge clock) begin
    clken_dly <= {clken_dly[1:0], clken};
  end


  //��ram��ַ�ӳٶ���
  always @(posedge clock) begin
    ram_rd_addr_d0 <= ram_rd_addr;
    ram_rd_addr_d1 <= ram_rd_addr_d0;
  end

  //���������ӳ�����
  always @(posedge clock) begin
    shiftin_d0 <= shiftin;
    shiftin_d1 <= shiftin_d0;
    shiftin_d2 <= shiftin_d1;
  end

  // //���ڴ洢ǰһ��ͼ���RAM
  // blk_mem_gen_0  u_ram_1024x8_0(    
  //     .clka   (clock), 
  //     .wea   (clken_dly[2]),    //���ӳٵĵ�����ʱ�����ڣ���ǰ�е�����д��RAM0
  //     .addra (ram_rd_addr_d1),
  //     .dina  (shiftin_d2),
  //     .clkb  (clock),
  //     .addrb (ram_rd_addr),
  //     .doutb (taps0x)           //�ӳ�һ��ʱ�����ڣ����RAM0��ǰһ��ͼ�������
  // ); 
/*
  blk_mem_gen_0 u_ram_1024x8_0 (
      .rdclock  (clock),           //input rdclock
      .wrclock  (clock),           //input wrclock
      .data     (shiftin_d2),      //input [7:0] data
      .rdaddress(ram_rd_addr),     //input [9:0] rdaddress
      .wraddress(ram_rd_addr_d1),  //input [9:0] wraddress
      .wren     (clken_dly[2]),    //input wren
      .q        (taps0x)           //output	[7:0] q
  );*/

blk_mem_gen_0 u_ram_1024x8_0 (
  .wr_data(shiftin_d2),    // input [7:0]
  .wr_addr(ram_rd_addr_d1),    // input [9:0]
  .rd_addr(ram_rd_addr),    // input [9:0]
  .wr_clk(clock),      // input
  .rd_clk(clock),      // input
  .wr_en(clken_dly[2]),        // input
  .rst(),            // input
  .rd_data(taps0x)     // output [7:0]
);



  //   dpram #(
  //       .WIDTH(8),
  //       .DEPTH(10)
  //   ) u_dpram (
  //       .clock    (clock),
  //       .data     (shiftin_d2),
  //       .wraddress(ram_rd_addr_d1),
  //       .rdaddress(ram_rd_addr),
  //       .wren     (clken_dly[2]),
  //       .q        (taps0x)
  //   );


  //�Ĵ�һ��ǰһ��ͼ�������
  always @(posedge clock) begin
    taps0x_d0 <= taps0x;
  end

  // blk_mem_gen_0  u_ram_1024x8_1(    
  //     .clka   (clock),           
  //     .wea   (clken_dly[1]),    //���ӳٵĵڶ���ʱ�����ڣ���ǰһ��ͼ�������д��RAM1
  //     .addra (ram_rd_addr_d0),
  //     .dina  (taps0x_d0),
  //     .clkb  (clock),
  //     .addrb (ram_rd_addr),
  //     .doutb (taps1x)           //�ӳ�һ��ʱ�����ڣ����RAM1��ǰǰһ��ͼ�������
  // ); 
/*
  blk_mem_gen_0 u_ram_1024x8_1 (

      .rdclock  (clock),           //input rdclock
      .wrclock  (clock),           //input wrclock
      .data     (taps0x_d0),       //input [7:0] data
      .rdaddress(ram_rd_addr),     //input [9:0] rdaddress
      .wraddress(ram_rd_addr_d0),  //input [9:0] wraddress
      .wren     (clken_dly[1]),    //input wren
      .q        (taps1x)           //output	[7:0] q
  );*/

blk_mem_gen_0 u_ram_1024x8_1 (
  .wr_data(taps0x_d0),    // input [7:0]
  .wr_addr(ram_rd_addr_d0),    // input [9:0]
  .rd_addr(ram_rd_addr),    // input [9:0]
  .wr_clk(clock),      // input
  .rd_clk(clock),      // input
  .wr_en(clken_dly[1]),        // input
  .rst(),            // input
  .rd_data(taps1x)     // output [7:0]
);










  //   dpram #(
  //       .WIDTH(8),
  //       .DEPTH(10)
  //   ) u_dpram2 (
  //       .clock    (clock),
  //       .data     (taps0x_d0),
  //       .wraddress(ram_rd_addr_d0),
  //       .rdaddress(ram_rd_addr),
  //       .wren     (clken_dly[1]),
  //       .q        (taps1x)
  //   );


endmodule
