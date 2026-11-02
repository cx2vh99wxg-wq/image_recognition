`timescale 1ns / 1ps
module reg_config (
    input             clk_25M,
    input             camera_rstn,
    input             initial_en,
    input      [12:0] cmos_h_pixel,
    input      [12:0] cmos_v_pixel,
    input      [12:0] total_h_pixel,  //水平总像素大小
    input      [12:0] total_v_pixel,  //垂直总像素大小
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
  //����i2c����ʱ��-20khz    
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


  ////iic�Ĵ������ù��̿���    
  always @(posedge clock_20k) begin
    if (!camera_rstn) begin
      config_step <= 0;
      start <= 0;
      reg_index <= 0;
      reg_conf_done_reg <= 0;
    end else begin
      if (reg_conf_done_reg == 1'b0) begin  //���camera��ʼ��δ���
        if (reg_index < 251) begin  //���üĴ���
          case (config_step)
            0: begin
              i2c_data    <= {8'h78, reg_data};  //OV5640 IIC Device address is 0x78   
              start       <= 1;  //i2cд��ʼ
              config_step <= 1;
            end
            1: begin
              if (tr_end) begin  //i2cд����               					
                start <= 0;
                config_step <= 2;
              end
            end
            2: begin
              reg_index   <= reg_index + 1'b1;  //������һ���Ĵ���
              config_step <= 0;
            end
          endcase
        end else reg_conf_done_reg <= 1'b1;  //OV5640�Ĵ�����ʼ�����
      end
    end
  end
/*
  ////iic��Ҫ���õļĴ���ֵ  			
  always @(reg_index) begin
    case (reg_index)
      0: reg_data <= 24'h310311;  //      
      1: reg_data <= 24'h300882;  //      

      102: reg_data <= 24'h300842;  //      
      103: reg_data <= 24'h310303;  //      
      104: reg_data <= 24'h3017ff;  //      
      105: reg_data <= 24'h3018ff;  //      
      106: reg_data <= 24'h30341A;  //      
      107: reg_data <= 24'h303713;  //      
      108: reg_data <= 24'h310801;  //      
      109: reg_data <= 24'h363036;  //      
      110: reg_data <= 24'h36310e;  //       
      111: reg_data <= 24'h3632e2;  //       
      112: reg_data <= 24'h363312;  //       
      113: reg_data <= 24'h3621e0;  //       
      114: reg_data <= 24'h3704a0;  //       
      115: reg_data <= 24'h37035a;  //       
      116: reg_data <= 24'h371578;  //       
      117: reg_data <= 24'h371701;  //       
      118: reg_data <= 24'h370b60;  //       
      119: reg_data <= 24'h37051a;  //       
      120: reg_data <= 24'h390502;  //       
      121: reg_data <= 24'h390610;  //       
      122: reg_data <= 24'h39010a;  //       
      123: reg_data <= 24'h373112;  //       
      124: reg_data <= 24'h360008;  //       
      125: reg_data <= 24'h360133;  //       
      126: reg_data <= 24'h302d60;  //       
      127: reg_data <= 24'h362052;  //       
      128: reg_data <= 24'h371b20;  //       
      129: reg_data <= 24'h471c50;  //       
      130: reg_data <= 24'h3a1343;  //       
      131: reg_data <= 24'h3a1800;  //       
      132: reg_data <= 24'h3a19f8;  //       
      133: reg_data <= 24'h363513;  //       
      134: reg_data <= 24'h363603;  //       
      135: reg_data <= 24'h363440;  //       
      136: reg_data <= 24'h362201;  ///      
      137: reg_data <= 24'h3c0134;  //       
      138: reg_data <= 24'h3c0428;  //       
      139: reg_data <= 24'h3c0598;  //       
      140: reg_data <= 24'h3c0600;  //       
      141: reg_data <= 24'h3c0708;  //       
      142: reg_data <= 24'h3c0800;  //       
      143: reg_data <= 24'h3c091c;  //       
      144: reg_data <= 24'h3c0a9c;  //       
      145: reg_data <= 24'h3c0b40;  //       
      146: reg_data <= 24'h381000;  //       
      147: reg_data <= 24'h381110;  //       
      148: reg_data <= 24'h381200;  //       
      149: reg_data <= 24'h370864;  //       
      150: reg_data <= 24'h400102;  //       
      151: reg_data <= 24'h40051a;  //       
      152: reg_data <= 24'h300000;  //       
      153: reg_data <= 24'h3004ff;  //       
      154: reg_data <= 24'h300e58;  //       
      155: reg_data <= 24'h302e00;  //       
      156: reg_data <= 24'h430060;  //       
      157: reg_data <= 24'h501f01;  //       
      158: reg_data <= 24'h440e00;  //       
      159: reg_data <= 24'h5000a7;  //     
      160: reg_data <= 24'h3a0f30;  //       
      161: reg_data <= 24'h3a1028;  //       
      162: reg_data <= 24'h3a1b30;  //       
      163: reg_data <= 24'h3a1e26;  //       
      164: reg_data <= 24'h3a1160;  //       
      165: reg_data <= 24'h3a1f14;  //       
      166: reg_data <= 24'h580023;  //       
      167: reg_data <= 24'h580114;  //       
      168: reg_data <= 24'h58020f;  //       
      169: reg_data <= 24'h58030f;  //       
      170: reg_data <= 24'h580412;  //       
      171: reg_data <= 24'h580526;  //       
      172: reg_data <= 24'h58060c;  //       
      173: reg_data <= 24'h580708;  //       
      174: reg_data <= 24'h580805;  //       
      175: reg_data <= 24'h580905;  //       
      176: reg_data <= 24'h580a08;  //       
      177: reg_data <= 24'h580b0d;  //       
      178: reg_data <= 24'h580c08;  //       
      179: reg_data <= 24'h580d03;  //       
      180: reg_data <= 24'h580e00;  //       
      181: reg_data <= 24'h580f00;  //       
      182: reg_data <= 24'h581003;  //       
      183: reg_data <= 24'h581109;  //       
      184: reg_data <= 24'h581207;  //       
      185: reg_data <= 24'h581303;  //       
      186: reg_data <= 24'h581400;  //       
      187: reg_data <= 24'h581501;  //       
      188: reg_data <= 24'h581603;  //       
      189: reg_data <= 24'h581708;  //       
      190: reg_data <= 24'h58180d;  //       
      191: reg_data <= 24'h581908;  //       
      192: reg_data <= 24'h581a05;  //       
      193: reg_data <= 24'h581b06;  //       
      194: reg_data <= 24'h581c08;  //       
      195: reg_data <= 24'h581d0e;  //       
      196: reg_data <= 24'h581e29;  //       
      197: reg_data <= 24'h581f17;  //       
      198: reg_data <= 24'h582011;  //       
      199: reg_data <= 24'h582111;  //       
      200: reg_data <= 24'h582215;  //        
      201: reg_data <= 24'h582328;  //        
      202: reg_data <= 24'h582446;  //        
      203: reg_data <= 24'h582526;  //        
      204: reg_data <= 24'h582608;  //        
      205: reg_data <= 24'h582726;  //        
      206: reg_data <= 24'h582864;  //        
      207: reg_data <= 24'h582926;  //        
      208: reg_data <= 24'h582a24;  //        
      209: reg_data <= 24'h582b22;  //        
      210: reg_data <= 24'h582c24;  //        
      211: reg_data <= 24'h582d24;  //        
      212: reg_data <= 24'h582e06;  //        
      213: reg_data <= 24'h582f22;  //        
      214: reg_data <= 24'h583040;  //        
      215: reg_data <= 24'h583142;  //        
      216: reg_data <= 24'h583224;  //        
      217: reg_data <= 24'h583326;  //        
      218: reg_data <= 24'h583424;  //        
      219: reg_data <= 24'h583522;  //        
      220: reg_data <= 24'h583622;  //        
      221: reg_data <= 24'h583726;  //        
      222: reg_data <= 24'h583844;  //        
      223: reg_data <= 24'h583924;  //        
      224: reg_data <= 24'h583a26;  //        
      225: reg_data <= 24'h583b28;  //        
      226: reg_data <= 24'h583c42;  //        
      227: reg_data <= 24'h583dce;  //        
      228: reg_data <= 24'h5180ff;  //        
      229: reg_data <= 24'h5181f2;  //        
      230: reg_data <= 24'h518200;  //        
      231: reg_data <= 24'h518314;  //        
      232: reg_data <= 24'h518425;  //        
      233: reg_data <= 24'h518524;  //        
      234: reg_data <= 24'h518609;  //        
      235: reg_data <= 24'h518709;  //        
      236: reg_data <= 24'h518809;  //        
      237: reg_data <= 24'h518975;  //        
      238: reg_data <= 24'h518a54;  //        
      239: reg_data <= 24'h518be0;  //        
      240: reg_data <= 24'h518cb2;  //        
      241: reg_data <= 24'h518d42;  //        
      242: reg_data <= 24'h518e3d;  //        
      243: reg_data <= 24'h518f56;  //        
      244: reg_data <= 24'h519046;  //        
      245: reg_data <= 24'h5191f8;  //        
      246: reg_data <= 24'h519204;  //        
      247: reg_data <= 24'h519370;  //        
      248: reg_data <= 24'h5194f0;  //        
      249: reg_data <= 24'h5195f0;  //        
      250: reg_data <= 24'h519603;  //        
      251: reg_data <= 24'h519701;  //        
      252: reg_data <= 24'h519804;  //        
      253: reg_data <= 24'h519912;  //        
      254: reg_data <= 24'h519a04;  //        
      255: reg_data <= 24'h519b00;  //        
      256: reg_data <= 24'h519c06;  //        
      257: reg_data <= 24'h519d82;  //        
      258: reg_data <= 24'h519e38;  //        
      259: reg_data <= 24'h548001;  //        
      260: reg_data <= 24'h548108;  //        
      261: reg_data <= 24'h548214;  //        
      262: reg_data <= 24'h548328;  //        
      263: reg_data <= 24'h548451;  //        
      264: reg_data <= 24'h548565;  //        
      265: reg_data <= 24'h548671;  //        
      266: reg_data <= 24'h54877d;  //        
      267: reg_data <= 24'h548887;  //        
      268: reg_data <= 24'h548991;  //        
      269: reg_data <= 24'h548a9a;  //        
      270: reg_data <= 24'h548baa;  //        
      271: reg_data <= 24'h548cb8;  //        
      272: reg_data <= 24'h548dcd;  //        
      273: reg_data <= 24'h548edd;  //        
      274: reg_data <= 24'h548fea;  //        
      275: reg_data <= 24'h54901d;  //        
      276: reg_data <= 24'h53811e;  //        
      277: reg_data <= 24'h53825b;  //        
      278: reg_data <= 24'h538308;  //        
      279: reg_data <= 24'h53840a;  //        
      280: reg_data <= 24'h53857e;  //        
      281: reg_data <= 24'h538688;  //        
      282: reg_data <= 24'h53877c;  //        
      283: reg_data <= 24'h53886c;  //        
      284: reg_data <= 24'h538910;  //        
      285: reg_data <= 24'h538a01;  //        
      286: reg_data <= 24'h538b98;  //       
      287: reg_data <= 24'h558006;  //        
      288: reg_data <= 24'h558340;  //        
      289: reg_data <= 24'h558410;  //        
      290: reg_data <= 24'h558910;  //        
      291: reg_data <= 24'h558a00;  //        
      292: reg_data <= 24'h558bf8;  //        
      293: reg_data <= 24'h501d40;  //        
      294: reg_data <= 24'h530008;  //        
      295: reg_data <= 24'h530130;  //        
      296: reg_data <= 24'h530210;  //        
      297: reg_data <= 24'h530300;  //        
      298: reg_data <= 24'h530408;  //        
      299: reg_data <= 24'h530530;  //        
      300: reg_data <= 24'h530608;  //        
      301: reg_data <= 24'h530716;  //        
      302: reg_data <= 24'h530908;  //        
      303: reg_data <= 24'h530a30;  //        
      304: reg_data <= 24'h530b04;  //        
      305: reg_data <= 24'h530c06;  //        
      306: reg_data <= 24'h502500;  //        
      307: reg_data <= 24'h300802;  //       
      //720 30֡/��, night mode 5fps ;//         
      //input clock=24Mhz,PCLK=84Mhz ;//         
      //  308  :reg_data <=24'h303521 ;//PLL      
      308: reg_data <= {16'h3035, rate};  //41:15fps, 21:30Fps, 11:60Fps
      309: reg_data <= 24'h303646;  //PLL倍频=105,提供更高帧率 (原0x3c=60倍频)
     //  309: reg_data <= 24'h30363c;  //3c=60倍频(较保守)    
      310: reg_data <= 24'h3c0703;  //        
      311: reg_data <= 24'h382047;  //        
      312: reg_data <= 24'h382100;  //        
      313: reg_data <= 24'h381431;  //        
      314: reg_data <= 24'h381531;  //        
      315: reg_data <= 24'h380000;  //        
      316: reg_data <= 24'h380100;  //        
      //  317  :reg_data <=24'h380200 ;//        
      317: reg_data <= {16'h3802, {3'd0, y_addr_st[12:8]}};
      //  318  :reg_data <=24'h3803fa ;//        
      318: reg_data <= {16'h3803, y_addr_st[7:0]};
      319: reg_data <= 24'h38040a;  //        
      320: reg_data <= 24'h38053f;  //        
      //  321  :reg_data <=24'h380606 ;//        
      321: reg_data <= {16'h3806, {3'd0, y_addr_end[12:8]}};
      //  322  :reg_data <=24'h3807a9 ;//        
      322: reg_data <= {16'h3807, y_addr_end[7:0]};
      //  323  :reg_data <=24'h380805 ;//        
      323: reg_data <= {16'h3808, {4'd0, cmos_h_pixel[11:8]}};
      //  324  :reg_data <=24'h380900 ;//        
      324: reg_data <= {16'h3809, cmos_h_pixel[7:0]};
      //  325  :reg_data <=24'h380a02 ;//        
      325: reg_data <= {16'h380a, {5'd0, cmos_v_pixel[10:8]}};
      //  326  :reg_data <=24'h380bd0 ;//        
      326: reg_data <= {16'h380b, cmos_v_pixel[7:0]};
      //  327  :reg_data <=24'h380c07 ;//        
      327: reg_data <= {16'h380c, {3'd0, total_h_pixel[12:8]}};
      //  328  :reg_data <=24'h380d64 ;//        
      328: reg_data <= {16'h380d, total_h_pixel[7:0]};
      //  329  :reg_data <=24'h380e02 ;//        
      329: reg_data <= {16'h380e, {3'd0, total_v_pixel[12:8]}};
      //  330  :reg_data <=24'h380fe4 ;//        
      330: reg_data <= {16'h380f, total_v_pixel[7:0]};
      331: reg_data <= 24'h381304;  //        
      332: reg_data <= 24'h361800;  //        
      333: reg_data <= 24'h361229;  //        
      334: reg_data <= 24'h370952;  //        
      335: reg_data <= 24'h370c03;  //        
      336: reg_data <= 24'h3a0202;  //        
      337: reg_data <= 24'h3a03e0;  //        

      338: reg_data <= 24'h3a0800;  //        
      339: reg_data <= 24'h3a096f;  //        
      340: reg_data <= 24'h3a0a00;  //        
      341: reg_data <= 24'h3a0b5c;  //        
      342: reg_data <= 24'h3a0e06;  //        
      343: reg_data <= 24'h3a0d08;  //        
      344: reg_data <= 24'h3a1402;  //         
      345: reg_data <= 24'h3a15e0;  //         
      346: reg_data <= 24'h400402;  //         
      347: reg_data <= 24'h30021c;  //         
      348: reg_data <= 24'h3006c3;  //         
      349: reg_data <= 24'h471303;  //         
      350: reg_data <= 24'h440704;  //         
      351: reg_data <= 24'h460b37;  //           
      352: reg_data <= 24'h460c20;  //          
      353: reg_data <= 24'h483716;  //         
      354: reg_data <= 24'h382404;  //         
      355: reg_data <= 24'h500183;  //         
      356: reg_data <= 24'h350300;  //         
      default: reg_data <= 24'hffffff;  //        
    endcase
  end
*/

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
        054: reg_data <= {16'h4300,8'h60};//输出显示
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
        205: reg_data <= {16'h3035,8'h11};//帧率
        206: reg_data <= {16'h3036,8'h46};
        207: reg_data <= {16'h3c07,8'h08};//08
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

