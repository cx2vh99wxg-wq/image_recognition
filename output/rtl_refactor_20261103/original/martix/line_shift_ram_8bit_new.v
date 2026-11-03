module line_shift_ram_8bit_new
    (input clock,
     input clken,
     input rst_n,
     input [7 : 0] shiftin,
     output [7 : 0] taps0x,
     output [7 : 0] taps1x);

    // 停用示例，仅供对照：reg define
    reg [2 : 0] clken_dly;
    reg [9 : 0] ram_rd_addr;
    reg [9 : 0] ram_rd_addr_d0;
    reg [9 : 0] ram_rd_addr_d1;
    reg [7 : 0] shiftin_d0;
    reg [7 : 0] shiftin_d1;
    reg [7 : 0] shiftin_d2;
    reg [7 : 0] taps0x_d0;

    // ── 像素窗口 ──
    // 像素窗口：** main code
    // ── 像素窗口 ──

    // 旧说明含编码损坏，原记录为：����������ʱ��ram��ַ�ۼ�
    always @(posedge clock)
    begin
        if (!rst_n)
        begin
            ram_rd_addr <= 0;
        end
        else if (ram_rd_addr == 10'd640)
        begin
            ram_rd_addr <= 0;
        end
        else if (clken)
            ram_rd_addr <= ram_rd_addr + 1;
        else
            ram_rd_addr <= ram_rd_addr;
    end

    // 旧说明含编码损坏，原记录为：ʱ��ʹ���ź��ӳ�����
    always @(posedge clock)
    begin
        clken_dly <= {clken_dly[1 : 0], clken};
    end

    // 旧说明含编码损坏，原记录为：��ram��ַ�ӳٶ���
    always @(posedge clock)
    begin
        ram_rd_addr_d0 <= ram_rd_addr;
        ram_rd_addr_d1 <= ram_rd_addr_d0;
    end

    // 旧说明含编码损坏，原记录为：���������ӳ�����
    always @(posedge clock)
    begin
        shiftin_d0 <= shiftin;
        shiftin_d1 <= shiftin_d0;
        shiftin_d2 <= shiftin_d1;
    end

    // 旧说明含编码损坏，原记录为：//���ڴ洢ǰһ��ͼ���RAM
    // 参数记录为 blk_mem_gen_0 u_ram_1024x8_0(。
    // 停用示例，仅供对照：.clka (clock),
    // 停用示例，仅供对照：.wea (clken_dly[2]), //���ӳٵĵ�����ʱ�����ڣ���ǰ�е�����д��RAM0
    // 停用示例，仅供对照：.addra (ram_rd_addr_d1),
    // 停用示例，仅供对照：.dina (shiftin_d2),
    // 停用示例，仅供对照：.clkb (clock),
    // 停用示例，仅供对照：.addrb (ram_rd_addr),
    // 旧说明含编码损坏，原记录为：.doutb (taps0x) //�ӳ�һ��ʱ�����ڣ����RAM0��ǰһ��ͼ�������
    // 像素窗口：);
    /* 停用示例，仅供对照：
  blk_mem_gen_0 u_ram_1024x8_0 (
      .rdclock  (clock),           //input rdclock
      .wrclock  (clock),           //input wrclock
      .data     (shiftin_d2),      //input [7:0] data
      .rdaddress(ram_rd_addr),     //input [9:0] rdaddress
      .wraddress(ram_rd_addr_d1),  //input [9:0] wraddress
      .wren     (clken_dly[2]),    //input wren
      .q        (taps0x)           //output	[7:0] q
  );*/

    blk_mem_gen_0 u_ram_1024x8_0(.wr_data(shiftin_d2),
                                 // 停用示例，仅供对照：input [7:0]
                                 .wr_addr(ram_rd_addr_d1),
                                 // 停用示例，仅供对照：input [9:0]
                                 .rd_addr(ram_rd_addr),
                                 // 停用示例，仅供对照：input [9:0]
                                 .wr_clk(clock),
                                 // 停用示例，仅供对照：input
                                 .rd_clk(clock),
                                 // 停用示例，仅供对照：input
                                 .wr_en(clken_dly[2]),
                                 // 停用示例，仅供对照：input
                                 .rst(),
                                 // 停用示例，仅供对照：input
                                 .rd_data(taps0x) // 停用示例，仅供对照：output [7:0]
                  );

    // 像素窗口：dpram #(
    // 停用示例，仅供对照：.WIDTH(8),
    // 参数记录为 .DEPTH(10)。
    // 像素窗口：) u_dpram (
    // 停用示例，仅供对照：.clock (clock),
    // 停用示例，仅供对照：.data (shiftin_d2),
    // 停用示例，仅供对照：.wraddress(ram_rd_addr_d1),
    // 停用示例，仅供对照：.rdaddress(ram_rd_addr),
    // 停用示例，仅供对照：.wren (clken_dly[2]),
    // 参数记录为 .q (taps0x)。
    // 像素窗口：);

    // 旧说明含编码损坏，原记录为：�Ĵ�һ��ǰһ��ͼ�������
    always @(posedge clock)
    begin
        taps0x_d0 <= taps0x;
    end

    // 参数记录为 blk_mem_gen_0 u_ram_1024x8_1(。
    // 停用示例，仅供对照：.clka (clock),
    // 停用示例，仅供对照：.wea (clken_dly[1]), //���ӳٵĵڶ���ʱ�����ڣ���ǰһ��ͼ�������д��RAM1
    // 停用示例，仅供对照：.addra (ram_rd_addr_d0),
    // 停用示例，仅供对照：.dina (taps0x_d0),
    // 停用示例，仅供对照：.clkb (clock),
    // 停用示例，仅供对照：.addrb (ram_rd_addr),
    // 旧说明含编码损坏，原记录为：.doutb (taps1x) //�ӳ�һ��ʱ�����ڣ����RAM1��ǰǰһ��ͼ�������
    // 像素窗口：);
    /* 停用示例，仅供对照：
  blk_mem_gen_0 u_ram_1024x8_1 (

      .rdclock  (clock),           //input rdclock
      .wrclock  (clock),           //input wrclock
      .data     (taps0x_d0),       //input [7:0] data
      .rdaddress(ram_rd_addr),     //input [9:0] rdaddress
      .wraddress(ram_rd_addr_d0),  //input [9:0] wraddress
      .wren     (clken_dly[1]),    //input wren
      .q        (taps1x)           //output	[7:0] q
  );*/

    blk_mem_gen_0 u_ram_1024x8_1(.wr_data(taps0x_d0),
                                 // 停用示例，仅供对照：input [7:0]
                                 .wr_addr(ram_rd_addr_d0),
                                 // 停用示例，仅供对照：input [9:0]
                                 .rd_addr(ram_rd_addr),
                                 // 停用示例，仅供对照：input [9:0]
                                 .wr_clk(clock),
                                 // 停用示例，仅供对照：input
                                 .rd_clk(clock),
                                 // 停用示例，仅供对照：input
                                 .wr_en(clken_dly[1]),
                                 // 停用示例，仅供对照：input
                                 .rst(),
                                 // 停用示例，仅供对照：input
                                 .rd_data(taps1x) // 停用示例，仅供对照：output [7:0]
                  );

    // 像素窗口：dpram #(
    // 停用示例，仅供对照：.WIDTH(8),
    // 参数记录为 .DEPTH(10)。
    // 参数记录为 ) u_dpram2 (。
    // 停用示例，仅供对照：.clock (clock),
    // 停用示例，仅供对照：.data (taps0x_d0),
    // 停用示例，仅供对照：.wraddress(ram_rd_addr_d0),
    // 停用示例，仅供对照：.rdaddress(ram_rd_addr),
    // 停用示例，仅供对照：.wren (clken_dly[1]),
    // 参数记录为 .q (taps1x)。
    // 像素窗口：);
endmodule
