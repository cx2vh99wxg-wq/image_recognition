// 2026-11-03 注释校订。根据帧率和识别编号绘制字形。保持原字形表触发条件，绿字在掩码登记后叠加。
module char_display (
    input wire clk_pixel,
    input wire sys_clk,
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

// 字形表保留原常量过程的事件触发方式。
reg [79:0]result_glyph_rows[0:623];
always @(*) begin
    {result_glyph_rows[0], result_glyph_rows[1], result_glyph_rows[2], result_glyph_rows[3],
     result_glyph_rows[4], result_glyph_rows[5], result_glyph_rows[6], result_glyph_rows[7],
     result_glyph_rows[8], result_glyph_rows[9], result_glyph_rows[10], result_glyph_rows[11],
     result_glyph_rows[12], result_glyph_rows[13], result_glyph_rows[14], result_glyph_rows[15]} =
        1280'h00000040000000000000_7BF82040000000000000_4A0817FC000000000000_52081040087E00000000_53F803F8384000000000_62080248084000000000_5208F248084000000000_4BF813F8087800000000_4A4410E0084400000000_4A481150080200000000_6A301248080200000000_52201444084200000000_42101040084400000000_428828003E3800000000_430647FE000000000000_42000000000000000000;
    {result_glyph_rows[16], result_glyph_rows[17], result_glyph_rows[18], result_glyph_rows[19],
     result_glyph_rows[20], result_glyph_rows[21], result_glyph_rows[22], result_glyph_rows[23],
     result_glyph_rows[24], result_glyph_rows[25], result_glyph_rows[26], result_glyph_rows[27],
     result_glyph_rows[28], result_glyph_rows[29], result_glyph_rows[30], result_glyph_rows[31]} =
        1280'h00000040000000000000_7BF82040000000000000_4A0817FC000000000000_520810403C1800000000_53F803F8422400000000_62080248424200000000_5208F248024200000000_4BF813F8044200000000_4A4410E0184200000000_4A481150044200000000_6A301248024200000000_52201444424200000000_42101040422400000000_428828003C1800000000_430647FE000000000000_42000000000000000000;
    {result_glyph_rows[32], result_glyph_rows[33], result_glyph_rows[34], result_glyph_rows[35],
     result_glyph_rows[36], result_glyph_rows[37], result_glyph_rows[38], result_glyph_rows[39],
     result_glyph_rows[40], result_glyph_rows[41], result_glyph_rows[42], result_glyph_rows[43],
     result_glyph_rows[44], result_glyph_rows[45], result_glyph_rows[46], result_glyph_rows[47]} =
        1280'h00000040000000000000_7BF82040000000000000_4A0817FC000000000000_52081040041800000000_53F803F80C2400000000_620802480C4200000000_5208F248144200000000_4BF813F8244200000000_4A4410E0244200000000_4A481150444200000000_6A3012487F4200000000_52201444044200000000_42101040042400000000_428828001F1800000000_430647FE000000000000_42000000000000000000;
    {result_glyph_rows[48], result_glyph_rows[49], result_glyph_rows[50], result_glyph_rows[51],
     result_glyph_rows[52], result_glyph_rows[53], result_glyph_rows[54], result_glyph_rows[55],
     result_glyph_rows[56], result_glyph_rows[57], result_glyph_rows[58], result_glyph_rows[59],
     result_glyph_rows[60], result_glyph_rows[61], result_glyph_rows[62], result_glyph_rows[63]} =
        1280'h00000040000000000000_7BF82040000000000000_4A0817FC000000000000_520810407E1800000000_53F803F8402400000000_62080248404200000000_5208F248404200000000_4BF813F8784200000000_4A4410E0444200000000_4A481150024200000000_6A301248024200000000_52201444424200000000_42101040442400000000_42882800381800000000_430647FE000000000000_42000000000000000000;
    {result_glyph_rows[64], result_glyph_rows[65], result_glyph_rows[66], result_glyph_rows[67],
     result_glyph_rows[68], result_glyph_rows[69], result_glyph_rows[70], result_glyph_rows[71],
     result_glyph_rows[72], result_glyph_rows[73], result_glyph_rows[74], result_glyph_rows[75],
     result_glyph_rows[76], result_glyph_rows[77], result_glyph_rows[78], result_glyph_rows[79]} =
        1280'h00000040000000000000_7BF82040000000000000_4A0817FC000000000000_52081040181800000000_53F803F8242400000000_62080248404200000000_5208F248404200000000_4BF813F85C4200000000_4A4410E0624200000000_4A481150424200000000_6A301248424200000000_52201444424200000000_42101040222400000000_428828001C1800000000_430647FE000000000000_42000000000000000000;
    {result_glyph_rows[80], result_glyph_rows[81], result_glyph_rows[82], result_glyph_rows[83],
     result_glyph_rows[84], result_glyph_rows[85], result_glyph_rows[86], result_glyph_rows[87],
     result_glyph_rows[88], result_glyph_rows[89], result_glyph_rows[90], result_glyph_rows[91],
     result_glyph_rows[92], result_glyph_rows[93], result_glyph_rows[94], result_glyph_rows[95]} =
        1280'h01000800000000000000_010009FC000000000000_7FFC1000000000000000_01002000000000000000_1FF04800000000000000_10100800000000000000_101013FE000000000000_1FF03020000000000000_10105020000000000000_1FF09020000000000000_10101020000000000000_1FF01020000000000000_10101020000000000000_10101020000000000000_FFFE10A0000000000000_00001040000000000000;
    {result_glyph_rows[96], result_glyph_rows[97], result_glyph_rows[98], result_glyph_rows[99],
     result_glyph_rows[100], result_glyph_rows[101], result_glyph_rows[102], result_glyph_rows[103],
     result_glyph_rows[104], result_glyph_rows[105], result_glyph_rows[106], result_glyph_rows[107],
     result_glyph_rows[108], result_glyph_rows[109], result_glyph_rows[110], result_glyph_rows[111]} =
        1280'h02002020000000000000_02002020000000000000_02002020000000000000_FFFEFDFC000000000000_04004020000000000000_04005040000000000000_040093FE000000000000_0800FC40000000000000_0FF81080000000000000_108011FC000000000000_10801C04000000000000_2080F088000000000000_40805050000000000000_80801020000000000000_3FFE1010000000000000_00001010000000000000;
    {result_glyph_rows[112], result_glyph_rows[113], result_glyph_rows[114], result_glyph_rows[115],
     result_glyph_rows[116], result_glyph_rows[117], result_glyph_rows[118], result_glyph_rows[119],
     result_glyph_rows[120], result_glyph_rows[121], result_glyph_rows[122], result_glyph_rows[123],
     result_glyph_rows[124], result_glyph_rows[125], result_glyph_rows[126], result_glyph_rows[127]} =
        1280'h02002020000000000000_02002020000000000000_02002020000000000000_FFFEFDFC000000000000_04004020000000000000_04005040000000000000_080093FE000000000000_0800FC40000000000000_1FF81080000000000000_280811FC000000000000_48081C04000000000000_8808F088000000000000_08085050000000000000_08081020000000000000_0FF81010000000000000_08081010000000000000;
    {result_glyph_rows[128], result_glyph_rows[129], result_glyph_rows[130], result_glyph_rows[131],
     result_glyph_rows[132], result_glyph_rows[133], result_glyph_rows[134], result_glyph_rows[135],
     result_glyph_rows[136], result_glyph_rows[137], result_glyph_rows[138], result_glyph_rows[139],
     result_glyph_rows[140], result_glyph_rows[141], result_glyph_rows[142], result_glyph_rows[143]} =
        1280'h01000800004802002020_010009FC004402002020_7FFC1000004002002020_01002000FFFEFFFEFDFC_1FF04800004004004020_10100800004004005040_101013FE3E44040093FE_1FF0302022440800FC40_1010502022440FF81080_1FF090202228108011FC_101010203E2810801C04_1FF0102000122080F088_10101020073240805050_10101020784A80801020_FFFE10A020863FFE1010_00001040030200001010;
    {result_glyph_rows[144], result_glyph_rows[145], result_glyph_rows[146], result_glyph_rows[147],
     result_glyph_rows[148], result_glyph_rows[149], result_glyph_rows[150], result_glyph_rows[151],
     result_glyph_rows[152], result_glyph_rows[153], result_glyph_rows[154], result_glyph_rows[155],
     result_glyph_rows[156], result_glyph_rows[157], result_glyph_rows[158], result_glyph_rows[159]} =
        1280'h01000800004802002020_010009FC004402002020_7FFC1000004002002020_01002000FFFEFFFEFDFC_1FF04800004004004020_10100800004004005040_101013FE3E44080093FE_1FF0302022440800FC40_1010502022441FF81080_1FF090202228280811FC_101010203E2848081C04_1FF0102000128808F088_10101020073208085050_10101020784A08081020_FFFE10A020860FF81010_00001040030208081010;
    {result_glyph_rows[160], result_glyph_rows[161], result_glyph_rows[162], result_glyph_rows[163],
     result_glyph_rows[164], result_glyph_rows[165], result_glyph_rows[166], result_glyph_rows[167],
     result_glyph_rows[168], result_glyph_rows[169], result_glyph_rows[170], result_glyph_rows[171],
     result_glyph_rows[172], result_glyph_rows[173], result_glyph_rows[174], result_glyph_rows[175]} =
        1280'h10800200088000000000_10400200084000000000_17FC0200084000000000_20007FFC100000000000_23F8040017FC00000000_62080900300000000000_63F81100300800000000_A0002100520800000000_2FFE3FF8920800000000_28020100111000000000_23F80100111000000000_2040FFFE111000000000_20400100112000000000_20400100102000000000_214001001FFE00000000_20800100100000000000;
    {result_glyph_rows[176], result_glyph_rows[177], result_glyph_rows[178], result_glyph_rows[179],
     result_glyph_rows[180], result_glyph_rows[181], result_glyph_rows[182], result_glyph_rows[183],
     result_glyph_rows[184], result_glyph_rows[185], result_glyph_rows[186], result_glyph_rows[187],
     result_glyph_rows[188], result_glyph_rows[189], result_glyph_rows[190], result_glyph_rows[191]} =
        1280'h01000800020800000000_010009FC211000000000_01001000100000000000_0100200017FC00000000_01004800008000000000_0100080003F800000000_028013FEF20800000000_0280302013F800000000_04405020120800000000_0440902013F800000000_08201020120800000000_0820102013F800000000_10101020120800000000_20081020280000000000_400410A047FE00000000_80021040000000000000;
    {result_glyph_rows[192], result_glyph_rows[193], result_glyph_rows[194], result_glyph_rows[195],
     result_glyph_rows[196], result_glyph_rows[197], result_glyph_rows[198], result_glyph_rows[199],
     result_glyph_rows[200], result_glyph_rows[201], result_glyph_rows[202], result_glyph_rows[203],
     result_glyph_rows[204], result_glyph_rows[205], result_glyph_rows[206], result_glyph_rows[207]} =
        1280'h00000100080000200000_0000020009FCF8200000_FDFE1FF0100008200000_10101010200049FC0000_10101210480049240000_10201150080049240000_1020102013FE49240000_7C68100030207D240000_10A41FFC502005FC0000_11220204902004200000_1222222410201CA00000_102022241020E4600000_1C203FE4102044600000_E0200004102004900000_4020002810A029080000_00200010104012060000;
    {result_glyph_rows[208], result_glyph_rows[209], result_glyph_rows[210], result_glyph_rows[211],
     result_glyph_rows[212], result_glyph_rows[213], result_glyph_rows[214], result_glyph_rows[215],
     result_glyph_rows[216], result_glyph_rows[217], result_glyph_rows[218], result_glyph_rows[219],
     result_glyph_rows[220], result_glyph_rows[221], result_glyph_rows[222], result_glyph_rows[223]} =
        1280'h10200080000000000000_10200080000000000000_103E0880000000000000_10200480000000000000_FDFC2480000000000000_11041080000000000000_15FC1080000000000000_19040080000000000000_31FCFFFE000000000000_D1240100000000000000_10200140000000000000_13FE0220000000000000_10200410000000000000_10200808000000000000_50203004000000000000_2020C004000000000000;
    {result_glyph_rows[224], result_glyph_rows[225], result_glyph_rows[226], result_glyph_rows[227],
     result_glyph_rows[228], result_glyph_rows[229], result_glyph_rows[230], result_glyph_rows[231],
     result_glyph_rows[232], result_glyph_rows[233], result_glyph_rows[234], result_glyph_rows[235],
     result_glyph_rows[236], result_glyph_rows[237], result_glyph_rows[238], result_glyph_rows[239]} =
        1280'h10100800004000000000_082009FC7C4000000000_04401000447800000000_3FF82000448800000000_21084800455000000000_210808007C2000000000_3FF813FE105000000000_21083020108800000000_21085020110600000000_3FF890205CF800000000_01001020508800000000_01001020508800000000_FFFE1020508800000000_010010205C8800000000_010010A0E0F800000000_01001040008800000000;
    {result_glyph_rows[240], result_glyph_rows[241], result_glyph_rows[242], result_glyph_rows[243],
     result_glyph_rows[244], result_glyph_rows[245], result_glyph_rows[246], result_glyph_rows[247],
     result_glyph_rows[248], result_glyph_rows[249], result_glyph_rows[250], result_glyph_rows[251],
     result_glyph_rows[252], result_glyph_rows[253], result_glyph_rows[254], result_glyph_rows[255]} =
        1280'h02000000004000000000_01003FF07C4000000000_0100101044783FF80000_FFFE1210448820080000_00001120455020080000_101009207C2020080000_10080820105020080000_20240440108820080000_48240440110620080000_044002805CF820080000_02800100508820080000_01000280508820080000_02800440508820080000_0C4008205C883FF80000_30303018E0F820080000_C00EC006008800000000;
    {result_glyph_rows[256], result_glyph_rows[257], result_glyph_rows[258], result_glyph_rows[259],
     result_glyph_rows[260], result_glyph_rows[261], result_glyph_rows[262], result_glyph_rows[263],
     result_glyph_rows[264], result_glyph_rows[265], result_glyph_rows[266], result_glyph_rows[267],
     result_glyph_rows[268], result_glyph_rows[269], result_glyph_rows[270], result_glyph_rows[271]} =
        1280'h00000200020000000000_00000400010047F80000_FDFC0800010020100000_04847FFCFFFE21A00000_44844004000000400000_44844004101007FC0000_288847C41008E4440000_28884444202424440000_10504444482427FC0000_10504444044024440000_28204444028024440000_282047C4010027FC0000_44504444028024440000_448840040C4024540000_81044014303054080000_02024008C00E8FFE0000;
    {result_glyph_rows[272], result_glyph_rows[273], result_glyph_rows[274], result_glyph_rows[275],
     result_glyph_rows[276], result_glyph_rows[277], result_glyph_rows[278], result_glyph_rows[279],
     result_glyph_rows[280], result_glyph_rows[281], result_glyph_rows[282], result_glyph_rows[283],
     result_glyph_rows[284], result_glyph_rows[285], result_glyph_rows[286], result_glyph_rows[287]} =
        1280'h00800100040000400000_20403FF8040078400000_100008200FF048A00000_17FC0440101050A00000_8040FFFE202051100000_404000005FFC62080000_40401FF0100055F60000_1040101013F048000000_13FC1FF0121048880000_20401010121048480000_E0401FF012506A480000_20400200122051500000_20405104220441100000_20405112220440200000_2FFE901241FC47FE0000_00000FF0800040000000;
    {result_glyph_rows[288], result_glyph_rows[289], result_glyph_rows[290], result_glyph_rows[291],
     result_glyph_rows[292], result_glyph_rows[293], result_glyph_rows[294], result_glyph_rows[295],
     result_glyph_rows[296], result_glyph_rows[297], result_glyph_rows[298], result_glyph_rows[299],
     result_glyph_rows[300], result_glyph_rows[301], result_glyph_rows[302], result_glyph_rows[303]} =
        1280'h00800100080001000000_20403FF809FC01000000_10000820100001000000_17FC0440200001000000_8040FFFE480001000000_40400000080001000000_40401FF013FE02800000_10401010302002800000_13FC1FF0502004400000_20401010902004400000_E0401FF0102008200000_20400200102008200000_20405104102010100000_20405112102020080000_2FFE901210A040040000_00000FF0104080020000;
    {result_glyph_rows[304], result_glyph_rows[305], result_glyph_rows[306], result_glyph_rows[307],
     result_glyph_rows[308], result_glyph_rows[309], result_glyph_rows[310], result_glyph_rows[311],
     result_glyph_rows[312], result_glyph_rows[313], result_glyph_rows[314], result_glyph_rows[315],
     result_glyph_rows[316], result_glyph_rows[317], result_glyph_rows[318], result_glyph_rows[319]} =
        1280'h00800100084002000000_20403FF8084001000000_1000082008403FF80000_17FC0440084008200000_8040FFFE084004400000_404000000840FFFE0000_40401FF0084000000000_1040101008401FF00000_13FC1FF0084011100000_2040101008401FF00000_E0401FF0084011100000_2040020010421FF00000_20405104104201000000_2040511220423FF80000_2FFE9012403E01000000_00000FF08000FFFE0000;
    {result_glyph_rows[320], result_glyph_rows[321], result_glyph_rows[322], result_glyph_rows[323],
     result_glyph_rows[324], result_glyph_rows[325], result_glyph_rows[326], result_glyph_rows[327],
     result_glyph_rows[328], result_glyph_rows[329], result_glyph_rows[330], result_glyph_rows[331],
     result_glyph_rows[332], result_glyph_rows[333], result_glyph_rows[334], result_glyph_rows[335]} =
        1280'h00800100004010800000_20403FF8004010800000_100008207C4050800000_17FC0440004050FC0000_8040FFFE01FC7D540000_40400000004452540000_40401FF0FE4490540000_10401010204410940000_13FC1FF020441C940000_204010102084F1240000_E0401FF0488452240000_20400200448410440000_20405104FD0410440000_20405112450410840000_2FFE9012022811280000_00000FF0041010100000;
    {result_glyph_rows[336], result_glyph_rows[337], result_glyph_rows[338], result_glyph_rows[339],
     result_glyph_rows[340], result_glyph_rows[341], result_glyph_rows[342], result_glyph_rows[343],
     result_glyph_rows[344], result_glyph_rows[345], result_glyph_rows[346], result_glyph_rows[347],
     result_glyph_rows[348], result_glyph_rows[349], result_glyph_rows[350], result_glyph_rows[351]} =
        1280'h00800100102002000000_20403FF8102001000000_1000082051207FFC0000_17FC0440512004000000_8040FFFE7DFC08200000_4040000051201FC00000_40401FF0922003100000_1040101010200C080000_13FC1FF01C203FFC0000_20401010F1FC00040000_E0401FF050203FF80000_20400200102021080000_2040510410203FF80000_20405112102021080000_2FFE901213FE3FF80000_00000FF0100020080000;
    {result_glyph_rows[352], result_glyph_rows[353], result_glyph_rows[354], result_glyph_rows[355],
     result_glyph_rows[356], result_glyph_rows[357], result_glyph_rows[358], result_glyph_rows[359],
     result_glyph_rows[360], result_glyph_rows[361], result_glyph_rows[362], result_glyph_rows[363],
     result_glyph_rows[364], result_glyph_rows[365], result_glyph_rows[366], result_glyph_rows[367]} =
        1280'h00800100020008002020_20403FF802000FE02020_10000820020010202020_17FC0440FFFE2040FDFC_8040FFFE04005FF84020_40400000040000085040_40401FF00400000893FE_1040101008001FF8FC40_13FC1FF00FF800081080_204010101080000811FC_E0401FF010803FF81C04_2040020020800200F088_20405104408051045050_20405112808051121020_2FFE90123FFE90121010_00000FF000000FF01010;
    {result_glyph_rows[368], result_glyph_rows[369], result_glyph_rows[370], result_glyph_rows[371],
     result_glyph_rows[372], result_glyph_rows[373], result_glyph_rows[374], result_glyph_rows[375],
     result_glyph_rows[376], result_glyph_rows[377], result_glyph_rows[378], result_glyph_rows[379],
     result_glyph_rows[380], result_glyph_rows[381], result_glyph_rows[382], result_glyph_rows[383]} =
        1280'h00800100020008002020_20403FF802000FE02020_10000820020010202020_17FC0440FFFE2040FDFC_8040FFFE04005FF84020_40400000040000085040_40401FF00800000893FE_1040101008001FF8FC40_13FC1FF01FF800081080_204010102808000811FC_E0401FF048083FF81C04_2040020088080200F088_20405104080851045050_20405112080851121020_2FFE90120FF890121010_00000FF008080FF01010;
    {result_glyph_rows[384], result_glyph_rows[385], result_glyph_rows[386], result_glyph_rows[387],
     result_glyph_rows[388], result_glyph_rows[389], result_glyph_rows[390], result_glyph_rows[391],
     result_glyph_rows[392], result_glyph_rows[393], result_glyph_rows[394], result_glyph_rows[395],
     result_glyph_rows[396], result_glyph_rows[397], result_glyph_rows[398], result_glyph_rows[399]} =
        1280'h08200100002004000000_08200100F82002000000_7EFC0100082001000000_0820010049FC01000000_1C701100492401000000_2AA81100492402800000_C82611F8492402800000_000011007D2402800000_3FF8110005FC04400000_00001100042004400000_000011001CA008200000_FFFE1100E46008200000_01001100446010100000_11101100049020100000_2508FFFE290840080000_42040000120680060000;
    {result_glyph_rows[400], result_glyph_rows[401], result_glyph_rows[402], result_glyph_rows[403],
     result_glyph_rows[404], result_glyph_rows[405], result_glyph_rows[406], result_glyph_rows[407],
     result_glyph_rows[408], result_glyph_rows[409], result_glyph_rows[410], result_glyph_rows[411],
     result_glyph_rows[412], result_glyph_rows[413], result_glyph_rows[414], result_glyph_rows[415]} =
        1280'h08200100020010802040_08200100020010401040_7EFC0100020017FC0040_082001007FFC2000FE80_1C701100040023F820FE_2AA81100090062082108_C82611F8110063F83E88_000011002100A0002488_3FF811003FF82FFE2488_00001100010028022450_00001100010023F82450_FFFE1100FFFE20402420_01001100010020404450_11101100010020405488_2508FFFE010021408904_42040000010020800202;
    {result_glyph_rows[416], result_glyph_rows[417], result_glyph_rows[418], result_glyph_rows[419],
     result_glyph_rows[420], result_glyph_rows[421], result_glyph_rows[422], result_glyph_rows[423],
     result_glyph_rows[424], result_glyph_rows[425], result_glyph_rows[426], result_glyph_rows[427],
     result_glyph_rows[428], result_glyph_rows[429], result_glyph_rows[430], result_glyph_rows[431]} =
        1280'h20800000000000000000_10800000000000000000_10FE7FFC000000000000_01200100000000000000_FE200100000000000000_21200100000000000000_212C0100000000000000_3D740100000000000000_27A40100000000000000_25240100000000000000_25340100000000000000_252A0100000000000000_25220100000000000000_4502FFFE000000000000_54FE0000000000000000_88000000000000000000;
    {result_glyph_rows[432], result_glyph_rows[433], result_glyph_rows[434], result_glyph_rows[435],
     result_glyph_rows[436], result_glyph_rows[437], result_glyph_rows[438], result_glyph_rows[439],
     result_glyph_rows[440], result_glyph_rows[441], result_glyph_rows[442], result_glyph_rows[443],
     result_glyph_rows[444], result_glyph_rows[445], result_glyph_rows[446], result_glyph_rows[447]} =
        1280'h10800200004008000000_10400200204009FC0000_17FC0200104010000000_20007FFC104020000000_23F80400004048000000_62080900004008000000_63F81100F07C13FE0000_A0002100104030200000_2FFE3FF8104050200000_28020100104090200000_23F80100104010200000_2040FFFE144010200000_20400100184010200000_20400100104010200000_214001000FFE10A00000_20800100000010400000;
    {result_glyph_rows[448], result_glyph_rows[449], result_glyph_rows[450], result_glyph_rows[451],
     result_glyph_rows[452], result_glyph_rows[453], result_glyph_rows[454], result_glyph_rows[455],
     result_glyph_rows[456], result_glyph_rows[457], result_glyph_rows[458], result_glyph_rows[459],
     result_glyph_rows[460], result_glyph_rows[461], result_glyph_rows[462], result_glyph_rows[463]} =
        1280'h00140040004008000000_40122040204009FC0000_201017FC104010000000_27FE1040104020000000_041003F8004048000000_04100248004008000000_15D0F248F07C13FE0000_141213F8104030200000_241210E0104050200000_E5D41150104090200000_25541248104010200000_25481444144010200000_25DA1040184010200000_242A2800104010200000_284647FE0FFE10A00000_10820000000010400000;
    {result_glyph_rows[464], result_glyph_rows[465], result_glyph_rows[466], result_glyph_rows[467],
     result_glyph_rows[468], result_glyph_rows[469], result_glyph_rows[470], result_glyph_rows[471],
     result_glyph_rows[472], result_glyph_rows[473], result_glyph_rows[474], result_glyph_rows[475],
     result_glyph_rows[476], result_glyph_rows[477], result_glyph_rows[478], result_glyph_rows[479]} =
        1280'h08200100010008000000_08200100010009FC0000_7EFC01007FFC10000000_08200100010020000000_1C7011001FF048000000_2AA81100101008000000_C82611F8101013FE0000_000011001FF030200000_3FF81100101050200000_000011001FF090200000_00001100101010200000_FFFE11001FF010200000_01001100101010200000_11101100101010200000_2508FFFEFFFE10A00000_42040000000010400000;
    {result_glyph_rows[480], result_glyph_rows[481], result_glyph_rows[482], result_glyph_rows[483],
     result_glyph_rows[484], result_glyph_rows[485], result_glyph_rows[486], result_glyph_rows[487],
     result_glyph_rows[488], result_glyph_rows[489], result_glyph_rows[490], result_glyph_rows[491],
     result_glyph_rows[492], result_glyph_rows[493], result_glyph_rows[494], result_glyph_rows[495]} =
        1280'h08200100020020200000_08200100020020200000_7EFC0100020020200000_08200100FFFEFDFC0000_1C701100040040200000_2AA81100040050400000_C82611F8080093FE0000_000011000800FC400000_3FF811001FF810800000_00001100280811FC0000_0000110048081C040000_FFFE11008808F0880000_01001100080850500000_11101100080810200000_2508FFFE0FF810100000_42040000080810100000;
    {result_glyph_rows[496], result_glyph_rows[497], result_glyph_rows[498], result_glyph_rows[499],
     result_glyph_rows[500], result_glyph_rows[501], result_glyph_rows[502], result_glyph_rows[503],
     result_glyph_rows[504], result_glyph_rows[505], result_glyph_rows[506], result_glyph_rows[507],
     result_glyph_rows[508], result_glyph_rows[509], result_glyph_rows[510], result_glyph_rows[511]} =
        1280'h08200100020002002020_08200100020002002020_7EFC0100020002002020_08200100FFFEFFFEFDFC_1C701100040004004020_2AA81100040004005040_C82611F80400080093FE_0000110008000800FC40_3FF811000FF81FF81080_000011001080280811FC_00001100108048081C04_FFFE110020808808F088_01001100408008085050_11101100808008081020_2508FFFE3FFE0FF81010_42040000000008081010;
    {result_glyph_rows[512], result_glyph_rows[513], result_glyph_rows[514], result_glyph_rows[515],
     result_glyph_rows[516], result_glyph_rows[517], result_glyph_rows[518], result_glyph_rows[519],
     result_glyph_rows[520], result_glyph_rows[521], result_glyph_rows[522], result_glyph_rows[523],
     result_glyph_rows[524], result_glyph_rows[525], result_glyph_rows[526], result_glyph_rows[527]} =
        1280'h08200100102000800000_08200100102000800000_7EFC0100103E08800000_08200100102004800000_1C701100FDFC24800000_2AA81100110410800000_C82611F815FC10800000_00001100190400800000_3FF8110031FCFFFE0000_00001100D12401000000_00001100102001400000_FFFE110013FE02200000_01001100102004100000_11101100102008080000_2508FFFE502030040000_420400002020C0040000;
    {result_glyph_rows[528], result_glyph_rows[529], result_glyph_rows[530], result_glyph_rows[531],
     result_glyph_rows[532], result_glyph_rows[533], result_glyph_rows[534], result_glyph_rows[535],
     result_glyph_rows[536], result_glyph_rows[537], result_glyph_rows[538], result_glyph_rows[539],
     result_glyph_rows[540], result_glyph_rows[541], result_glyph_rows[542], result_glyph_rows[543]} =
        1280'h08200100002010400000_08200100004010400000_7EFC010079FC3F7E0000_08200100490428900000_1C701100494445080000_2AA81100492481000000_C82611F849243FF80000_00001100490C21080000_3FF81100490021080000_0000110049FE21080000_0000110078023FF80000_FFFE1100480221080000_0100110003FA21080000_11101100000221080000_2508FFFE00143FF80000_42040000000820080000;
    {result_glyph_rows[544], result_glyph_rows[545], result_glyph_rows[546], result_glyph_rows[547],
     result_glyph_rows[548], result_glyph_rows[549], result_glyph_rows[550], result_glyph_rows[551],
     result_glyph_rows[552], result_glyph_rows[553], result_glyph_rows[554], result_glyph_rows[555],
     result_glyph_rows[556], result_glyph_rows[557], result_glyph_rows[558], result_glyph_rows[559]} =
        1280'h01000200004008000000_01000200204009FC0000_02800200104010000000_04407FFC104020000000_08200400004048000000_30180900004008000000_CFE61100F07C13FE0000_00002100104030200000_00003FF8104050200000_7FFC0100104090200000_02000100104010200000_0400FFFE144010200000_08200100184010200000_10100100104010200000_3FF801000FFE10A00000_10080100000010400000;
    {result_glyph_rows[560], result_glyph_rows[561], result_glyph_rows[562], result_glyph_rows[563],
     result_glyph_rows[564], result_glyph_rows[565], result_glyph_rows[566], result_glyph_rows[567],
     result_glyph_rows[568], result_glyph_rows[569], result_glyph_rows[570], result_glyph_rows[571],
     result_glyph_rows[572], result_glyph_rows[573], result_glyph_rows[574], result_glyph_rows[575]} =
        1280'h00000040000000000000_7BF82040000000000000_4A0817FC000000000000_520810407E1800000000_53F803F8422400000000_62080248044200000000_5208F248044200000000_4BF813F8084200000000_4A4410E0084200000000_4A481150104200000000_6A301248104200000000_52201444104200000000_42101040102400000000_42882800101800000000_430647FE000000000000_42000000000000000000;
    {result_glyph_rows[576], result_glyph_rows[577], result_glyph_rows[578], result_glyph_rows[579],
     result_glyph_rows[580], result_glyph_rows[581], result_glyph_rows[582], result_glyph_rows[583],
     result_glyph_rows[584], result_glyph_rows[585], result_glyph_rows[586], result_glyph_rows[587],
     result_glyph_rows[588], result_glyph_rows[589], result_glyph_rows[590], result_glyph_rows[591]} =
        1280'h00000040000000000000_7BF82040000000000000_4A0817FC000000000000_520810403C1800000000_53F803F8422400000000_62080248424200000000_5208F248424200000000_4BF813F8244200000000_4A4410E0184200000000_4A481150244200000000_6A301248424200000000_52201444424200000000_42101040422400000000_428828003C1800000000_430647FE000000000000_42000000000000000000;
    {result_glyph_rows[592], result_glyph_rows[593], result_glyph_rows[594], result_glyph_rows[595],
     result_glyph_rows[596], result_glyph_rows[597], result_glyph_rows[598], result_glyph_rows[599],
     result_glyph_rows[600], result_glyph_rows[601], result_glyph_rows[602], result_glyph_rows[603],
     result_glyph_rows[604], result_glyph_rows[605], result_glyph_rows[606], result_glyph_rows[607]} =
        1280'h00000040000000000000_7BF82040000000000000_4A0817FC000000000000_520810403C1800000000_53F803F8422400000000_62080248424200000000_5208F248424200000000_4BF813F8024200000000_4A4410E0044200000000_4A481150084200000000_6A301248104200000000_52201444204200000000_42101040422400000000_428828007E1800000000_430647FE000000000000_42000000000000000000;
    {result_glyph_rows[608], result_glyph_rows[609], result_glyph_rows[610], result_glyph_rows[611],
     result_glyph_rows[612], result_glyph_rows[613], result_glyph_rows[614], result_glyph_rows[615],
     result_glyph_rows[616], result_glyph_rows[617], result_glyph_rows[618], result_glyph_rows[619],
     result_glyph_rows[620], result_glyph_rows[621], result_glyph_rows[622], result_glyph_rows[623]} =
        1280'h08200100020020200000_08200100020020200000_7EFC0100020020200000_08200100FFFEFDFC0000_1C701100040040200000_2AA81100040050400000_C82611F8040093FE0000_000011000800FC400000_3FF811000FF810800000_00001100108011FC0000_0000110010801C040000_FFFE11002080F0880000_01001100408050500000_11101100808010200000_2508FFFE3FFE10100000_42040000000010100000;
end

reg [15:0]digit_glyph_rows[0:229];
always @(*) begin
    {digit_glyph_rows[0], digit_glyph_rows[1], digit_glyph_rows[2], digit_glyph_rows[3],
     digit_glyph_rows[4], digit_glyph_rows[5], digit_glyph_rows[6], digit_glyph_rows[7],
     digit_glyph_rows[8], digit_glyph_rows[9], digit_glyph_rows[10], digit_glyph_rows[11],
     digit_glyph_rows[12], digit_glyph_rows[13], digit_glyph_rows[14], digit_glyph_rows[15],
     digit_glyph_rows[16], digit_glyph_rows[17], digit_glyph_rows[18], digit_glyph_rows[19],
     digit_glyph_rows[20], digit_glyph_rows[21], digit_glyph_rows[22]} =
        368'h0000_0FF0_1FF8_1E78_3C3C_3C3C_781E_781E_781E_781E_781E_781E_781E_781E_781E_781E_781E_781E_3C3C_3C3C_1E78_1FF8_0FE0;
    {digit_glyph_rows[23], digit_glyph_rows[24], digit_glyph_rows[25], digit_glyph_rows[26],
     digit_glyph_rows[27], digit_glyph_rows[28], digit_glyph_rows[29], digit_glyph_rows[30],
     digit_glyph_rows[31], digit_glyph_rows[32], digit_glyph_rows[33], digit_glyph_rows[34],
     digit_glyph_rows[35], digit_glyph_rows[36], digit_glyph_rows[37], digit_glyph_rows[38],
     digit_glyph_rows[39], digit_glyph_rows[40], digit_glyph_rows[41], digit_glyph_rows[42],
     digit_glyph_rows[43], digit_glyph_rows[44], digit_glyph_rows[45]} =
        368'h0000_01C0_07C0_1FC0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_03C0_07E0_1FFC;
    {digit_glyph_rows[46], digit_glyph_rows[47], digit_glyph_rows[48], digit_glyph_rows[49],
     digit_glyph_rows[50], digit_glyph_rows[51], digit_glyph_rows[52], digit_glyph_rows[53],
     digit_glyph_rows[54], digit_glyph_rows[55], digit_glyph_rows[56], digit_glyph_rows[57],
     digit_glyph_rows[58], digit_glyph_rows[59], digit_glyph_rows[60], digit_glyph_rows[61],
     digit_glyph_rows[62], digit_glyph_rows[63], digit_glyph_rows[64], digit_glyph_rows[65],
     digit_glyph_rows[66], digit_glyph_rows[67], digit_glyph_rows[68]} =
        368'h0000_0FF0_1EF8_383C_783C_781C_7C1C_7C1C_383C_003C_0078_0078_00F0_01E0_03C0_0780_0F00_1E0E_3C0E_380E_701C_7FFC_7FFC;
    {digit_glyph_rows[69], digit_glyph_rows[70], digit_glyph_rows[71], digit_glyph_rows[72],
     digit_glyph_rows[73], digit_glyph_rows[74], digit_glyph_rows[75], digit_glyph_rows[76],
     digit_glyph_rows[77], digit_glyph_rows[78], digit_glyph_rows[79], digit_glyph_rows[80],
     digit_glyph_rows[81], digit_glyph_rows[82], digit_glyph_rows[83], digit_glyph_rows[84],
     digit_glyph_rows[85], digit_glyph_rows[86], digit_glyph_rows[87], digit_glyph_rows[88],
     digit_glyph_rows[89], digit_glyph_rows[90], digit_glyph_rows[91]} =
        368'h0000_0FF0_3FF8_3878_383C_383C_383C_003C_003C_0078_01F0_07E0_01F8_003C_003C_001E_001E_781E_781E_783C_783C_3EF8_1FF0;
    {digit_glyph_rows[92], digit_glyph_rows[93], digit_glyph_rows[94], digit_glyph_rows[95],
     digit_glyph_rows[96], digit_glyph_rows[97], digit_glyph_rows[98], digit_glyph_rows[99],
     digit_glyph_rows[100], digit_glyph_rows[101], digit_glyph_rows[102], digit_glyph_rows[103],
     digit_glyph_rows[104], digit_glyph_rows[105], digit_glyph_rows[106], digit_glyph_rows[107],
     digit_glyph_rows[108], digit_glyph_rows[109], digit_glyph_rows[110], digit_glyph_rows[111],
     digit_glyph_rows[112], digit_glyph_rows[113], digit_glyph_rows[114]} =
        368'h0000_0070_00F0_00F0_01F0_01F0_03F0_0770_0770_0E70_1E70_1C70_3870_3870_7070_F070_FFFF_0070_0070_0070_0070_00F8_07FE;
    {digit_glyph_rows[115], digit_glyph_rows[116], digit_glyph_rows[117], digit_glyph_rows[118],
     digit_glyph_rows[119], digit_glyph_rows[120], digit_glyph_rows[121], digit_glyph_rows[122],
     digit_glyph_rows[123], digit_glyph_rows[124], digit_glyph_rows[125], digit_glyph_rows[126],
     digit_glyph_rows[127], digit_glyph_rows[128], digit_glyph_rows[129], digit_glyph_rows[130],
     digit_glyph_rows[131], digit_glyph_rows[132], digit_glyph_rows[133], digit_glyph_rows[134],
     digit_glyph_rows[135], digit_glyph_rows[136], digit_glyph_rows[137]} =
        368'h0000_1FFC_1FFC_3800_3800_3800_3800_3800_39C0_3FF8_3FF8_3C3C_383C_001E_001E_001E_381E_781E_781C_783C_783C_3EF8_0FF0;
    {digit_glyph_rows[138], digit_glyph_rows[139], digit_glyph_rows[140], digit_glyph_rows[141],
     digit_glyph_rows[142], digit_glyph_rows[143], digit_glyph_rows[144], digit_glyph_rows[145],
     digit_glyph_rows[146], digit_glyph_rows[147], digit_glyph_rows[148], digit_glyph_rows[149],
     digit_glyph_rows[150], digit_glyph_rows[151], digit_glyph_rows[152], digit_glyph_rows[153],
     digit_glyph_rows[154], digit_glyph_rows[155], digit_glyph_rows[156], digit_glyph_rows[157],
     digit_glyph_rows[158], digit_glyph_rows[159], digit_glyph_rows[160]} =
        368'h0000_07F8_0FFC_1E3C_3C3C_3838_3800_7800_7800_7FF8_7FFC_7E3C_7C1E_781E_781E_781E_781E_781E_781E_3C1E_3E3C_1F78_0FF0;
    {digit_glyph_rows[161], digit_glyph_rows[162], digit_glyph_rows[163], digit_glyph_rows[164],
     digit_glyph_rows[165], digit_glyph_rows[166], digit_glyph_rows[167], digit_glyph_rows[168],
     digit_glyph_rows[169], digit_glyph_rows[170], digit_glyph_rows[171], digit_glyph_rows[172],
     digit_glyph_rows[173], digit_glyph_rows[174], digit_glyph_rows[175], digit_glyph_rows[176],
     digit_glyph_rows[177], digit_glyph_rows[178], digit_glyph_rows[179], digit_glyph_rows[180],
     digit_glyph_rows[181], digit_glyph_rows[182], digit_glyph_rows[183]} =
        368'h0000_3FFE_3FFE_381C_701C_7038_7078_0070_00F0_00E0_00E0_01C0_01C0_03C0_03C0_0380_0780_0780_0780_0780_0780_0780_0780;
    {digit_glyph_rows[184], digit_glyph_rows[185], digit_glyph_rows[186], digit_glyph_rows[187],
     digit_glyph_rows[188], digit_glyph_rows[189], digit_glyph_rows[190], digit_glyph_rows[191],
     digit_glyph_rows[192], digit_glyph_rows[193], digit_glyph_rows[194], digit_glyph_rows[195],
     digit_glyph_rows[196], digit_glyph_rows[197], digit_glyph_rows[198], digit_glyph_rows[199],
     digit_glyph_rows[200], digit_glyph_rows[201], digit_glyph_rows[202], digit_glyph_rows[203],
     digit_glyph_rows[204], digit_glyph_rows[205], digit_glyph_rows[206]} =
        368'h0000_0FF0_3FF8_383C_781C_701E_781E_781E_7C1C_3F3C_1FF8_0FF0_1FF8_3CFC_783C_701E_701E_701E_701E_701E_783C_3EF8_0FF0;
    {digit_glyph_rows[207], digit_glyph_rows[208], digit_glyph_rows[209], digit_glyph_rows[210],
     digit_glyph_rows[211], digit_glyph_rows[212], digit_glyph_rows[213], digit_glyph_rows[214],
     digit_glyph_rows[215], digit_glyph_rows[216], digit_glyph_rows[217], digit_glyph_rows[218],
     digit_glyph_rows[219], digit_glyph_rows[220], digit_glyph_rows[221], digit_glyph_rows[222],
     digit_glyph_rows[223], digit_glyph_rows[224], digit_glyph_rows[225], digit_glyph_rows[226],
     digit_glyph_rows[227], digit_glyph_rows[228], digit_glyph_rows[229]} =
        368'h0000_0FF0_3EF8_3C3C_783C_781C_701E_701E_701E_701E_783E_783E_7C7E_3FFE_1FFE_001E_003C_003C_183C_3C78_3C78_3FF0_1FE0;
end

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

wire pixel_fps_hundred= (in_hundred_area == 1)?digit_glyph_rows[v_cnt + 23*fps_hundred][15 - h_cnt]:0;
wire pixel_fps_decade = (in_decade_area == 1)?digit_glyph_rows[v_cnt + 23*fps_decade][31-h_cnt]:0;
wire pixel_fps_unit = (in_unit_area == 1)?digit_glyph_rows[v_cnt + 23*fps_unit][47-h_cnt]:0;
wire pixel_fps = pixel_fps_hundred || pixel_fps_decade || pixel_fps_unit;

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

        case (char_en)
            16'd1:
                {tens, ones, digit_en, result_en} <= {4'd1, 4'd5, 1'b1, 1'b1};
            16'd2:
                {tens, ones, digit_en, result_en} <= {4'd3, 4'd0, 1'b1, 1'b1};
            16'd3:
                {tens, ones, digit_en, result_en} <= {4'd4, 4'd0, 1'b1, 1'b1};
            16'd4:
                {tens, ones, digit_en, result_en} <= {4'd5, 4'd0, 1'b1, 1'b1};
            16'd5:
                {tens, ones, digit_en, result_en} <= {4'd6, 4'd0, 1'b1, 1'b1};
            16'd6, 16'd7, 16'd8, 16'd9, 16'd10, 16'd11,
            16'd12, 16'd13, 16'd14, 16'd15, 16'd16, 16'd17,
            16'd18, 16'd19, 16'd20, 16'd21, 16'd22, 16'd23,
            16'd24, 16'd25, 16'd26, 16'd27, 16'd28, 16'd29,
            16'd30, 16'd31, 16'd32, 16'd33, 16'd34, 16'd35,
            16'd39:
                {tens, ones, digit_en, result_en} <= {4'd0, 4'd0, 1'b0, 1'b1};
            16'd36:
                {tens, ones, digit_en, result_en} <= {4'd7, 4'd0, 1'b1, 1'b1};
            16'd37:
                {tens, ones, digit_en, result_en} <= {4'd8, 4'd0, 1'b1, 1'b1};
            16'd38:
                {tens, ones, digit_en, result_en} <= {4'd2, 4'd0, 1'b1, 1'b1};
            default: {tens, ones, digit_en, result_en} <= 10'd0;
        endcase
	end
end
wire pixel_digit_tens = (is_tens_area == 1 && digit_en == 1)?digit_glyph_rows[v_cnt + 23*tens][622 - h_cnt]:0;
wire pixel_digit_ones = (is_ones_area == 1 && digit_en == 1)?digit_glyph_rows[v_cnt + 23*ones][639 - h_cnt]:0;
wire pixel_digit = pixel_digit_tens || pixel_digit_ones;

wire [10:0] col_right = col_left + 80;
wire [10:0] row_high = row_low - 17;
wire is_result_area =   (h_cnt >= col_left && h_cnt < col_right) && (v_cnt >= row_high && v_cnt < (row_low - 1));
wire [79:0]pixel_v = (is_result_area == 1 && result_en == 1)?result_glyph_rows[v_cnt-row_high + 16*(char_en-1)]:64'd0;
wire pixel_h = (is_result_area == 1 && result_en == 1)?pixel_v[col_right-h_cnt]:0;

reg overlay_pixel;
always @(posedge clk_pixel) begin
    overlay_pixel <= pixel_fps || pixel_digit || pixel_h;
    pixel_rgb_reg <= overlay_pixel ? 24'h00ff00 : rgb;
end

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