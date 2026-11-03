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

always @(reg_index) begin
    case (reg_index)
        000: reg_data <= {16'h300a,8'h00};
        001: reg_data <= {16'h300b,8'h00};
        002: reg_data <= {16'h3008,8'h82};
        003: reg_data <= {16'h3008,8'h02};
        004: reg_data <= {16'h3103,8'h02};
        005: reg_data <= {16'h3017,8'hff};
        006: reg_data <= {16'h3018,8'hff};
        007: reg_data <= {16'h3037,8'h13};
        008: reg_data <= {16'h3108,8'h01};
        009: reg_data <= {16'h3630,8'h36};
        010: reg_data <= {16'h3631,8'h0e};
        011: reg_data <= {16'h3632,8'he2};
        012: reg_data <= {16'h3633,8'h12};
        013: reg_data <= {16'h3621,8'he0};
        014: reg_data <= {16'h3704,8'ha0};
        015: reg_data <= {16'h3703,8'h5a};
        016: reg_data <= {16'h3715,8'h78};
        017: reg_data <= {16'h3717,8'h01};
        018: reg_data <= {16'h370b,8'h60};
        019: reg_data <= {16'h3705,8'h1a};
        020: reg_data <= {16'h3905,8'h02};
        021: reg_data <= {16'h3906,8'h10};
        022: reg_data <= {16'h3901,8'h0a};
        023: reg_data <= {16'h3731,8'h12};
        024: reg_data <= {16'h3600,8'h08};
        025: reg_data <= {16'h3601,8'h33};
        026: reg_data <= {16'h302d,8'h60};
        027: reg_data <= {16'h3620,8'h52};
        028: reg_data <= {16'h371b,8'h20};
        029: reg_data <= {16'h471c,8'h50};
        030: reg_data <= {16'h3a13,8'h43};
        031: reg_data <= {16'h3a18,8'h00};
        032: reg_data <= {16'h3a19,8'hf8};
        033: reg_data <= {16'h3635,8'h13};
        034: reg_data <= {16'h3636,8'h03};
        035: reg_data <= {16'h3634,8'h40};
        036: reg_data <= {16'h3622,8'h01};
        037: reg_data <= {16'h3c01,8'h34};
        038: reg_data <= {16'h3c04,8'h28};
        039: reg_data <= {16'h3c05,8'h98};
        040: reg_data <= {16'h3c06,8'h00};
        041: reg_data <= {16'h3c07,8'h08};
        042: reg_data <= {16'h3c08,8'h00};
        043: reg_data <= {16'h3c09,8'h1c};
        044: reg_data <= {16'h3c0a,8'h9c};
        045: reg_data <= {16'h3c0b,8'h40};
        046: reg_data <= {16'h3810,8'h00};
        047: reg_data <= {16'h3811,8'h10};
        048: reg_data <= {16'h3812,8'h00};
        049: reg_data <= {16'h3708,8'h64};
        050: reg_data <= {16'h4001,8'h02};
        051: reg_data <= {16'h4005,8'h1a};
        052: reg_data <= {16'h3000,8'h00};
        053: reg_data <= {16'h3004,8'hff};
        054: reg_data <= {16'h4300,8'h60};
        055: reg_data <= {16'h501f,8'h01};
        056: reg_data <= {16'h440e,8'h00};
        057: reg_data <= {16'h5000,8'ha7};
        058: reg_data <= {16'h3a0f,8'h30};
        059: reg_data <= {16'h3a10,8'h28};
        060: reg_data <= {16'h3a1b,8'h30};
        061: reg_data <= {16'h3a1e,8'h26};
        062: reg_data <= {16'h3a11,8'h60};
        063: reg_data <= {16'h3a1f,8'h14};
        064: reg_data <= {16'h5800,8'h23};
        065: reg_data <= {16'h5801,8'h14};
        066: reg_data <= {16'h5802,8'h0f};
        067: reg_data <= {16'h5803,8'h0f};
        068: reg_data <= {16'h5804,8'h12};
        069: reg_data <= {16'h5805,8'h26};
        070: reg_data <= {16'h5806,8'h0c};
        071: reg_data <= {16'h5807,8'h08};
        072: reg_data <= {16'h5808,8'h05};
        073: reg_data <= {16'h5809,8'h05};
        074: reg_data <= {16'h580a,8'h08};
        075: reg_data <= {16'h580b,8'h0d};
        076: reg_data <= {16'h580c,8'h08};
        077: reg_data <= {16'h580d,8'h03};
        078: reg_data <= {16'h580e,8'h00};
        079: reg_data <= {16'h580f,8'h00};
        080: reg_data <= {16'h5810,8'h03};
        081: reg_data <= {16'h5811,8'h09};
        082: reg_data <= {16'h5812,8'h07};
        083: reg_data <= {16'h5813,8'h03};
        084: reg_data <= {16'h5814,8'h00};
        085: reg_data <= {16'h5815,8'h01};
        086: reg_data <= {16'h5816,8'h03};
        087: reg_data <= {16'h5817,8'h08};
        088: reg_data <= {16'h5818,8'h0d};
        089: reg_data <= {16'h5819,8'h08};
        090: reg_data <= {16'h581a,8'h05};
        091: reg_data <= {16'h581b,8'h06};
        092: reg_data <= {16'h581c,8'h08};
        093: reg_data <= {16'h581d,8'h0e};
        094: reg_data <= {16'h581e,8'h29};
        095: reg_data <= {16'h581f,8'h17};
        096: reg_data <= {16'h5820,8'h11};
        097: reg_data <= {16'h5821,8'h11};
        098: reg_data <= {16'h5822,8'h15};
        099: reg_data <= {16'h5823,8'h28};
        100: reg_data <= {16'h5824,8'h46};
        101: reg_data <= {16'h5825,8'h26};
        102: reg_data <= {16'h5826,8'h08};
        103: reg_data <= {16'h5827,8'h26};
        104: reg_data <= {16'h5828,8'h64};
        105: reg_data <= {16'h5829,8'h26};
        106: reg_data <= {16'h582a,8'h24};
        107: reg_data <= {16'h582b,8'h22};
        108: reg_data <= {16'h582c,8'h24};
        109: reg_data <= {16'h582d,8'h24};
        110: reg_data <= {16'h582e,8'h06};
        111: reg_data <= {16'h582f,8'h22};
        112: reg_data <= {16'h5830,8'h40};
        113: reg_data <= {16'h5831,8'h42};
        114: reg_data <= {16'h5832,8'h24};
        115: reg_data <= {16'h5833,8'h26};
        116: reg_data <= {16'h5834,8'h24};
        117: reg_data <= {16'h5835,8'h22};
        118: reg_data <= {16'h5836,8'h22};
        119: reg_data <= {16'h5837,8'h26};
        120: reg_data <= {16'h5838,8'h44};
        121: reg_data <= {16'h5839,8'h24};
        122: reg_data <= {16'h583a,8'h26};
        123: reg_data <= {16'h583b,8'h28};
        124: reg_data <= {16'h583c,8'h42};
        125: reg_data <= {16'h583d,8'hce};
        126: reg_data <= {16'h5180,8'hff};
        127: reg_data <= {16'h5181,8'hf2};
        128: reg_data <= {16'h5182,8'h00};
        129: reg_data <= {16'h5183,8'h14};
        130: reg_data <= {16'h5184,8'h25};
        131: reg_data <= {16'h5185,8'h24};
        132: reg_data <= {16'h5186,8'h09};
        133: reg_data <= {16'h5187,8'h09};
        134: reg_data <= {16'h5188,8'h09};
        135: reg_data <= {16'h5189,8'h75};
        136: reg_data <= {16'h518a,8'h54};
        137: reg_data <= {16'h518b,8'he0};
        138: reg_data <= {16'h518c,8'hb2};
        139: reg_data <= {16'h518d,8'h42};
        140: reg_data <= {16'h518e,8'h3d};
        141: reg_data <= {16'h518f,8'h56};
        142: reg_data <= {16'h5190,8'h46};
        143: reg_data <= {16'h5191,8'hf8};
        144: reg_data <= {16'h5192,8'h04};
        145: reg_data <= {16'h5193,8'h70};
        146: reg_data <= {16'h5194,8'hf0};
        147: reg_data <= {16'h5195,8'hf0};
        148: reg_data <= {16'h5196,8'h03};
        149: reg_data <= {16'h5197,8'h01};
        150: reg_data <= {16'h5198,8'h04};
        151: reg_data <= {16'h5199,8'h12};
        152: reg_data <= {16'h519a,8'h04};
        153: reg_data <= {16'h519b,8'h00};
        154: reg_data <= {16'h519c,8'h06};
        155: reg_data <= {16'h519d,8'h82};
        156: reg_data <= {16'h519e,8'h38};
        157: reg_data <= {16'h5480,8'h01};
        158: reg_data <= {16'h5481,8'h08};
        159: reg_data <= {16'h5482,8'h14};
        160: reg_data <= {16'h5483,8'h28};
        161: reg_data <= {16'h5484,8'h51};
        162: reg_data <= {16'h5485,8'h65};
        163: reg_data <= {16'h5486,8'h71};
        164: reg_data <= {16'h5487,8'h7d};
        165: reg_data <= {16'h5488,8'h87};
        166: reg_data <= {16'h5489,8'h91};
        167: reg_data <= {16'h548a,8'h9a};
        168: reg_data <= {16'h548b,8'haa};
        169: reg_data <= {16'h548c,8'hb8};
        170: reg_data <= {16'h548d,8'hcd};
        171: reg_data <= {16'h548e,8'hdd};
        172: reg_data <= {16'h548f,8'hea};
        173: reg_data <= {16'h5490,8'h1d};
        174: reg_data <= {16'h5381,8'h1e};
        175: reg_data <= {16'h5382,8'h5b};
        176: reg_data <= {16'h5383,8'h08};
        177: reg_data <= {16'h5384,8'h0a};
        178: reg_data <= {16'h5385,8'h7e};
        179: reg_data <= {16'h5386,8'h88};
        180: reg_data <= {16'h5387,8'h7c};
        181: reg_data <= {16'h5388,8'h6c};
        182: reg_data <= {16'h5389,8'h10};
        183: reg_data <= {16'h538a,8'h01};
        184: reg_data <= {16'h538b,8'h98};
        185: reg_data <= {16'h5580,8'h06};
        186: reg_data <= {16'h5583,8'h40};
        187: reg_data <= {16'h5584,8'h10};
        188: reg_data <= {16'h5589,8'h10};
        189: reg_data <= {16'h558a,8'h00};
        190: reg_data <= {16'h558b,8'hf8};
        191: reg_data <= {16'h501d,8'h40};
        192: reg_data <= {16'h5300,8'h08};
        193: reg_data <= {16'h5301,8'h30};
        194: reg_data <= {16'h5302,8'h10};
        195: reg_data <= {16'h5303,8'h00};
        196: reg_data <= {16'h5304,8'h08};
        197: reg_data <= {16'h5305,8'h30};
        198: reg_data <= {16'h5306,8'h08};
        199: reg_data <= {16'h5307,8'h16};
        200: reg_data <= {16'h5309,8'h08};
        201: reg_data <= {16'h530a,8'h30};
        202: reg_data <= {16'h530b,8'h04};
        203: reg_data <= {16'h530c,8'h06};
        204: reg_data <= {16'h5025,8'h00};
        205: reg_data <= {16'h3035,8'h11};
        206: reg_data <= {16'h3036,8'h46};
        207: reg_data <= {16'h3c07,8'h08};
        208: reg_data <= {16'h3820,8'h40};
        209: reg_data <= {16'h3821,8'h07};
        210: reg_data <= {16'h3814,8'h31};
        211: reg_data <= {16'h3815,8'h31};
        212: reg_data <= {16'h3800,8'h00};
        213: reg_data <= {16'h3801,8'h00};
        214: reg_data <= {16'h3802,8'h00};
        215: reg_data <= {16'h3803,8'h04};
        216: reg_data <= {16'h3804,8'h0a};
        217: reg_data <= {16'h3805,8'h3f};
        218: reg_data <= {16'h3806,8'h07};
        219: reg_data <= {16'h3807,8'h9b};
        220: reg_data <= {16'h3813,8'h06};
        221: reg_data <= {16'h3618,8'h00};
        222: reg_data <= {16'h3612,8'h29};
        223: reg_data <= {16'h3709,8'h52};
        224: reg_data <= {16'h370c,8'h03};
        225: reg_data <= {16'h3a02,8'h17};
        226: reg_data <= {16'h3a03,8'h10};
        227: reg_data <= {16'h3a14,8'h17};
        228: reg_data <= {16'h3a15,8'h10};
        229: reg_data <= {16'h4004,8'h02};
        230: reg_data <= {16'h4713,8'h03};
        231: reg_data <= {16'h4407,8'h04};
        232: reg_data <= {16'h460c,8'h22};
        233: reg_data <= {16'h4837,8'h22};
        234: reg_data <= {16'h3824,8'h02};
        235: reg_data <= {16'h5001,8'ha3};
        236: reg_data <= {16'h3b07,8'h0a};
        237: reg_data <= {16'h503d,8'h00};
        238: reg_data <= {16'h3016,8'h02};
        239: reg_data <= {16'h301c,8'h02};
        240: reg_data <= {16'h3019,8'h02};
        241: reg_data <= {16'h3019,8'h00};
        242: reg_data <= {16'h3808,8'h02};
        243: reg_data <= {16'h3809,8'h80};
        244: reg_data <= {16'h380a,8'h01};
        245: reg_data <= {16'h380b,8'he0};
        246: reg_data <= {16'h380c,8'h07};
        247: reg_data <= {16'h380d,8'h68};
        248: reg_data <= {16'h380e,8'h03};
        249: reg_data <= {16'h380f,8'hd8};
        250: reg_data <= {16'h300a,8'h00};
        default: reg_data <= 24'hffffff;
    endcase
end

endmodule

