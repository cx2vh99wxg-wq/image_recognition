module char_display (
    input wire clk_pixel,//像素时钟
    input wire sys_clk, //50mhz系统时钟
    input wire [23:0]rgb,
    input wire rst_n,
	input wire [15:0] char_en,
    input wire i_vs,
    input wire i_hs,
    input wire i_de,
    input wire [9:0] h_cnt,  
    input wire [9:0] v_cnt,   
	input wire [10:0]col_left,
	input wire [10:0]row_low,
    output wire o_hs,
    output wire o_de,
	output wire o_vs,
    output [23:0] pixel_rgb 
);


assign pixel_rgb = pixel_rgb_reg;
assign o_vs = vs_d0;
assign o_hs = hs_d0;
assign o_de = de_d0;
reg [23:0] pixel_rgb_reg;
reg vs_d0,hs_d0,de_d0;

///////////////////char_init////////////////////

reg [79:0]result_char[0:623];
always@(*) begin
    result_char[0] =   80'h00000040000000000000; //speed15 
    result_char[1] =   80'h7bf82040000000000000;  
    result_char[2] =   80'h4a0817fc000000000000;  
    result_char[3] =   80'h52081040087e00000000;  
    result_char[4] =   80'h53f803f8384000000000;  
    result_char[5] =   80'h62080248084000000000;  
    result_char[6] =   80'h5208f248084000000000;  
    result_char[7] =   80'h4bf813f8087800000000;  
    result_char[8] =   80'h4a4410e0084400000000;  
    result_char[9] =   80'h4a481150080200000000;  
    result_char[10]=   80'h6a301248080200000000; 
    result_char[11]=   80'h52201444084200000000; 
    result_char[12]=   80'h42101040084400000000; 
    result_char[13]=   80'h428828003e3800000000; 
    result_char[14]=   80'h430647fe000000000000; 
    result_char[15]=   80'h42000000000000000000; 
     
    result_char[16] =  80'h00000040000000000000; //speed30
    result_char[17] =  80'h7bf82040000000000000; 
    result_char[18] =  80'h4a0817fc000000000000; 
    result_char[19] =  80'h520810403c1800000000; 
    result_char[20] =  80'h53f803f8422400000000; 
    result_char[21] =  80'h62080248424200000000; 
    result_char[22] =  80'h5208f248024200000000; 
    result_char[23] =  80'h4bf813f8044200000000; 
    result_char[24] =  80'h4a4410e0184200000000; 
    result_char[25] =  80'h4a481150044200000000; 
    result_char[26] =  80'h6a301248024200000000;
    result_char[27] =  80'h52201444424200000000;
    result_char[28] =  80'h42101040422400000000;
    result_char[29] =  80'h428828003c1800000000;
    result_char[30] =  80'h430647fe000000000000;
    result_char[31] =  80'h42000000000000000000;
 
    result_char[32] =  80'h00000040000000000000;  //speed40
    result_char[33] =  80'h7bf82040000000000000;  
    result_char[34] =  80'h4a0817fc000000000000;  
    result_char[35] =  80'h52081040041800000000;  
    result_char[36] =  80'h53f803f80c2400000000;  
    result_char[37] =  80'h620802480c4200000000;  
    result_char[38] =  80'h5208f248144200000000;  
    result_char[39] =  80'h4bf813f8244200000000;  
    result_char[40] =  80'h4a4410e0244200000000;  
    result_char[41] =  80'h4a481150444200000000;  
    result_char[42] =  80'h6a3012487f4200000000; 
    result_char[43] =  80'h52201444044200000000; 
    result_char[44] =  80'h42101040042400000000; 
    result_char[45] =  80'h428828001f1800000000; 
    result_char[46] =  80'h430647fe000000000000; 
    result_char[47] =  80'h42000000000000000000; 
      
    result_char[48] =  80'h00000040000000000000;  //speed50
    result_char[49] =  80'h7bf82040000000000000;  
    result_char[50] =  80'h4a0817fc000000000000;  
    result_char[51] =  80'h520810407e1800000000;  
    result_char[52] =  80'h53f803f8402400000000;  
    result_char[53] =  80'h62080248404200000000;  
    result_char[54] =  80'h5208f248404200000000;  
    result_char[55] =  80'h4bf813f8784200000000;  
    result_char[56] =  80'h4a4410e0444200000000;  
    result_char[57] =  80'h4a481150024200000000;  
    result_char[58] =  80'h6a301248024200000000; 
    result_char[59] =  80'h52201444424200000000; 
    result_char[60] =  80'h42101040442400000000; 
    result_char[61] =  80'h42882800381800000000; 
    result_char[62] =  80'h430647fe000000000000; 
    result_char[63] =  80'h42000000000000000000; 
      
    result_char[64] =  80'h00000040000000000000; //speed60
    result_char[65] =  80'h7bf82040000000000000; 
    result_char[66] =  80'h4a0817fc000000000000; 
    result_char[67] =  80'h52081040181800000000; 
    result_char[68] =  80'h53f803f8242400000000; 
    result_char[69] =  80'h62080248404200000000; 
    result_char[70] =  80'h5208f248404200000000; 
    result_char[71] =  80'h4bf813f85c4200000000; 
    result_char[72] =  80'h4a4410e0624200000000; 
    result_char[73] =  80'h4a481150424200000000; 
    result_char[74] =  80'h6a301248424200000000;
    result_char[75] =  80'h52201444424200000000;
    result_char[76] =  80'h42101040222400000000;
    result_char[77] =  80'h428828001c1800000000;
    result_char[78] =  80'h430647fe000000000000;
    result_char[79] =  80'h42000000000000000000;
	
	result_char[80] =  80'h01000800000000000000;//直行
	result_char[81] =  80'h010009fc000000000000;
	result_char[82] =  80'h7ffc1000000000000000;
	result_char[83] =  80'h01002000000000000000;
	result_char[84] =  80'h1ff04800000000000000;
	result_char[85] =  80'h10100800000000000000;
	result_char[86] =  80'h101013fe000000000000;
	result_char[87] =  80'h1ff03020000000000000;
	result_char[88] =  80'h10105020000000000000;
	result_char[89] =  80'h1ff09020000000000000;
	result_char[90] =  80'h10101020000000000000;
	result_char[91] =  80'h1ff01020000000000000;
	result_char[92] =  80'h10101020000000000000;
	result_char[93] =  80'h10101020000000000000;
	result_char[94] =  80'hfffe10a0000000000000;
	result_char[95] =  80'h00001040000000000000;
	
	result_char[96] =  80'h02002020000000000000;//左转
    result_char[97] =  80'h02002020000000000000;
    result_char[98] =  80'h02002020000000000000;
    result_char[99] =  80'hfffefdfc000000000000;
    result_char[100] = 80'h04004020000000000000;
    result_char[101] = 80'h04005040000000000000;
    result_char[102] = 80'h040093fe000000000000;
    result_char[103] = 80'h0800fc40000000000000;
    result_char[104] = 80'h0ff81080000000000000;
    result_char[105] = 80'h108011fc000000000000;
    result_char[106] = 80'h10801c04000000000000;
    result_char[107] = 80'h2080f088000000000000;
    result_char[108] = 80'h40805050000000000000;
    result_char[109] = 80'h80801020000000000000;
    result_char[110] = 80'h3ffe1010000000000000;
    result_char[111] = 80'h00001010000000000000;
	
	result_char[112] = 80'h02002020000000000000;//右转
    result_char[113] = 80'h02002020000000000000;
    result_char[114] = 80'h02002020000000000000;
    result_char[115] = 80'hfffefdfc000000000000;
    result_char[116] = 80'h04004020000000000000;
    result_char[117] = 80'h04005040000000000000;
    result_char[118] = 80'h080093fe000000000000;
    result_char[119] = 80'h0800fc40000000000000;
    result_char[120] = 80'h1ff81080000000000000;
    result_char[121] = 80'h280811fc000000000000;
    result_char[122] = 80'h48081c04000000000000;
    result_char[123] = 80'h8808f088000000000000;
    result_char[124] = 80'h08085050000000000000;
    result_char[125] = 80'h08081020000000000000;
    result_char[126] = 80'h0ff81010000000000000;
    result_char[127] = 80'h08081010000000000000;
	
	result_char[128] = 80'h01000800004802002020;//直行或左转
    result_char[129] = 80'h010009fc004402002020; 
    result_char[130] = 80'h7ffc1000004002002020; 
    result_char[131] = 80'h01002000fffefffefdfc; 
    result_char[132] = 80'h1ff04800004004004020; 
    result_char[133] = 80'h10100800004004005040; 
    result_char[134] = 80'h101013fe3e44040093fe; 
    result_char[135] = 80'h1ff0302022440800fc40; 
    result_char[136] = 80'h1010502022440ff81080; 
    result_char[137] = 80'h1ff090202228108011fc; 
    result_char[138] = 80'h101010203e2810801c04; 
    result_char[139] = 80'h1ff0102000122080f088; 
    result_char[140] = 80'h10101020073240805050; 
    result_char[141] = 80'h10101020784a80801020; 
    result_char[142] = 80'hfffe10a020863ffe1010; 
    result_char[143] = 80'h00001040030200001010;
	
	result_char[144] = 80'h01000800004802002020;//直行或右转
    result_char[145] = 80'h010009fc004402002020;
    result_char[146] = 80'h7ffc1000004002002020;
    result_char[147] = 80'h01002000fffefffefdfc;
    result_char[148] = 80'h1ff04800004004004020;
    result_char[149] = 80'h10100800004004005040;
    result_char[150] = 80'h101013fe3e44080093fe;
    result_char[151] = 80'h1ff0302022440800fc40;
    result_char[152] = 80'h1010502022441ff81080;
    result_char[153] = 80'h1ff090202228280811fc;
    result_char[154] = 80'h101010203e2848081c04;
    result_char[155] = 80'h1ff0102000128808f088;
    result_char[156] = 80'h10101020073208085050;
    result_char[157] = 80'h10101020784a08081020;
    result_char[158] = 80'hfffe10a020860ff81010;
    result_char[159] = 80'h00001040030208081010;
	
	result_char[160] = 80'h10800200088000000000;//停车位
    result_char[161] = 80'h10400200084000000000; 
    result_char[162] = 80'h17fc0200084000000000; 
    result_char[163] = 80'h20007ffc100000000000; 
    result_char[164] = 80'h23f8040017fc00000000; 
    result_char[165] = 80'h62080900300000000000; 
    result_char[166] = 80'h63f81100300800000000; 
    result_char[167] = 80'ha0002100520800000000; 
    result_char[168] = 80'h2ffe3ff8920800000000; 
    result_char[169] = 80'h28020100111000000000; 
    result_char[170] = 80'h23f80100111000000000; 
    result_char[171] = 80'h2040fffe111000000000; 
    result_char[172] = 80'h20400100112000000000; 
    result_char[173] = 80'h20400100102000000000; 
    result_char[174] = 80'h214001001ffe00000000; 
    result_char[175] = 80'h20800100100000000000;
	
	result_char[176] = 80'h01000800020800000000;//人行道
    result_char[177] = 80'h010009fc211000000000;
    result_char[178] = 80'h01001000100000000000;
    result_char[179] = 80'h0100200017fc00000000;
    result_char[180] = 80'h01004800008000000000;
    result_char[181] = 80'h0100080003f800000000;
    result_char[182] = 80'h028013fef20800000000;
    result_char[183] = 80'h0280302013f800000000;
    result_char[184] = 80'h04405020120800000000;
    result_char[185] = 80'h0440902013f800000000;
    result_char[186] = 80'h08201020120800000000;
    result_char[187] = 80'h0820102013f800000000;
    result_char[188] = 80'h10101020120800000000;
    result_char[189] = 80'h20081020280000000000;
    result_char[190] = 80'h400410a047fe00000000;
    result_char[191] = 80'h80021040000000000000;
	
	result_char[192] = 80'h00000100080000200000;//环岛行驶
    result_char[193] = 80'h0000020009fcf8200000; 
    result_char[194] = 80'hfdfe1ff0100008200000; 
    result_char[195] = 80'h10101010200049fc0000; 
    result_char[196] = 80'h10101210480049240000; 
    result_char[197] = 80'h10201150080049240000; 
    result_char[198] = 80'h1020102013fe49240000; 
    result_char[199] = 80'h7c68100030207d240000; 
    result_char[200] = 80'h10a41ffc502005fc0000; 
    result_char[201] = 80'h11220204902004200000; 
    result_char[202] = 80'h1222222410201ca00000; 
    result_char[203] = 80'h102022241020e4600000; 
    result_char[204] = 80'h1c203fe4102044600000; 
    result_char[205] = 80'he0200004102004900000; 
    result_char[206] = 80'h4020002810a029080000; 
    result_char[207] = 80'h00200010104012060000;
     
	result_char[208] = 80'h10200080000000000000;//掉头
    result_char[209] = 80'h10200080000000000000; 
    result_char[210] = 80'h103e0880000000000000; 
    result_char[211] = 80'h10200480000000000000; 
    result_char[212] = 80'hfdfc2480000000000000; 
    result_char[213] = 80'h11041080000000000000; 
    result_char[214] = 80'h15fc1080000000000000; 
    result_char[215] = 80'h19040080000000000000; 
    result_char[216] = 80'h31fcfffe000000000000; 
    result_char[217] = 80'hd1240100000000000000; 
    result_char[218] = 80'h10200140000000000000; 
    result_char[219] = 80'h13fe0220000000000000; 
    result_char[220] = 80'h10200410000000000000; 
    result_char[221] = 80'h10200808000000000000; 
    result_char[222] = 80'h50203004000000000000; 
    result_char[223] = 80'h2020c004000000000000;
	
	result_char[224] = 80'h10100800004000000000;//单行路
    result_char[225] = 80'h082009fc7c4000000000;
    result_char[226] = 80'h04401000447800000000;
    result_char[227] = 80'h3ff82000448800000000;
    result_char[228] = 80'h21084800455000000000;
    result_char[229] = 80'h210808007c2000000000;
    result_char[230] = 80'h3ff813fe105000000000;
    result_char[231] = 80'h21083020108800000000;
    result_char[232] = 80'h21085020110600000000;
    result_char[233] = 80'h3ff890205cf800000000;
    result_char[234] = 80'h01001020508800000000;
    result_char[235] = 80'h01001020508800000000;
    result_char[236] = 80'hfffe1020508800000000;
    result_char[237] = 80'h010010205c8800000000;
    result_char[238] = 80'h010010a0e0f800000000;
    result_char[239] = 80'h01001040008800000000;
	
	result_char[240] = 80'h02000000004000000000;//交叉路口 
    result_char[241] = 80'h01003ff07c4000000000; 
    result_char[242] = 80'h0100101044783ff80000; 
    result_char[243] = 80'hfffe1210448820080000; 
    result_char[244] = 80'h00001120455020080000; 
    result_char[245] = 80'h101009207c2020080000; 
    result_char[246] = 80'h10080820105020080000; 
    result_char[247] = 80'h20240440108820080000; 
    result_char[248] = 80'h48240440110620080000; 
    result_char[249] = 80'h044002805cf820080000; 
    result_char[250] = 80'h02800100508820080000; 
    result_char[251] = 80'h01000280508820080000; 
    result_char[252] = 80'h02800440508820080000; 
    result_char[253] = 80'h0c4008205c883ff80000; 
    result_char[254] = 80'h30303018e0f820080000; 
    result_char[255] = 80'hc00ec006008800000000;
	
	result_char[256] = 80'h00000200020000000000;//双向交通 
    result_char[257] = 80'h00000400010047f80000; 
    result_char[258] = 80'hfdfc0800010020100000; 
    result_char[259] = 80'h04847ffcfffe21a00000; 
    result_char[260] = 80'h44844004000000400000; 
    result_char[261] = 80'h44844004101007fc0000; 
    result_char[262] = 80'h288847c41008e4440000; 
    result_char[263] = 80'h28884444202424440000; 
    result_char[264] = 80'h10504444482427fc0000; 
    result_char[265] = 80'h10504444044024440000; 
    result_char[266] = 80'h28204444028024440000; 
    result_char[267] = 80'h282047c4010027fc0000; 
    result_char[268] = 80'h44504444028024440000; 
    result_char[269] = 80'h448840040c4024540000; 
    result_char[270] = 80'h81044014303054080000; 
    result_char[271] = 80'h02024008c00e8ffe0000;
	 
	result_char[272] = 80'h00800100040000400000;//注意危险
    result_char[273] = 80'h20403ff8040078400000;
    result_char[274] = 80'h100008200ff048a00000;
    result_char[275] = 80'h17fc0440101050a00000;
    result_char[276] = 80'h8040fffe202051100000;
    result_char[277] = 80'h404000005ffc62080000;
    result_char[278] = 80'h40401ff0100055f60000;
    result_char[279] = 80'h1040101013f048000000;
    result_char[280] = 80'h13fc1ff0121048880000;
    result_char[281] = 80'h20401010121048480000;
    result_char[282] = 80'he0401ff012506a480000;
    result_char[283] = 80'h20400200122051500000;
    result_char[284] = 80'h20405104220441100000;
    result_char[285] = 80'h20405112220440200000;
    result_char[286] = 80'h2ffe901241fc47fe0000;
    result_char[287] = 80'h00000ff0800040000000;
	
	result_char[288] = 80'h00800100080001000000;//注意行人 
    result_char[289] = 80'h20403ff809fc01000000; 
    result_char[290] = 80'h10000820100001000000; 
    result_char[291] = 80'h17fc0440200001000000; 
    result_char[292] = 80'h8040fffe480001000000; 
    result_char[293] = 80'h40400000080001000000; 
    result_char[294] = 80'h40401ff013fe02800000; 
    result_char[295] = 80'h10401010302002800000; 
    result_char[296] = 80'h13fc1ff0502004400000; 
    result_char[297] = 80'h20401010902004400000; 
    result_char[298] = 80'he0401ff0102008200000; 
    result_char[299] = 80'h20400200102008200000; 
    result_char[300] = 80'h20405104102010100000; 
    result_char[301] = 80'h20405112102020080000; 
    result_char[302] = 80'h2ffe901210a040040000; 
    result_char[303] = 80'h00000ff0104080020000;
	
	result_char[304] = 80'h00800100084002000000;//注意儿童
    result_char[305] = 80'h20403ff8084001000000;
    result_char[306] = 80'h1000082008403ff80000;
    result_char[307] = 80'h17fc0440084008200000;
    result_char[308] = 80'h8040fffe084004400000;
    result_char[309] = 80'h404000000840fffe0000;
    result_char[310] = 80'h40401ff0084000000000;
    result_char[311] = 80'h1040101008401ff00000;
    result_char[312] = 80'h13fc1ff0084011100000;
    result_char[313] = 80'h2040101008401ff00000;
    result_char[314] = 80'he0401ff0084011100000;
    result_char[315] = 80'h2040020010421ff00000;
    result_char[316] = 80'h20405104104201000000;
    result_char[317] = 80'h2040511220423ff80000;
    result_char[318] = 80'h2ffe9012403e01000000;
    result_char[319] = 80'h00000ff08000fffe0000;
	
	result_char[320] = 80'h00800100004010800000;//注意野生动物
    result_char[321] = 80'h20403ff8004010800000;
    result_char[322] = 80'h100008207c4050800000;
    result_char[323] = 80'h17fc0440004050fc0000;
    result_char[324] = 80'h8040fffe01fc7d540000;
    result_char[325] = 80'h40400000004452540000;
    result_char[326] = 80'h40401ff0fe4490540000;
    result_char[327] = 80'h10401010204410940000;
    result_char[328] = 80'h13fc1ff020441c940000;
    result_char[329] = 80'h204010102084f1240000;
    result_char[330] = 80'he0401ff0488452240000;
    result_char[331] = 80'h20400200448410440000;
    result_char[332] = 80'h20405104fd0410440000;
    result_char[333] = 80'h20405112450410840000;
    result_char[334] = 80'h2ffe9012022811280000;
    result_char[335] = 80'h00000ff0041010100000;
	
	result_char[336] = 80'h00800100102002000000;//注意牲畜 
    result_char[337] = 80'h20403ff8102001000000;
    result_char[338] = 80'h1000082051207ffc0000;
    result_char[339] = 80'h17fc0440512004000000;
    result_char[340] = 80'h8040fffe7dfc08200000;
    result_char[341] = 80'h4040000051201fc00000;
    result_char[342] = 80'h40401ff0922003100000;
    result_char[343] = 80'h1040101010200c080000;
    result_char[344] = 80'h13fc1ff01c203ffc0000;
    result_char[345] = 80'h20401010f1fc00040000;
    result_char[346] = 80'he0401ff050203ff80000;
    result_char[347] = 80'h20400200102021080000;
    result_char[348] = 80'h2040510410203ff80000;
    result_char[349] = 80'h20405112102021080000;
    result_char[350] = 80'h2ffe901213fe3ff80000;
    result_char[351] = 80'h00000ff0100020080000;
	
	result_char[352] = 80'h00800100020008002020;//注意左急转
    result_char[353] = 80'h20403ff802000fe02020; 
    result_char[354] = 80'h10000820020010202020; 
    result_char[355] = 80'h17fc0440fffe2040fdfc; 
    result_char[356] = 80'h8040fffe04005ff84020; 
    result_char[357] = 80'h40400000040000085040; 
    result_char[358] = 80'h40401ff00400000893fe; 
    result_char[359] = 80'h1040101008001ff8fc40; 
    result_char[360] = 80'h13fc1ff00ff800081080; 
    result_char[361] = 80'h204010101080000811fc; 
    result_char[362] = 80'he0401ff010803ff81c04; 
    result_char[363] = 80'h2040020020800200f088; 
    result_char[364] = 80'h20405104408051045050; 
    result_char[365] = 80'h20405112808051121020; 
    result_char[366] = 80'h2ffe90123ffe90121010; 
    result_char[367] = 80'h00000ff000000ff01010;
	
	result_char[368] = 80'h00800100020008002020;//注意右急转
    result_char[369] = 80'h20403ff802000fe02020;
    result_char[370] = 80'h10000820020010202020;
    result_char[371] = 80'h17fc0440fffe2040fdfc;
    result_char[372] = 80'h8040fffe04005ff84020;
    result_char[373] = 80'h40400000040000085040;
    result_char[374] = 80'h40401ff00800000893fe;
    result_char[375] = 80'h1040101008001ff8fc40;
    result_char[376] = 80'h13fc1ff01ff800081080;
    result_char[377] = 80'h204010102808000811fc;
    result_char[378] = 80'he0401ff048083ff81c04;
    result_char[379] = 80'h2040020088080200f088;
    result_char[380] = 80'h20405104080851045050;
    result_char[381] = 80'h20405112080851121020;
    result_char[382] = 80'h2ffe90120ff890121010;
    result_char[383] = 80'h00000ff008080ff01010;
	
	result_char[384] = 80'h08200100002004000000;//禁止驶入 
    result_char[385] = 80'h08200100f82002000000; 
    result_char[386] = 80'h7efc0100082001000000; 
    result_char[387] = 80'h0820010049fc01000000; 
    result_char[388] = 80'h1c701100492401000000; 
    result_char[389] = 80'h2aa81100492402800000; 
    result_char[390] = 80'hc82611f8492402800000; 
    result_char[391] = 80'h000011007d2402800000; 
    result_char[392] = 80'h3ff8110005fc04400000; 
    result_char[393] = 80'h00001100042004400000; 
    result_char[394] = 80'h000011001ca008200000; 
    result_char[395] = 80'hfffe1100e46008200000; 
    result_char[396] = 80'h01001100446010100000; 
    result_char[397] = 80'h11101100049020100000; 
    result_char[398] = 80'h2508fffe290840080000; 
    result_char[399] = 80'h42040000120680060000;
	
	result_char[400] = 80'h08200100020010802040;//禁止车辆停放
    result_char[401] = 80'h08200100020010401040;
    result_char[402] = 80'h7efc0100020017fc0040;
    result_char[403] = 80'h082001007ffc2000fe80;
    result_char[404] = 80'h1c701100040023f820fe;
    result_char[405] = 80'h2aa81100090062082108;
    result_char[406] = 80'hc82611f8110063f83e88;
    result_char[407] = 80'h000011002100a0002488;
    result_char[408] = 80'h3ff811003ff82ffe2488;
    result_char[409] = 80'h00001100010028022450;
    result_char[410] = 80'h00001100010023f82450;
    result_char[411] = 80'hfffe1100fffe20402420;
    result_char[412] = 80'h01001100010020404450;
    result_char[413] = 80'h11101100010020405488;
    result_char[414] = 80'h2508fffe010021408904;
    result_char[415] = 80'h42040000010020800202;
	
	result_char[416] = 80'h20800000000000000000;//施工
    result_char[417] = 80'h10800000000000000000;
    result_char[418] = 80'h10fe7ffc000000000000;
    result_char[419] = 80'h01200100000000000000;
    result_char[420] = 80'hfe200100000000000000;
    result_char[421] = 80'h21200100000000000000;
    result_char[422] = 80'h212c0100000000000000;
    result_char[423] = 80'h3d740100000000000000;
    result_char[424] = 80'h27a40100000000000000;
    result_char[425] = 80'h25240100000000000000;
    result_char[426] = 80'h25340100000000000000;
    result_char[427] = 80'h252a0100000000000000;
    result_char[428] = 80'h25220100000000000000;
    result_char[429] = 80'h4502fffe000000000000;
    result_char[430] = 80'h54fe0000000000000000;
    result_char[431] = 80'h88000000000000000000;
	
	result_char[432] = 80'h10800200004008000000;//停车让行 
    result_char[433] = 80'h10400200204009fc0000; 
    result_char[434] = 80'h17fc0200104010000000; 
    result_char[435] = 80'h20007ffc104020000000; 
    result_char[436] = 80'h23f80400004048000000; 
    result_char[437] = 80'h62080900004008000000; 
    result_char[438] = 80'h63f81100f07c13fe0000; 
    result_char[439] = 80'ha0002100104030200000; 
    result_char[440] = 80'h2ffe3ff8104050200000; 
    result_char[441] = 80'h28020100104090200000; 
    result_char[442] = 80'h23f80100104010200000; 
    result_char[443] = 80'h2040fffe144010200000; 
    result_char[444] = 80'h20400100184010200000; 
    result_char[445] = 80'h20400100104010200000; 
    result_char[446] = 80'h214001000ffe10a00000; 
    result_char[447] = 80'h20800100000010400000;
	
	result_char[448] = 80'h00140040004008000000;//减速让行
    result_char[449] = 80'h40122040204009fc0000; 
    result_char[450] = 80'h201017fc104010000000; 
    result_char[451] = 80'h27fe1040104020000000; 
    result_char[452] = 80'h041003f8004048000000; 
    result_char[453] = 80'h04100248004008000000; 
    result_char[454] = 80'h15d0f248f07c13fe0000; 
    result_char[455] = 80'h141213f8104030200000; 
    result_char[456] = 80'h241210e0104050200000; 
    result_char[457] = 80'he5d41150104090200000; 
    result_char[458] = 80'h25541248104010200000; 
    result_char[459] = 80'h25481444144010200000; 
    result_char[460] = 80'h25da1040184010200000; 
    result_char[461] = 80'h242a2800104010200000; 
    result_char[462] = 80'h284647fe0ffe10a00000; 
    result_char[463] = 80'h10820000000010400000;
	
	result_char[464] = 80'h08200100010008000000;//禁止直行 
    result_char[465] = 80'h08200100010009fc0000; 
    result_char[466] = 80'h7efc01007ffc10000000; 
    result_char[467] = 80'h08200100010020000000; 
    result_char[468] = 80'h1c7011001ff048000000; 
    result_char[469] = 80'h2aa81100101008000000; 
    result_char[470] = 80'hc82611f8101013fe0000; 
    result_char[471] = 80'h000011001ff030200000; 
    result_char[472] = 80'h3ff81100101050200000; 
    result_char[473] = 80'h000011001ff090200000; 
    result_char[474] = 80'h00001100101010200000; 
    result_char[475] = 80'hfffe11001ff010200000; 
    result_char[476] = 80'h01001100101010200000; 
    result_char[477] = 80'h11101100101010200000; 
    result_char[478] = 80'h2508fffefffe10a00000; 
    result_char[479] = 80'h42040000000010400000;
	
	result_char[480] = 80'h08200100020020200000;//禁止右转 
    result_char[481] = 80'h08200100020020200000; 
    result_char[482] = 80'h7efc0100020020200000; 
    result_char[483] = 80'h08200100fffefdfc0000; 
    result_char[484] = 80'h1c701100040040200000; 
    result_char[485] = 80'h2aa81100040050400000; 
    result_char[486] = 80'hc82611f8080093fe0000; 
    result_char[487] = 80'h000011000800fc400000; 
    result_char[488] = 80'h3ff811001ff810800000; 
    result_char[489] = 80'h00001100280811fc0000; 
    result_char[490] = 80'h0000110048081c040000; 
    result_char[491] = 80'hfffe11008808f0880000; 
    result_char[492] = 80'h01001100080850500000; 
    result_char[493] = 80'h11101100080810200000; 
    result_char[494] = 80'h2508fffe0ff810100000; 
    result_char[495] = 80'h42040000080810100000;
	
	result_char[496] = 80'h08200100020002002020;//禁止左右转 
    result_char[497] = 80'h08200100020002002020; 
    result_char[498] = 80'h7efc0100020002002020; 
    result_char[499] = 80'h08200100fffefffefdfc; 
    result_char[500] = 80'h1c701100040004004020; 
    result_char[501] = 80'h2aa81100040004005040; 
    result_char[502] = 80'hc82611f80400080093fe; 
    result_char[503] = 80'h0000110008000800fc40; 
    result_char[504] = 80'h3ff811000ff81ff81080; 
    result_char[505] = 80'h000011001080280811fc; 
    result_char[506] = 80'h00001100108048081c04; 
    result_char[507] = 80'hfffe110020808808f088; 
    result_char[508] = 80'h01001100408008085050; 
    result_char[509] = 80'h11101100808008081020; 
    result_char[510] = 80'h2508fffe3ffe0ff81010; 
    result_char[511] = 80'h42040000000008081010;
	
	result_char[512] = 80'h08200100102000800000;//禁止掉头
    result_char[513] = 80'h08200100102000800000;
    result_char[514] = 80'h7efc0100103e08800000;
    result_char[515] = 80'h08200100102004800000;
    result_char[516] = 80'h1c701100fdfc24800000;
    result_char[517] = 80'h2aa81100110410800000;
    result_char[518] = 80'hc82611f815fc10800000;
    result_char[519] = 80'h00001100190400800000;
    result_char[520] = 80'h3ff8110031fcfffe0000;
    result_char[521] = 80'h00001100d12401000000;
    result_char[522] = 80'h00001100102001400000;
    result_char[523] = 80'hfffe110013fe02200000;
    result_char[524] = 80'h01001100102004100000;
    result_char[525] = 80'h11101100102008080000;
    result_char[526] = 80'h2508fffe502030040000;
    result_char[527] = 80'h420400002020c0040000;
	
	result_char[528] = 80'h08200100002010400000;//禁止鸣笛 
    result_char[529] = 80'h08200100004010400000; 
    result_char[530] = 80'h7efc010079fc3f7e0000; 
    result_char[531] = 80'h08200100490428900000; 
    result_char[532] = 80'h1c701100494445080000; 
    result_char[533] = 80'h2aa81100492481000000; 
    result_char[534] = 80'hc82611f849243ff80000; 
    result_char[535] = 80'h00001100490c21080000; 
    result_char[536] = 80'h3ff81100490021080000; 
    result_char[537] = 80'h0000110049fe21080000; 
    result_char[538] = 80'h0000110078023ff80000; 
    result_char[539] = 80'hfffe1100480221080000; 
    result_char[540] = 80'h0100110003fa21080000; 
    result_char[541] = 80'h11101100000221080000; 
    result_char[542] = 80'h2508fffe00143ff80000; 
    result_char[543] = 80'h42040000000820080000;
	
	result_char[544] = 80'h01000200004008000000;//会车让行 
    result_char[545] = 80'h01000200204009fc0000; 
    result_char[546] = 80'h02800200104010000000; 
    result_char[547] = 80'h04407ffc104020000000; 
    result_char[548] = 80'h08200400004048000000; 
    result_char[549] = 80'h30180900004008000000; 
    result_char[550] = 80'hcfe61100f07c13fe0000; 
    result_char[551] = 80'h00002100104030200000; 
    result_char[552] = 80'h00003ff8104050200000; 
    result_char[553] = 80'h7ffc0100104090200000; 
    result_char[554] = 80'h02000100104010200000; 
    result_char[555] = 80'h0400fffe144010200000; 
    result_char[556] = 80'h08200100184010200000; 
    result_char[557] = 80'h10100100104010200000; 
    result_char[558] = 80'h3ff801000ffe10a00000; 
    result_char[559] = 80'h10080100000010400000;
	
	result_char[560] = 80'h00000040000000000000;//限速70 
    result_char[561] = 80'h7bf82040000000000000; 
    result_char[562] = 80'h4a0817fc000000000000; 
    result_char[563] = 80'h520810407e1800000000; 
    result_char[564] = 80'h53f803f8422400000000; 
    result_char[565] = 80'h62080248044200000000; 
    result_char[566] = 80'h5208f248044200000000; 
    result_char[567] = 80'h4bf813f8084200000000; 
    result_char[568] = 80'h4a4410e0084200000000; 
    result_char[569] = 80'h4a481150104200000000; 
    result_char[570] = 80'h6a301248104200000000; 
    result_char[571] = 80'h52201444104200000000; 
    result_char[572] = 80'h42101040102400000000; 
    result_char[573] = 80'h42882800101800000000; 
    result_char[574] = 80'h430647fe000000000000; 
    result_char[575] = 80'h42000000000000000000;
	
	result_char[576] = 80'h00000040000000000000;//限速80 
    result_char[577] = 80'h7bf82040000000000000; 
    result_char[578] = 80'h4a0817fc000000000000; 
    result_char[579] = 80'h520810403c1800000000; 
    result_char[580] = 80'h53f803f8422400000000; 
    result_char[581] = 80'h62080248424200000000; 
    result_char[582] = 80'h5208f248424200000000; 
    result_char[583] = 80'h4bf813f8244200000000; 
    result_char[584] = 80'h4a4410e0184200000000; 
    result_char[585] = 80'h4a481150244200000000; 
    result_char[586] = 80'h6a301248424200000000; 
    result_char[587] = 80'h52201444424200000000; 
    result_char[588] = 80'h42101040422400000000; 
    result_char[589] = 80'h428828003c1800000000; 
    result_char[590] = 80'h430647fe000000000000; 
    result_char[591] = 80'h42000000000000000000;
	
	result_char[592] = 80'h00000040000000000000;//限速20 
    result_char[593] = 80'h7bf82040000000000000; 
    result_char[594] = 80'h4a0817fc000000000000; 
    result_char[595] = 80'h520810403c1800000000; 
    result_char[596] = 80'h53f803f8422400000000; 
    result_char[597] = 80'h62080248424200000000; 
    result_char[598] = 80'h5208f248424200000000; 
    result_char[599] = 80'h4bf813f8024200000000; 
    result_char[600] = 80'h4a4410e0044200000000; 
    result_char[601] = 80'h4a481150084200000000; 
    result_char[602] = 80'h6a301248104200000000; 
    result_char[603] = 80'h52201444204200000000; 
    result_char[604] = 80'h42101040422400000000; 
    result_char[605] = 80'h428828007e1800000000; 
    result_char[606] = 80'h430647fe000000000000; 
    result_char[607] = 80'h42000000000000000000;
	
	result_char[608] = 80'h08200100020020200000;//禁止左转
    result_char[609] = 80'h08200100020020200000;
    result_char[610] = 80'h7efc0100020020200000;
    result_char[611] = 80'h08200100fffefdfc0000;
    result_char[612] = 80'h1c701100040040200000;
    result_char[613] = 80'h2aa81100040050400000;
    result_char[614] = 80'hc82611f8040093fe0000;
    result_char[615] = 80'h000011000800fc400000;
    result_char[616] = 80'h3ff811000ff810800000;
    result_char[617] = 80'h00001100108011fc0000;
    result_char[618] = 80'h0000110010801c040000;
    result_char[619] = 80'hfffe11002080f0880000;
    result_char[620] = 80'h01001100408050500000;
    result_char[621] = 80'h11101100808010200000;
    result_char[622] = 80'h2508fffe3ffe10100000;
    result_char[623] = 80'h42040000000010100000;
	
