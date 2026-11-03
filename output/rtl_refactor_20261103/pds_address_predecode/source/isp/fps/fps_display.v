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


reg [23:0] pixel_rgb_reg;
reg vs_d0,hs_d0,de_d0;
assign pixel_rgb = pixel_rgb_reg;
assign o_vs = vs_d0;
assign o_hs = hs_d0;
assign o_de = de_d0;

///////////////////char_init////////////////////

reg [79:0]result_char[0:623];
// Decode identical bitmap rows with one case value.
function [79:0] result_char_row;
    input integer address;
    begin
        case (address)
            0, 16, 32, 48, 64, 560, 576, 592: result_char_row = 80'h00000040000000000000;
            1, 17, 33, 49, 65, 561, 577, 593: result_char_row = 80'h7BF82040000000000000;
            2, 18, 34, 50, 66, 562, 578, 594: result_char_row = 80'h4A0817FC000000000000;
            3: result_char_row = 80'h52081040087E00000000;
            4: result_char_row = 80'h53F803F8384000000000;
            5: result_char_row = 80'h62080248084000000000;
            6: result_char_row = 80'h5208F248084000000000;
            7: result_char_row = 80'h4BF813F8087800000000;
            8: result_char_row = 80'h4A4410E0084400000000;
            9: result_char_row = 80'h4A481150080200000000;
            10: result_char_row = 80'h6A301248080200000000;
            11: result_char_row = 80'h52201444084200000000;
            12: result_char_row = 80'h42101040084400000000;
            13: result_char_row = 80'h428828003E3800000000;
            14, 30, 46, 62, 78, 574, 590, 606: result_char_row = 80'h430647FE000000000000;
            15, 31, 47, 63, 79, 575, 591, 607: result_char_row = 80'h42000000000000000000;
            19, 579, 595: result_char_row = 80'h520810403C1800000000;
            20, 564, 580, 596: result_char_row = 80'h53F803F8422400000000;
            21, 581, 597: result_char_row = 80'h62080248424200000000;
            22: result_char_row = 80'h5208F248024200000000;
            23: result_char_row = 80'h4BF813F8044200000000;
            24, 584: result_char_row = 80'h4A4410E0184200000000;
            25: result_char_row = 80'h4A481150044200000000;
            26, 58: result_char_row = 80'h6A301248024200000000;
            27, 59, 75, 587: result_char_row = 80'h52201444424200000000;
            28, 588, 604: result_char_row = 80'h42101040422400000000;
            29, 589: result_char_row = 80'h428828003C1800000000;
            35: result_char_row = 80'h52081040041800000000;
            36: result_char_row = 80'h53F803F80C2400000000;
            37: result_char_row = 80'h620802480C4200000000;
            38: result_char_row = 80'h5208F248144200000000;
            39, 583: result_char_row = 80'h4BF813F8244200000000;
            40: result_char_row = 80'h4A4410E0244200000000;
            41: result_char_row = 80'h4A481150444200000000;
            42: result_char_row = 80'h6A3012487F4200000000;
            43: result_char_row = 80'h52201444044200000000;
            44: result_char_row = 80'h42101040042400000000;
            45: result_char_row = 80'h428828001F1800000000;
            51, 563: result_char_row = 80'h520810407E1800000000;
            52: result_char_row = 80'h53F803F8402400000000;
            53, 69: result_char_row = 80'h62080248404200000000;
            54, 70: result_char_row = 80'h5208F248404200000000;
            55: result_char_row = 80'h4BF813F8784200000000;
            56: result_char_row = 80'h4A4410E0444200000000;
            57: result_char_row = 80'h4A481150024200000000;
            60: result_char_row = 80'h42101040442400000000;
            61: result_char_row = 80'h42882800381800000000;
            67: result_char_row = 80'h52081040181800000000;
            68: result_char_row = 80'h53F803F8242400000000;
            71: result_char_row = 80'h4BF813F85C4200000000;
            72: result_char_row = 80'h4A4410E0624200000000;
            73: result_char_row = 80'h4A481150424200000000;
            74, 586: result_char_row = 80'h6A301248424200000000;
            76: result_char_row = 80'h42101040222400000000;
            77: result_char_row = 80'h428828001C1800000000;
            80: result_char_row = 80'h01000800000000000000;
            81: result_char_row = 80'h010009FC000000000000;
            82: result_char_row = 80'h7FFC1000000000000000;
            83: result_char_row = 80'h01002000000000000000;
            84: result_char_row = 80'h1FF04800000000000000;
            85: result_char_row = 80'h10100800000000000000;
            86: result_char_row = 80'h101013FE000000000000;
            87: result_char_row = 80'h1FF03020000000000000;
            88: result_char_row = 80'h10105020000000000000;
            89: result_char_row = 80'h1FF09020000000000000;
            90, 92, 93: result_char_row = 80'h10101020000000000000;
            91: result_char_row = 80'h1FF01020000000000000;
            94: result_char_row = 80'hFFFE10A0000000000000;
            95: result_char_row = 80'h00001040000000000000;
            96, 97, 98, 112, 113, 114: result_char_row = 80'h02002020000000000000;
            99, 115: result_char_row = 80'hFFFEFDFC000000000000;
            100, 116: result_char_row = 80'h04004020000000000000;
            101, 117: result_char_row = 80'h04005040000000000000;
            102: result_char_row = 80'h040093FE000000000000;
            103, 119: result_char_row = 80'h0800FC40000000000000;
            104: result_char_row = 80'h0FF81080000000000000;
            105: result_char_row = 80'h108011FC000000000000;
            106: result_char_row = 80'h10801C04000000000000;
            107: result_char_row = 80'h2080F088000000000000;
            108: result_char_row = 80'h40805050000000000000;
            109: result_char_row = 80'h80801020000000000000;
            110: result_char_row = 80'h3FFE1010000000000000;
            111: result_char_row = 80'h00001010000000000000;
            118: result_char_row = 80'h080093FE000000000000;
            120: result_char_row = 80'h1FF81080000000000000;
            121: result_char_row = 80'h280811FC000000000000;
            122: result_char_row = 80'h48081C04000000000000;
            123: result_char_row = 80'h8808F088000000000000;
            124: result_char_row = 80'h08085050000000000000;
            125: result_char_row = 80'h08081020000000000000;
            126: result_char_row = 80'h0FF81010000000000000;
            127: result_char_row = 80'h08081010000000000000;
            128, 144: result_char_row = 80'h01000800004802002020;
            129, 145: result_char_row = 80'h010009FC004402002020;
            130, 146: result_char_row = 80'h7FFC1000004002002020;
            131, 147: result_char_row = 80'h01002000FFFEFFFEFDFC;
            132, 148: result_char_row = 80'h1FF04800004004004020;
            133, 149: result_char_row = 80'h10100800004004005040;
            134: result_char_row = 80'h101013FE3E44040093FE;
            135, 151: result_char_row = 80'h1FF0302022440800FC40;
            136: result_char_row = 80'h1010502022440FF81080;
            137: result_char_row = 80'h1FF090202228108011FC;
            138: result_char_row = 80'h101010203E2810801C04;
            139: result_char_row = 80'h1FF0102000122080F088;
            140: result_char_row = 80'h10101020073240805050;
            141: result_char_row = 80'h10101020784A80801020;
            142: result_char_row = 80'hFFFE10A020863FFE1010;
            143: result_char_row = 80'h00001040030200001010;
            150: result_char_row = 80'h101013FE3E44080093FE;
            152: result_char_row = 80'h1010502022441FF81080;
            153: result_char_row = 80'h1FF090202228280811FC;
            154: result_char_row = 80'h101010203E2848081C04;
            155: result_char_row = 80'h1FF0102000128808F088;
            156: result_char_row = 80'h10101020073208085050;
            157: result_char_row = 80'h10101020784A08081020;
            158: result_char_row = 80'hFFFE10A020860FF81010;
            159: result_char_row = 80'h00001040030208081010;
            160: result_char_row = 80'h10800200088000000000;
            161: result_char_row = 80'h10400200084000000000;
            162: result_char_row = 80'h17FC0200084000000000;
            163: result_char_row = 80'h20007FFC100000000000;
            164: result_char_row = 80'h23F8040017FC00000000;
            165: result_char_row = 80'h62080900300000000000;
            166: result_char_row = 80'h63F81100300800000000;
            167: result_char_row = 80'hA0002100520800000000;
            168: result_char_row = 80'h2FFE3FF8920800000000;
            169: result_char_row = 80'h28020100111000000000;
            170: result_char_row = 80'h23F80100111000000000;
            171: result_char_row = 80'h2040FFFE111000000000;
            172: result_char_row = 80'h20400100112000000000;
            173: result_char_row = 80'h20400100102000000000;
            174: result_char_row = 80'h214001001FFE00000000;
            175: result_char_row = 80'h20800100100000000000;
            176: result_char_row = 80'h01000800020800000000;
            177: result_char_row = 80'h010009FC211000000000;
            178: result_char_row = 80'h01001000100000000000;
            179: result_char_row = 80'h0100200017FC00000000;
            180: result_char_row = 80'h01004800008000000000;
            181: result_char_row = 80'h0100080003F800000000;
            182: result_char_row = 80'h028013FEF20800000000;
            183: result_char_row = 80'h0280302013F800000000;
            184: result_char_row = 80'h04405020120800000000;
            185: result_char_row = 80'h0440902013F800000000;
            186: result_char_row = 80'h08201020120800000000;
            187: result_char_row = 80'h0820102013F800000000;
            188: result_char_row = 80'h10101020120800000000;
            189: result_char_row = 80'h20081020280000000000;
            190: result_char_row = 80'h400410A047FE00000000;
            191: result_char_row = 80'h80021040000000000000;
            192: result_char_row = 80'h00000100080000200000;
            193: result_char_row = 80'h0000020009FCF8200000;
            194: result_char_row = 80'hFDFE1FF0100008200000;
            195: result_char_row = 80'h10101010200049FC0000;
            196: result_char_row = 80'h10101210480049240000;
            197: result_char_row = 80'h10201150080049240000;
            198: result_char_row = 80'h1020102013FE49240000;
            199: result_char_row = 80'h7C68100030207D240000;
            200: result_char_row = 80'h10A41FFC502005FC0000;
            201: result_char_row = 80'h11220204902004200000;
            202: result_char_row = 80'h1222222410201CA00000;
            203: result_char_row = 80'h102022241020E4600000;
            204: result_char_row = 80'h1C203FE4102044600000;
            205: result_char_row = 80'hE0200004102004900000;
            206: result_char_row = 80'h4020002810A029080000;
            207: result_char_row = 80'h00200010104012060000;
            208, 209: result_char_row = 80'h10200080000000000000;
            210: result_char_row = 80'h103E0880000000000000;
            211: result_char_row = 80'h10200480000000000000;
            212: result_char_row = 80'hFDFC2480000000000000;
            213: result_char_row = 80'h11041080000000000000;
            214: result_char_row = 80'h15FC1080000000000000;
            215: result_char_row = 80'h19040080000000000000;
            216: result_char_row = 80'h31FCFFFE000000000000;
            217: result_char_row = 80'hD1240100000000000000;
            218: result_char_row = 80'h10200140000000000000;
            219: result_char_row = 80'h13FE0220000000000000;
            220: result_char_row = 80'h10200410000000000000;
            221: result_char_row = 80'h10200808000000000000;
            222: result_char_row = 80'h50203004000000000000;
            223: result_char_row = 80'h2020C004000000000000;
            224: result_char_row = 80'h10100800004000000000;
            225: result_char_row = 80'h082009FC7C4000000000;
            226: result_char_row = 80'h04401000447800000000;
            227: result_char_row = 80'h3FF82000448800000000;
            228: result_char_row = 80'h21084800455000000000;
            229: result_char_row = 80'h210808007C2000000000;
            230: result_char_row = 80'h3FF813FE105000000000;
            231: result_char_row = 80'h21083020108800000000;
            232: result_char_row = 80'h21085020110600000000;
            233: result_char_row = 80'h3FF890205CF800000000;
            234, 235: result_char_row = 80'h01001020508800000000;
            236: result_char_row = 80'hFFFE1020508800000000;
            237: result_char_row = 80'h010010205C8800000000;
            238: result_char_row = 80'h010010A0E0F800000000;
            239: result_char_row = 80'h01001040008800000000;
            240: result_char_row = 80'h02000000004000000000;
            241: result_char_row = 80'h01003FF07C4000000000;
            242: result_char_row = 80'h0100101044783FF80000;
            243: result_char_row = 80'hFFFE1210448820080000;
            244: result_char_row = 80'h00001120455020080000;
            245: result_char_row = 80'h101009207C2020080000;
            246: result_char_row = 80'h10080820105020080000;
            247: result_char_row = 80'h20240440108820080000;
            248: result_char_row = 80'h48240440110620080000;
            249: result_char_row = 80'h044002805CF820080000;
            250: result_char_row = 80'h02800100508820080000;
            251: result_char_row = 80'h01000280508820080000;
            252: result_char_row = 80'h02800440508820080000;
            253: result_char_row = 80'h0C4008205C883FF80000;
            254: result_char_row = 80'h30303018E0F820080000;
            255: result_char_row = 80'hC00EC006008800000000;
            256: result_char_row = 80'h00000200020000000000;
            257: result_char_row = 80'h00000400010047F80000;
            258: result_char_row = 80'hFDFC0800010020100000;
            259: result_char_row = 80'h04847FFCFFFE21A00000;
            260: result_char_row = 80'h44844004000000400000;
            261: result_char_row = 80'h44844004101007FC0000;
            262: result_char_row = 80'h288847C41008E4440000;
            263: result_char_row = 80'h28884444202424440000;
            264: result_char_row = 80'h10504444482427FC0000;
            265: result_char_row = 80'h10504444044024440000;
            266: result_char_row = 80'h28204444028024440000;
            267: result_char_row = 80'h282047C4010027FC0000;
            268: result_char_row = 80'h44504444028024440000;
            269: result_char_row = 80'h448840040C4024540000;
            270: result_char_row = 80'h81044014303054080000;
            271: result_char_row = 80'h02024008C00E8FFE0000;
            272: result_char_row = 80'h00800100040000400000;
            273: result_char_row = 80'h20403FF8040078400000;
            274: result_char_row = 80'h100008200FF048A00000;
            275: result_char_row = 80'h17FC0440101050A00000;
            276: result_char_row = 80'h8040FFFE202051100000;
            277: result_char_row = 80'h404000005FFC62080000;
            278: result_char_row = 80'h40401FF0100055F60000;
            279: result_char_row = 80'h1040101013F048000000;
            280: result_char_row = 80'h13FC1FF0121048880000;
            281: result_char_row = 80'h20401010121048480000;
            282: result_char_row = 80'hE0401FF012506A480000;
            283: result_char_row = 80'h20400200122051500000;
            284: result_char_row = 80'h20405104220441100000;
            285: result_char_row = 80'h20405112220440200000;
            286: result_char_row = 80'h2FFE901241FC47FE0000;
            287: result_char_row = 80'h00000FF0800040000000;
            288: result_char_row = 80'h00800100080001000000;
            289: result_char_row = 80'h20403FF809FC01000000;
            290: result_char_row = 80'h10000820100001000000;
            291: result_char_row = 80'h17FC0440200001000000;
            292: result_char_row = 80'h8040FFFE480001000000;
            293: result_char_row = 80'h40400000080001000000;
            294: result_char_row = 80'h40401FF013FE02800000;
            295: result_char_row = 80'h10401010302002800000;
            296: result_char_row = 80'h13FC1FF0502004400000;
            297: result_char_row = 80'h20401010902004400000;
            298: result_char_row = 80'hE0401FF0102008200000;
            299: result_char_row = 80'h20400200102008200000;
            300: result_char_row = 80'h20405104102010100000;
            301: result_char_row = 80'h20405112102020080000;
            302: result_char_row = 80'h2FFE901210A040040000;
            303: result_char_row = 80'h00000FF0104080020000;
            304: result_char_row = 80'h00800100084002000000;
            305: result_char_row = 80'h20403FF8084001000000;
            306: result_char_row = 80'h1000082008403FF80000;
            307: result_char_row = 80'h17FC0440084008200000;
            308: result_char_row = 80'h8040FFFE084004400000;
            309: result_char_row = 80'h404000000840FFFE0000;
            310: result_char_row = 80'h40401FF0084000000000;
            311: result_char_row = 80'h1040101008401FF00000;
            312: result_char_row = 80'h13FC1FF0084011100000;
            313: result_char_row = 80'h2040101008401FF00000;
            314: result_char_row = 80'hE0401FF0084011100000;
            315: result_char_row = 80'h2040020010421FF00000;
            316: result_char_row = 80'h20405104104201000000;
            317: result_char_row = 80'h2040511220423FF80000;
            318: result_char_row = 80'h2FFE9012403E01000000;
            319: result_char_row = 80'h00000FF08000FFFE0000;
            320: result_char_row = 80'h00800100004010800000;
            321: result_char_row = 80'h20403FF8004010800000;
            322: result_char_row = 80'h100008207C4050800000;
            323: result_char_row = 80'h17FC0440004050FC0000;
            324: result_char_row = 80'h8040FFFE01FC7D540000;
            325: result_char_row = 80'h40400000004452540000;
            326: result_char_row = 80'h40401FF0FE4490540000;
            327: result_char_row = 80'h10401010204410940000;
            328: result_char_row = 80'h13FC1FF020441C940000;
            329: result_char_row = 80'h204010102084F1240000;
            330: result_char_row = 80'hE0401FF0488452240000;
            331: result_char_row = 80'h20400200448410440000;
            332: result_char_row = 80'h20405104FD0410440000;
            333: result_char_row = 80'h20405112450410840000;
            334: result_char_row = 80'h2FFE9012022811280000;
            335: result_char_row = 80'h00000FF0041010100000;
            336: result_char_row = 80'h00800100102002000000;
            337: result_char_row = 80'h20403FF8102001000000;
            338: result_char_row = 80'h1000082051207FFC0000;
            339: result_char_row = 80'h17FC0440512004000000;
            340: result_char_row = 80'h8040FFFE7DFC08200000;
            341: result_char_row = 80'h4040000051201FC00000;
            342: result_char_row = 80'h40401FF0922003100000;
            343: result_char_row = 80'h1040101010200C080000;
            344: result_char_row = 80'h13FC1FF01C203FFC0000;
            345: result_char_row = 80'h20401010F1FC00040000;
            346: result_char_row = 80'hE0401FF050203FF80000;
            347: result_char_row = 80'h20400200102021080000;
            348: result_char_row = 80'h2040510410203FF80000;
            349: result_char_row = 80'h20405112102021080000;
            350: result_char_row = 80'h2FFE901213FE3FF80000;
            351: result_char_row = 80'h00000FF0100020080000;
            352, 368: result_char_row = 80'h00800100020008002020;
            353, 369: result_char_row = 80'h20403FF802000FE02020;
            354, 370: result_char_row = 80'h10000820020010202020;
            355, 371: result_char_row = 80'h17FC0440FFFE2040FDFC;
            356, 372: result_char_row = 80'h8040FFFE04005FF84020;
            357, 373: result_char_row = 80'h40400000040000085040;
            358: result_char_row = 80'h40401FF00400000893FE;
            359, 375: result_char_row = 80'h1040101008001FF8FC40;
            360: result_char_row = 80'h13FC1FF00FF800081080;
            361: result_char_row = 80'h204010101080000811FC;
            362: result_char_row = 80'hE0401FF010803FF81C04;
            363: result_char_row = 80'h2040020020800200F088;
            364: result_char_row = 80'h20405104408051045050;
            365: result_char_row = 80'h20405112808051121020;
            366: result_char_row = 80'h2FFE90123FFE90121010;
            367: result_char_row = 80'h00000FF000000FF01010;
            374: result_char_row = 80'h40401FF00800000893FE;
            376: result_char_row = 80'h13FC1FF01FF800081080;
            377: result_char_row = 80'h204010102808000811FC;
            378: result_char_row = 80'hE0401FF048083FF81C04;
            379: result_char_row = 80'h2040020088080200F088;
            380: result_char_row = 80'h20405104080851045050;
            381: result_char_row = 80'h20405112080851121020;
            382: result_char_row = 80'h2FFE90120FF890121010;
            383: result_char_row = 80'h00000FF008080FF01010;
            384: result_char_row = 80'h08200100002004000000;
            385: result_char_row = 80'h08200100F82002000000;
            386: result_char_row = 80'h7EFC0100082001000000;
            387: result_char_row = 80'h0820010049FC01000000;
            388: result_char_row = 80'h1C701100492401000000;
            389: result_char_row = 80'h2AA81100492402800000;
            390: result_char_row = 80'hC82611F8492402800000;
            391: result_char_row = 80'h000011007D2402800000;
            392: result_char_row = 80'h3FF8110005FC04400000;
            393: result_char_row = 80'h00001100042004400000;
            394: result_char_row = 80'h000011001CA008200000;
            395: result_char_row = 80'hFFFE1100E46008200000;
            396: result_char_row = 80'h01001100446010100000;
            397: result_char_row = 80'h11101100049020100000;
            398: result_char_row = 80'h2508FFFE290840080000;
            399: result_char_row = 80'h42040000120680060000;
            400: result_char_row = 80'h08200100020010802040;
            401: result_char_row = 80'h08200100020010401040;
            402: result_char_row = 80'h7EFC0100020017FC0040;
            403: result_char_row = 80'h082001007FFC2000FE80;
            404: result_char_row = 80'h1C701100040023F820FE;
            405: result_char_row = 80'h2AA81100090062082108;
            406: result_char_row = 80'hC82611F8110063F83E88;
            407: result_char_row = 80'h000011002100A0002488;
            408: result_char_row = 80'h3FF811003FF82FFE2488;
            409: result_char_row = 80'h00001100010028022450;
            410: result_char_row = 80'h00001100010023F82450;
            411: result_char_row = 80'hFFFE1100FFFE20402420;
            412: result_char_row = 80'h01001100010020404450;
            413: result_char_row = 80'h11101100010020405488;
            414: result_char_row = 80'h2508FFFE010021408904;
            415: result_char_row = 80'h42040000010020800202;
            416: result_char_row = 80'h20800000000000000000;
            417: result_char_row = 80'h10800000000000000000;
            418: result_char_row = 80'h10FE7FFC000000000000;
            419: result_char_row = 80'h01200100000000000000;
            420: result_char_row = 80'hFE200100000000000000;
            421: result_char_row = 80'h21200100000000000000;
            422: result_char_row = 80'h212C0100000000000000;
            423: result_char_row = 80'h3D740100000000000000;
            424: result_char_row = 80'h27A40100000000000000;
            425: result_char_row = 80'h25240100000000000000;
            426: result_char_row = 80'h25340100000000000000;
            427: result_char_row = 80'h252A0100000000000000;
            428: result_char_row = 80'h25220100000000000000;
            429: result_char_row = 80'h4502FFFE000000000000;
            430: result_char_row = 80'h54FE0000000000000000;
            431: result_char_row = 80'h88000000000000000000;
            432: result_char_row = 80'h10800200004008000000;
            433: result_char_row = 80'h10400200204009FC0000;
            434: result_char_row = 80'h17FC0200104010000000;
            435: result_char_row = 80'h20007FFC104020000000;
            436: result_char_row = 80'h23F80400004048000000;
            437: result_char_row = 80'h62080900004008000000;
            438: result_char_row = 80'h63F81100F07C13FE0000;
            439: result_char_row = 80'hA0002100104030200000;
            440: result_char_row = 80'h2FFE3FF8104050200000;
            441: result_char_row = 80'h28020100104090200000;
            442: result_char_row = 80'h23F80100104010200000;
            443: result_char_row = 80'h2040FFFE144010200000;
            444: result_char_row = 80'h20400100184010200000;
            445: result_char_row = 80'h20400100104010200000;
            446: result_char_row = 80'h214001000FFE10A00000;
            447: result_char_row = 80'h20800100000010400000;
            448: result_char_row = 80'h00140040004008000000;
            449: result_char_row = 80'h40122040204009FC0000;
            450: result_char_row = 80'h201017FC104010000000;
            451: result_char_row = 80'h27FE1040104020000000;
            452: result_char_row = 80'h041003F8004048000000;
            453: result_char_row = 80'h04100248004008000000;
            454: result_char_row = 80'h15D0F248F07C13FE0000;
            455: result_char_row = 80'h141213F8104030200000;
            456: result_char_row = 80'h241210E0104050200000;
            457: result_char_row = 80'hE5D41150104090200000;
            458: result_char_row = 80'h25541248104010200000;
            459: result_char_row = 80'h25481444144010200000;
            460: result_char_row = 80'h25DA1040184010200000;
            461: result_char_row = 80'h242A2800104010200000;
            462: result_char_row = 80'h284647FE0FFE10A00000;
            463: result_char_row = 80'h10820000000010400000;
            464: result_char_row = 80'h08200100010008000000;
            465: result_char_row = 80'h08200100010009FC0000;
            466: result_char_row = 80'h7EFC01007FFC10000000;
            467: result_char_row = 80'h08200100010020000000;
            468: result_char_row = 80'h1C7011001FF048000000;
            469: result_char_row = 80'h2AA81100101008000000;
            470: result_char_row = 80'hC82611F8101013FE0000;
            471: result_char_row = 80'h000011001FF030200000;
            472: result_char_row = 80'h3FF81100101050200000;
            473: result_char_row = 80'h000011001FF090200000;
            474: result_char_row = 80'h00001100101010200000;
            475: result_char_row = 80'hFFFE11001FF010200000;
            476: result_char_row = 80'h01001100101010200000;
            477: result_char_row = 80'h11101100101010200000;
            478: result_char_row = 80'h2508FFFEFFFE10A00000;
            479: result_char_row = 80'h42040000000010400000;
            480, 481, 608, 609: result_char_row = 80'h08200100020020200000;
            482, 610: result_char_row = 80'h7EFC0100020020200000;
            483, 611: result_char_row = 80'h08200100FFFEFDFC0000;
            484, 612: result_char_row = 80'h1C701100040040200000;
            485, 613: result_char_row = 80'h2AA81100040050400000;
            486: result_char_row = 80'hC82611F8080093FE0000;
            487, 615: result_char_row = 80'h000011000800FC400000;
            488: result_char_row = 80'h3FF811001FF810800000;
            489: result_char_row = 80'h00001100280811FC0000;
            490: result_char_row = 80'h0000110048081C040000;
            491: result_char_row = 80'hFFFE11008808F0880000;
            492: result_char_row = 80'h01001100080850500000;
            493: result_char_row = 80'h11101100080810200000;
            494: result_char_row = 80'h2508FFFE0FF810100000;
            495: result_char_row = 80'h42040000080810100000;
            496, 497: result_char_row = 80'h08200100020002002020;
            498: result_char_row = 80'h7EFC0100020002002020;
            499: result_char_row = 80'h08200100FFFEFFFEFDFC;
            500: result_char_row = 80'h1C701100040004004020;
            501: result_char_row = 80'h2AA81100040004005040;
            502: result_char_row = 80'hC82611F80400080093FE;
            503: result_char_row = 80'h0000110008000800FC40;
            504: result_char_row = 80'h3FF811000FF81FF81080;
            505: result_char_row = 80'h000011001080280811FC;
            506: result_char_row = 80'h00001100108048081C04;
            507: result_char_row = 80'hFFFE110020808808F088;
            508: result_char_row = 80'h01001100408008085050;
            509: result_char_row = 80'h11101100808008081020;
            510: result_char_row = 80'h2508FFFE3FFE0FF81010;
            511: result_char_row = 80'h42040000000008081010;
            512, 513: result_char_row = 80'h08200100102000800000;
            514: result_char_row = 80'h7EFC0100103E08800000;
            515: result_char_row = 80'h08200100102004800000;
            516: result_char_row = 80'h1C701100FDFC24800000;
            517: result_char_row = 80'h2AA81100110410800000;
            518: result_char_row = 80'hC82611F815FC10800000;
            519: result_char_row = 80'h00001100190400800000;
            520: result_char_row = 80'h3FF8110031FCFFFE0000;
            521: result_char_row = 80'h00001100D12401000000;
            522: result_char_row = 80'h00001100102001400000;
            523: result_char_row = 80'hFFFE110013FE02200000;
            524: result_char_row = 80'h01001100102004100000;
            525: result_char_row = 80'h11101100102008080000;
            526: result_char_row = 80'h2508FFFE502030040000;
            527: result_char_row = 80'h420400002020C0040000;
            528: result_char_row = 80'h08200100002010400000;
            529: result_char_row = 80'h08200100004010400000;
            530: result_char_row = 80'h7EFC010079FC3F7E0000;
            531: result_char_row = 80'h08200100490428900000;
            532: result_char_row = 80'h1C701100494445080000;
            533: result_char_row = 80'h2AA81100492481000000;
            534: result_char_row = 80'hC82611F849243FF80000;
            535: result_char_row = 80'h00001100490C21080000;
            536: result_char_row = 80'h3FF81100490021080000;
            537: result_char_row = 80'h0000110049FE21080000;
            538: result_char_row = 80'h0000110078023FF80000;
            539: result_char_row = 80'hFFFE1100480221080000;
            540: result_char_row = 80'h0100110003FA21080000;
            541: result_char_row = 80'h11101100000221080000;
            542: result_char_row = 80'h2508FFFE00143FF80000;
            543: result_char_row = 80'h42040000000820080000;
            544: result_char_row = 80'h01000200004008000000;
            545: result_char_row = 80'h01000200204009FC0000;
            546: result_char_row = 80'h02800200104010000000;
            547: result_char_row = 80'h04407FFC104020000000;
            548: result_char_row = 80'h08200400004048000000;
            549: result_char_row = 80'h30180900004008000000;
            550: result_char_row = 80'hCFE61100F07C13FE0000;
            551: result_char_row = 80'h00002100104030200000;
            552: result_char_row = 80'h00003FF8104050200000;
            553: result_char_row = 80'h7FFC0100104090200000;
            554: result_char_row = 80'h02000100104010200000;
            555: result_char_row = 80'h0400FFFE144010200000;
            556: result_char_row = 80'h08200100184010200000;
            557: result_char_row = 80'h10100100104010200000;
            558: result_char_row = 80'h3FF801000FFE10A00000;
            559: result_char_row = 80'h10080100000010400000;
            565: result_char_row = 80'h62080248044200000000;
            566: result_char_row = 80'h5208F248044200000000;
            567: result_char_row = 80'h4BF813F8084200000000;
            568: result_char_row = 80'h4A4410E0084200000000;
            569: result_char_row = 80'h4A481150104200000000;
            570, 602: result_char_row = 80'h6A301248104200000000;
            571: result_char_row = 80'h52201444104200000000;
            572: result_char_row = 80'h42101040102400000000;
            573: result_char_row = 80'h42882800101800000000;
            582, 598: result_char_row = 80'h5208F248424200000000;
            585: result_char_row = 80'h4A481150244200000000;
            599: result_char_row = 80'h4BF813F8024200000000;
            600: result_char_row = 80'h4A4410E0044200000000;
            601: result_char_row = 80'h4A481150084200000000;
            603: result_char_row = 80'h52201444204200000000;
            605: result_char_row = 80'h428828007E1800000000;
            614: result_char_row = 80'hC82611F8040093FE0000;
            616: result_char_row = 80'h3FF811000FF810800000;
            617: result_char_row = 80'h00001100108011FC0000;
            618: result_char_row = 80'h0000110010801C040000;
            619: result_char_row = 80'hFFFE11002080F0880000;
            620: result_char_row = 80'h01001100408050500000;
            621: result_char_row = 80'h11101100808010200000;
            622: result_char_row = 80'h2508FFFE3FFE10100000;
            623: result_char_row = 80'h42040000000010100000;
            default: result_char_row = {80{1'bx}};
        endcase
    end
endfunction

always @(*) begin : load_result_char
    integer address;
    for (address = 0; address < 624; address = address + 1)
        result_char[address] = result_char_row(address);
end

///////////////////////////
reg [15:0]font[0:229];
// Decode identical bitmap rows with one case value.
function [15:0] font_row;
    input integer address;
    begin
        case (address)
            0, 23, 46, 69, 92, 115, 138, 161,
            184, 207: font_row = 16'h0000;
            1, 47, 70, 137, 160, 185, 195, 206,
            208: font_row = 16'h0FF0;
            2, 21, 194, 196: font_row = 16'h1FF8;
            3, 20: font_row = 16'h1E78;
            4, 5, 18, 19, 126, 142, 210: font_row = 16'h3C3C;
            6, 7, 8, 9, 10, 11, 12, 13,
            14, 15, 16, 17, 86, 87, 132, 151,
            152, 153, 154, 155, 156, 190, 191: font_row = 16'h781E;
            22: font_row = 16'h0FE0;
            24, 172, 173: font_row = 16'h01C0;
            25: font_row = 16'h07C0;
            26: font_row = 16'h1FC0;
            27, 28, 29, 30, 31, 32, 33, 34,
            35, 36, 37, 38, 39, 40, 41, 42,
            43, 60, 174, 175: font_row = 16'h03C0;
            44, 80: font_row = 16'h07E0;
            45, 116, 117: font_row = 16'h1FFC;
            48: font_row = 16'h1EF8;
            49, 54, 73, 74, 75, 127, 187: font_row = 16'h383C;
            50, 88, 89, 134, 135, 198, 204, 211: font_row = 16'h783C;
            51, 133, 188, 212: font_row = 16'h781C;
            52, 53, 192: font_row = 16'h7C1C;
            55, 76, 77, 82, 83, 223, 224: font_row = 16'h003C;
            56, 57, 78: font_row = 16'h0078;
            58, 94, 95, 169: font_row = 16'h00F0;
            59: font_row = 16'h01E0;
            61, 177, 178, 179, 180, 181, 182, 183: font_row = 16'h0780;
            62: font_row = 16'h0F00;
            63: font_row = 16'h1E0E;
            64: font_row = 16'h3C0E;
            65: font_row = 16'h380E;
            66, 165: font_row = 16'h701C;
            67, 68, 148: font_row = 16'h7FFC;
            71, 124, 125, 186: font_row = 16'h3FF8;
            72: font_row = 16'h3878;
            79, 96, 97: font_row = 16'h01F0;
            81: font_row = 16'h01F8;
            84, 85, 128, 129, 130, 222: font_row = 16'h001E;
            90, 136, 205, 209: font_row = 16'h3EF8;
            91: font_row = 16'h1FF0;
            93, 109, 110, 111, 112, 168: font_row = 16'h0070;
            98: font_row = 16'h03F0;
            99, 100: font_row = 16'h0770;
            101: font_row = 16'h0E70;
            102: font_row = 16'h1E70;
            103: font_row = 16'h1C70;
            104, 105: font_row = 16'h3870;
            106: font_row = 16'h7070;
            107: font_row = 16'hF070;
            108: font_row = 16'hFFFF;
            113: font_row = 16'h00F8;
            114: font_row = 16'h07FE;
            118, 119, 120, 121, 122, 144: font_row = 16'h3800;
            123: font_row = 16'h39C0;
            131: font_row = 16'h381E;
            139: font_row = 16'h07F8;
            140: font_row = 16'h0FFC;
            141: font_row = 16'h1E3C;
            143: font_row = 16'h3838;
            145, 146: font_row = 16'h7800;
            147: font_row = 16'h7FF8;
            149: font_row = 16'h7E3C;
            150: font_row = 16'h7C1E;
            157: font_row = 16'h3C1E;
            158: font_row = 16'h3E3C;
            159: font_row = 16'h1F78;
            162, 163, 220: font_row = 16'h3FFE;
            164: font_row = 16'h381C;
            166: font_row = 16'h7038;
            167: font_row = 16'h7078;
            170, 171: font_row = 16'h00E0;
            176: font_row = 16'h0380;
            189, 199, 200, 201, 202, 203, 213, 214,
            215, 216: font_row = 16'h701E;
            193: font_row = 16'h3F3C;
            197: font_row = 16'h3CFC;
            217, 218: font_row = 16'h783E;
            219: font_row = 16'h7C7E;
            221: font_row = 16'h1FFE;
            225: font_row = 16'h183C;
            226, 227: font_row = 16'h3C78;
            228: font_row = 16'h3FF0;
            229: font_row = 16'h1FE0;
            default: font_row = {16{1'bx}};
        endcase
    end
endfunction

always @(*) begin : load_font
    integer address;
    for (address = 0; address < 230; address = address + 1)
        font[address] = font_row(address);
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