// 2026-11-03 注释校订。按表向 OV5640 写 SCCB 寄存器。只有前一笔传输结束才推进配置索引。
`timescale 1ns / 1ps
module reg_config (
    input             clk_25M,
    input             camera_rstn,
    input             initial_en,
    input      [12:0] cmos_h_pixel,
    input      [12:0] cmos_v_pixel,
    input      [12:0] total_h_pixel,
    input      [12:0] total_v_pixel,
    input      [ 7:0] rate,
    input      [12:0] y_addr_st,
    input      [12:0] y_addr_end,
    output            reg_conf_done,
    output            i2c_sclk,
    inout             i2c_sdat,
    output reg        clock_20k,
    output reg [ 8:0] reg_index
);

  reg [15:0] clock_20k_cnt;

  localparam [1:0] SEND_REGISTER = 2'd0, WAIT_TRANSFER = 2'd1, ADVANCE_TABLE = 2'd2;
  localparam [8:0] TABLE_LENGTH = 9'd251;
  reg [1:0] config_step;
  reg [31:0] i2c_data;
  reg [23:0] reg_data;
  reg start;
  reg reg_conf_done_reg;

  i2c_com u1 (
      .clock_i2c(clock_20k),
      .camera_rstn(camera_rstn),
      .ack(ack),
      .i2c_data(i2c_data),
      .start(start),
      .tr_end(tr_end),
      .i2c_sclk(i2c_sclk),
      .i2c_sdat(i2c_sdat)
  );

  assign reg_conf_done = reg_conf_done_reg;

  always @(posedge clk_25M) begin
    if (!camera_rstn) begin
      clock_20k <= 0;
      clock_20k_cnt <= 0;
    end else if (clock_20k_cnt < 1249) clock_20k_cnt <= clock_20k_cnt + 1'b1;
    else begin
      clock_20k <= !clock_20k;
      clock_20k_cnt <= 0;
    end
  end

  always @(posedge clock_20k) begin
    if (!camera_rstn) begin
      config_step <= SEND_REGISTER;
      start <= 0;
      reg_index <= 0;
      reg_conf_done_reg <= 0;
    end else begin
      if (reg_conf_done_reg == 1'b0) begin
        if (reg_index < TABLE_LENGTH) begin
          // 发送寄存器字、等待传输完成、递增索引依次占用三个状态。
          case (config_step)
            SEND_REGISTER: begin
              i2c_data    <= {8'h78, reg_data};
              start       <= 1;
              config_step <= WAIT_TRANSFER;
            end
            WAIT_TRANSFER: begin
              if (tr_end) begin
                start <= 0;
                config_step <= ADVANCE_TABLE;
              end
            end
            ADVANCE_TABLE: begin
              reg_index   <= reg_index + 1'b1;
              config_step <= SEND_REGISTER;
            end
          endcase
        end else reg_conf_done_reg <= 1'b1;
      end
    end
  end

// 每项按寄存器地址、数据字节排列，低位对应最早发送的配置字。
localparam [6143:0] INIT_WORDS = {
    24'hFFFFFF, 24'hFFFFFF, 24'hFFFFFF, 24'hFFFFFF, 24'hFFFFFF, 24'h300A00, 24'h380FD8, 24'h380E03,
    24'h380D68, 24'h380C07, 24'h380BE0, 24'h380A01, 24'h380980, 24'h380802, 24'h301900, 24'h301902,
    24'h301C02, 24'h301602, 24'h503D00, 24'h3B070A, 24'h5001A3, 24'h382402, 24'h483722, 24'h460C22,
    24'h440704, 24'h471303, 24'h400402, 24'h3A1510, 24'h3A1417, 24'h3A0310, 24'h3A0217, 24'h370C03,
    24'h370952, 24'h361229, 24'h361800, 24'h381306, 24'h38079B, 24'h380607, 24'h38053F, 24'h38040A,
    24'h380304, 24'h380200, 24'h380100, 24'h380000, 24'h381531, 24'h381431, 24'h382107, 24'h382040,
    24'h3C0708, 24'h303646, 24'h303511, 24'h502500, 24'h530C06, 24'h530B04, 24'h530A30, 24'h530908,
    24'h530716, 24'h530608, 24'h530530, 24'h530408, 24'h530300, 24'h530210, 24'h530130, 24'h530008,
    24'h501D40, 24'h558BF8, 24'h558A00, 24'h558910, 24'h558410, 24'h558340, 24'h558006, 24'h538B98,
    24'h538A01, 24'h538910, 24'h53886C, 24'h53877C, 24'h538688, 24'h53857E, 24'h53840A, 24'h538308,
    24'h53825B, 24'h53811E, 24'h54901D, 24'h548FEA, 24'h548EDD, 24'h548DCD, 24'h548CB8, 24'h548BAA,
    24'h548A9A, 24'h548991, 24'h548887, 24'h54877D, 24'h548671, 24'h548565, 24'h548451, 24'h548328,
    24'h548214, 24'h548108, 24'h548001, 24'h519E38, 24'h519D82, 24'h519C06, 24'h519B00, 24'h519A04,
    24'h519912, 24'h519804, 24'h519701, 24'h519603, 24'h5195F0, 24'h5194F0, 24'h519370, 24'h519204,
    24'h5191F8, 24'h519046, 24'h518F56, 24'h518E3D, 24'h518D42, 24'h518CB2, 24'h518BE0, 24'h518A54,
    24'h518975, 24'h518809, 24'h518709, 24'h518609, 24'h518524, 24'h518425, 24'h518314, 24'h518200,
    24'h5181F2, 24'h5180FF, 24'h583DCE, 24'h583C42, 24'h583B28, 24'h583A26, 24'h583924, 24'h583844,
    24'h583726, 24'h583622, 24'h583522, 24'h583424, 24'h583326, 24'h583224, 24'h583142, 24'h583040,
    24'h582F22, 24'h582E06, 24'h582D24, 24'h582C24, 24'h582B22, 24'h582A24, 24'h582926, 24'h582864,
    24'h582726, 24'h582608, 24'h582526, 24'h582446, 24'h582328, 24'h582215, 24'h582111, 24'h582011,
    24'h581F17, 24'h581E29, 24'h581D0E, 24'h581C08, 24'h581B06, 24'h581A05, 24'h581908, 24'h58180D,
    24'h581708, 24'h581603, 24'h581501, 24'h581400, 24'h581303, 24'h581207, 24'h581109, 24'h581003,
    24'h580F00, 24'h580E00, 24'h580D03, 24'h580C08, 24'h580B0D, 24'h580A08, 24'h580905, 24'h580805,
    24'h580708, 24'h58060C, 24'h580526, 24'h580412, 24'h58030F, 24'h58020F, 24'h580114, 24'h580023,
    24'h3A1F14, 24'h3A1160, 24'h3A1E26, 24'h3A1B30, 24'h3A1028, 24'h3A0F30, 24'h5000A7, 24'h440E00,
    24'h501F01, 24'h430060, 24'h3004FF, 24'h300000, 24'h40051A, 24'h400102, 24'h370864, 24'h381200,
    24'h381110, 24'h381000, 24'h3C0B40, 24'h3C0A9C, 24'h3C091C, 24'h3C0800, 24'h3C0708, 24'h3C0600,
    24'h3C0598, 24'h3C0428, 24'h3C0134, 24'h362201, 24'h363440, 24'h363603, 24'h363513, 24'h3A19F8,
    24'h3A1800, 24'h3A1343, 24'h471C50, 24'h371B20, 24'h362052, 24'h302D60, 24'h360133, 24'h360008,
    24'h373112, 24'h39010A, 24'h390610, 24'h390502, 24'h37051A, 24'h370B60, 24'h371701, 24'h371578,
    24'h37035A, 24'h3704A0, 24'h3621E0, 24'h363312, 24'h3632E2, 24'h36310E, 24'h363036, 24'h310801,
    24'h303713, 24'h3018FF, 24'h3017FF, 24'h310302, 24'h300802, 24'h300882, 24'h300B00, 24'h300A00
};

// 保留原索引触发和非阻塞更新。越界或未知索引返回全一。
always @(reg_index) begin
    case (^reg_index)
        1'b0, 1'b1: begin
            if (reg_index[8])
                reg_data <= 24'hffffff;
            else
                reg_data <= INIT_WORDS[reg_index[7:0] * 24 +: 24];
        end
        default: reg_data <= 24'hffffff;
    endcase
end

endmodule