end

///////////////////////////
reg [15:0]font[0:229];
always@(*) begin
  // zero_font (索引 0-22)
  font[0]  = 16'h0000;  
  font[1]  = 16'h0FF0;  
  font[2]  = 16'h1FF8;  
  font[3]  = 16'h1E78;  
  font[4]  = 16'h3C3C;  
  font[5]  = 16'h3C3C;  
  font[6]  = 16'h781E;  
  font[7]  = 16'h781E;  
  font[8]  = 16'h781E;  
  font[9]  = 16'h781E;  
  font[10] = 16'h781E;  
  font[11] = 16'h781E;  
  font[12] = 16'h781E;  
  font[13] = 16'h781E;  
  font[14] = 16'h781E;  
  font[15] = 16'h781E;  
  font[16] = 16'h781E;  
  font[17] = 16'h781E;  
  font[18] = 16'h3C3C;  
  font[19] = 16'h3C3C;  
  font[20] = 16'h1E78;  
  font[21] = 16'h1FF8;  
  font[22] = 16'h0FE0;  

  // one_font (索引 23-45)
  font[23]  = 16'h0000;  
  font[24]  = 16'h01C0;  
  font[25]  = 16'h07C0;  
  font[26]  = 16'h1FC0;  
  font[27]  = 16'h03C0;  
  font[28]  = 16'h03C0;  
  font[29]  = 16'h03C0;  
  font[30]  = 16'h03C0;  
  font[31]  = 16'h03C0;  
  font[32]  = 16'h03C0;  
  font[33]  = 16'h03C0;  
  font[34]  = 16'h03C0;  
  font[35]  = 16'h03C0;  
  font[36]  = 16'h03C0;  
  font[37]  = 16'h03C0;  
  font[38]  = 16'h03C0;  
  font[39]  = 16'h03C0;  
  font[40]  = 16'h03C0;  
  font[41]  = 16'h03C0;  
  font[42]  = 16'h03C0;  
  font[43]  = 16'h03C0;  
  font[44]  = 16'h07E0;  
  font[45]  = 16'h1FFC;  

  // two_font (索引 46-68)
  font[46]  = 16'h0000;  
  font[47]  = 16'h0FF0;  
  font[48]  = 16'h1EF8;  
  font[49]  = 16'h383C;  
  font[50]  = 16'h783C;  
  font[51]  = 16'h781C;  
  font[52]  = 16'h7C1C;  
  font[53]  = 16'h7C1C;  
  font[54]  = 16'h383C;  
  font[55]  = 16'h003C;  
  font[56]  = 16'h0078;  
  font[57]  = 16'h0078;  
  font[58]  = 16'h00F0;  
  font[59]  = 16'h01E0;  
  font[60]  = 16'h03C0;  
  font[61]  = 16'h0780;  
  font[62]  = 16'h0F00;  
  font[63]  = 16'h1E0E;  
  font[64]  = 16'h3C0E;  
  font[65]  = 16'h380E;  
  font[66]  = 16'h701C;  
  font[67]  = 16'h7FFC;  
  font[68]  = 16'h7FFC;  

  // three_font (索引 69-91)
  font[69]  = 16'h0000;  
  font[70]  = 16'h0FF0;  
  font[71]  = 16'h3FF8;  
  font[72]  = 16'h3878;  
  font[73]  = 16'h383C;  
  font[74]  = 16'h383C;  
  font[75]  = 16'h383C;  
  font[76]  = 16'h003C;  
  font[77]  = 16'h003C;  
  font[78]  = 16'h0078;  
  font[79]  = 16'h01F0;  
  font[80]  = 16'h07E0;  
  font[81]  = 16'h01F8;  
  font[82]  = 16'h003C;  
  font[83]  = 16'h003C;  
  font[84]  = 16'h001E;  
  font[85]  = 16'h001E;  
  font[86]  = 16'h781E;  
  font[87]  = 16'h781E;  
  font[88]  = 16'h783C;  
  font[89]  = 16'h783C;  
  font[90]  = 16'h3EF8;  
  font[91]  = 16'h1FF0;  

  // four_font (索引 92-114)
  font[92]  = 16'h0000;  
  font[93]  = 16'h0070;  
  font[94]  = 16'h00F0;  
  font[95]  = 16'h00F0;  
  font[96]  = 16'h01F0;  
  font[97]  = 16'h01F0;  
  font[98]  = 16'h03F0;  
  font[99]  = 16'h0770;  
  font[100] = 16'h0770;  
  font[101] = 16'h0E70;  
  font[102] = 16'h1E70;  
  font[103] = 16'h1C70;  
  font[104] = 16'h3870;  
  font[105] = 16'h3870;  
  font[106] = 16'h7070;  
  font[107] = 16'hF070;  
  font[108] = 16'hFFFF;  
  font[109] = 16'h0070;  
  font[110] = 16'h0070;  
  font[111] = 16'h0070;  
  font[112] = 16'h0070;  
  font[113] = 16'h00F8;  
  font[114] = 16'h07FE;  

  // five_font (索引 115-137)
  font[115] = 16'h0000;  
  font[116] = 16'h1FFC;  
  font[117] = 16'h1FFC;  
  font[118] = 16'h3800;  
  font[119] = 16'h3800;  
  font[120] = 16'h3800;  
  font[121] = 16'h3800;  
  font[122] = 16'h3800;  
  font[123] = 16'h39C0;  
  font[124] = 16'h3FF8;  
  font[125] = 16'h3FF8;  
  font[126] = 16'h3C3C;  
  font[127] = 16'h383C;  
  font[128] = 16'h001E;  
  font[129] = 16'h001E;  
  font[130] = 16'h001E;  
  font[131] = 16'h381E;  
  font[132] = 16'h781E;  
  font[133] = 16'h781C;  
  font[134] = 16'h783C;  
  font[135] = 16'h783C;  
  font[136] = 16'h3EF8;  
  font[137] = 16'h0FF0;  

  // six_font (索引 138-160)
  font[138] = 16'h0000;  
  font[139] = 16'h07F8;  
  font[140] = 16'h0FFC;  
  font[141] = 16'h1E3C;  
  font[142] = 16'h3C3C;  
  font[143] = 16'h3838;  
  font[144] = 16'h3800;  
  font[145] = 16'h7800;  
  font[146] = 16'h7800;  
  font[147] = 16'h7FF8;  
  font[148] = 16'h7FFC;  
  font[149] = 16'h7E3C;  
  font[150] = 16'h7C1E;  
  font[151] = 16'h781E;  
  font[152] = 16'h781E;  
  font[153] = 16'h781E;  
  font[154] = 16'h781E;  
  font[155] = 16'h781E;  
  font[156] = 16'h781E;  
  font[157] = 16'h3C1E;  
  font[158] = 16'h3E3C;  
  font[159] = 16'h1F78;  
  font[160] = 16'h0FF0;  

  // seven_font (索引 161-183)
  font[161] = 16'h0000;  
  font[162] = 16'h3FFE;  
  font[163] = 16'h3FFE;  
  font[164] = 16'h381C;  
  font[165] = 16'h701C;  
  font[166] = 16'h7038;  
  font[167] = 16'h7078;  
  font[168] = 16'h0070;  
  font[169] = 16'h00F0;  
  font[170] = 16'h00E0;  
  font[171] = 16'h00E0;  
  font[172] = 16'h01C0;  
  font[173] = 16'h01C0;  
  font[174] = 16'h03C0;  
  font[175] = 16'h03C0;  
  font[176] = 16'h0380;  
  font[177] = 16'h0780;  
  font[178] = 16'h0780;  
  font[179] = 16'h0780;  
  font[180] = 16'h0780;  
  font[181] = 16'h0780;  
  font[182] = 16'h0780;  
  font[183] = 16'h0780;  

  // eight_font (索引 184-206)
  font[184] = 16'h0000;  
  font[185] = 16'h0FF0;  
  font[186] = 16'h3FF8;  
  font[187] = 16'h383C;  
  font[188] = 16'h781C;  
  font[189] = 16'h701E;  
  font[190] = 16'h781E;  
  font[191] = 16'h781E;  
  font[192] = 16'h7C1C;  
  font[193] = 16'h3F3C;  
  font[194] = 16'h1FF8;  
  font[195] = 16'h0FF0;  
  font[196] = 16'h1FF8;  
  font[197] = 16'h3CFC;  
  font[198] = 16'h783C;  
  font[199] = 16'h701E;  
  font[200] = 16'h701E;  
  font[201] = 16'h701E;  
  font[202] = 16'h701E;  
  font[203] = 16'h701E;  
  font[204] = 16'h783C;  
  font[205] = 16'h3EF8;  
  font[206] = 16'h0FF0;  

  // nine_font (索引 207-229)
  font[207] = 16'h0000;  
  font[208] = 16'h0FF0;  
  font[209] = 16'h3EF8;  
  font[210] = 16'h3C3C;  
  font[211] = 16'h783C;  
  font[212] = 16'h781C;  
  font[213] = 16'h701E;  
  font[214] = 16'h701E;  
  font[215] = 16'h701E;  
  font[216] = 16'h701E;  
  font[217] = 16'h783E;  
  font[218] = 16'h783E;  
  font[219] = 16'h7C7E;  
  font[220] = 16'h3FFE;  
  font[221] = 16'h1FFE;  
  font[222] = 16'h001E;  
  font[223] = 16'h003C;  
  font[224] = 16'h003C;  
  font[225] = 16'h183C;  
  font[226] = 16'h3C78;  
  font[227] = 16'h3C78;  
  font[228] = 16'h3FF0;  
  font[229] = 16'h1FE0;  
end
/////////fps计算///////////////
wire [8:0] fps;
wire [3:0] fps_unit;
wire [3:0] fps_decade;
wire [3:0] fps_hundred;
wire fps_vs;
wire fps_hs;
wire fps_de;
fps_count fps_count(
    .pixelclk(clk_pixel),    
    .clk(sys_clk),
    .rst_n(rst_n),
    .vsync(i_vs),
    .fps(fps)
);   
fps_calculation fps_calculation(
    .fps(fps),
    .fps_unit(fps_unit),
    .fps_decade(fps_decade),
    .fps_hundred(fps_hundred)
);
wire in_hundred_area = (h_cnt < 16) && (v_cnt < 23);
wire in_decade_area = (h_cnt >= 16 && h_cnt < 32) && (v_cnt < 23);
wire in_unit_area = (h_cnt >= 32 && h_cnt < 48) && (v_cnt < 23);

wire pixel_fps_hundred= (in_hundred_area == 1)?font[v_cnt + 23*fps_hundred][15 - h_cnt]:0; //百位
wire pixel_fps_decade = (in_decade_area == 1)?font[v_cnt + 23*fps_decade][31-h_cnt]:0; //十位
wire pixel_fps_unit = (in_unit_area == 1)?font[v_cnt + 23*fps_unit][47-h_cnt]:0;  //个位
wire pixel_fps = pixel_fps_hundred || pixel_fps_decade || pixel_fps_unit;

/////////////////////digit_recognition_result///////////////
reg [3:0]tens;reg [3:0]ones;reg digit_en,result_en;
wire is_tens_area = (h_cnt >= 607 && h_cnt <623) && (v_cnt>= 0 && v_cnt< 23);
wire is_ones_area = (h_cnt >= 624 && h_cnt <640 ) && (v_cnt >= 0 && v_cnt< 23);
always@(posedge clk_pixel or negedge rst_n)begin
	if(!rst_n)begin
		tens <= 4'd0;
		ones <= 4'd0;
		digit_en <= 1'd0;
		result_en <= 1'd0;
	end
	else begin
		case(char_en)
			16'd0:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=0;end
			16'd1:begin tens <= 4'd1; ones <= 4'd5;digit_en <= 1;result_en<=1;end
			16'd2:begin tens <= 4'd3; ones <= 4'd0;digit_en <= 1;result_en<=1;end
			16'd3:begin tens <= 4'd4; ones <= 4'd0;digit_en <= 1;result_en<=1;end
			16'd4:begin tens <= 4'd5; ones <= 4'd0;digit_en <= 1;result_en<=1;end
			16'd5:begin tens <= 4'd6; ones <= 4'd0;digit_en <= 1;result_en<=1;end
			
			16'd6:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd7:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd8:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd9:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd10:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd11:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd12:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd13:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd14:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd15:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd16:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd17:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd18:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd19:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd20:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd21:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd22:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd23:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd24:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd25:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd26:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd27:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd28:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd29:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd30:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd31:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd32:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd33:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd34:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd35:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			16'd36:begin tens <= 4'd7; ones <= 4'd0;digit_en <= 1;result_en<=1;end
			16'd37:begin tens <= 4'd8; ones <= 4'd0;digit_en <= 1;result_en<=1;end
			16'd38:begin tens <= 4'd2; ones <= 4'd0;digit_en <= 1;result_en<=1;end
			16'd39:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 0;result_en<=1;end
			
			default:begin tens <= 4'd0; ones <= 4'd0;digit_en <= 1'd0;result_en<=1'd0;end
		endcase
	end
end
wire pixel_digit_tens = (is_tens_area == 1 && digit_en == 1)?font[v_cnt + 23*tens][622 - h_cnt]:0;
wire pixel_digit_ones = (is_ones_area == 1 && digit_en == 1)?font[v_cnt + 23*ones][639 - h_cnt]:0;
wire pixel_digit = pixel_digit_tens || pixel_digit_ones;

////////////////////////////recognition_result////////////////////////////
wire [10:0] col_right = col_left + 80;
wire [10:0] row_high = row_low - 17;
wire is_result_area =   (h_cnt >= col_left && h_cnt < col_right) && (v_cnt >= row_high && v_cnt < (row_low - 1));
wire [79:0]pixel_v = (is_result_area == 1 && result_en == 1)?result_char[v_cnt-row_high + 16*(char_en-1)]:64'd0;
wire pixel_h = (is_result_area == 1 && result_en == 1)?pixel_v[col_right-h_cnt]:0;
 
///////////////////char_choose_output//////////////////////////
reg pixel_fps_reg, pixel_digit_reg, pixel_result_reg;
always @(posedge clk_pixel) begin
    pixel_fps_reg <= pixel_fps;
    pixel_digit_reg <= pixel_digit;
    pixel_result_reg <= pixel_h;
end

always @(posedge clk_pixel) begin
    pixel_rgb_reg <= (pixel_fps_reg|| pixel_digit_reg ||pixel_result_reg) ? 24'h00ff00 : rgb;
end
//////////////timing////////
always@(posedge clk_pixel or negedge rst_n)begin
    if(!rst_n)begin
        vs_d0 <= 0;
        hs_d0 <= 0;
        de_d0 <= 0;
    end
    else begin
        vs_d0 <= i_vs;
        hs_d0 <= i_hs;
        de_d0 <= i_de;
    end
end
endmodule