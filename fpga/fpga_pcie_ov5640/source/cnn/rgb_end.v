// 2026-11-03 注释校订。根据新旧亮度比例恢复 RGB。各通道沿用原饱和边界及复位方式。
`timescale 1ns / 1ps

module rgb_end(
    input                               clk                        ,
    input                               rst_n                      ,

    input              [   7: 0]        per_img_r                  ,
    input              [   7: 0]        per_img_g                  ,
    input              [   7: 0]        per_img_b                  ,
    input              [   7: 0]        per_img_Y                  ,
    input              [   7: 0]        per_img_Y_delay            ,

    output reg         [   7: 0]        post_img_out_r             ,
    output reg         [   7: 0]        post_img_out_g             ,
    output reg         [   7: 0]        post_img_out_b             ,

    input                               per_img_href               ,
    input                               per_img_vsync              ,
    output reg                          post_img_href              ,
    output reg                          post_img_vsync
);

localparam C_CLK_LATENCY = 2;

reg     [C_CLK_LATENCY-1:0]     matrix_img_vsync_r1;
reg     [C_CLK_LATENCY-1:0]     matrix_img_href_r1;

always @(posedge clk or negedge rst_n)
begin
    if(!rst_n)
    begin
        matrix_img_vsync_r1 <= {C_CLK_LATENCY{1'b0}};
        matrix_img_href_r1  <= {C_CLK_LATENCY{1'b0}};
    end
    else
    begin
        matrix_img_vsync_r1 <= {matrix_img_vsync_r1[C_CLK_LATENCY-2:0],per_img_vsync};
        matrix_img_href_r1  <= {matrix_img_href_r1[C_CLK_LATENCY-2:0],per_img_href};
    end
end

always @(posedge clk or negedge rst_n)
begin
    if(!rst_n)
    begin
        post_img_vsync <= 1'b0;
        post_img_href  <= 1'b0;
    end
    else
    begin
        post_img_vsync <= matrix_img_vsync_r1[C_CLK_LATENCY-2];
        post_img_href  <=  matrix_img_href_r1[C_CLK_LATENCY-2];
    end
end

wire [23:0] input_rgb = {per_img_r, per_img_g, per_img_b};
wire [23:0] saturated_rgb;

reg [7:0] per_img_Y_delay_reg = 0;
wire [31:0] Recip;
always @(posedge clk)
    per_img_Y_delay_reg <= per_img_Y_delay;

Reciprocal_cnn u_Reciprocal_cnn (
    .clk(clk),
    .Average(per_img_Y_delay_reg),
    .Recip(Recip)
);

function [7:0] saturate;
    input [47:0] scaled;
    input red_channel;
    begin
        if (red_channel ? scaled[47:31] >= 17'd255 : scaled[47:31] > 17'd255)
            saturate = 8'd255;
        else
            saturate = scaled[38:31];
    end
endfunction

genvar channel;
generate
    for (channel = 0; channel < 3; channel = channel + 1) begin : rgb_channel
        reg [15:0] product;
        reg [15:0] delayed_product;
        reg [47:0] scaled;
        reg [7:0] saturated;

        always @(posedge clk or negedge rst_n) begin
            if (!rst_n)
                product <= 16'd0;
            else if (per_img_href)
                product <= input_rgb[channel * 8 +: 8] * per_img_Y;
        end

        always @(posedge clk) begin
            delayed_product <= product;
            if (!rst_n)
                scaled <= 48'd0;
            else
                scaled <= Recip * delayed_product;
        end

        // 绿色饱和级使用同步复位。红色和蓝色饱和级在复位拉低时立即清零。
        if (channel == 1) begin : sync_green
            always @(posedge clk) begin
                if (!rst_n)
                    saturated <= 8'd0;
                else
                    saturated <= saturate(scaled, 1'b0);
            end
        end else begin : async_blue_red
            always @(posedge clk or negedge rst_n) begin
                if (!rst_n)
                    saturated <= 8'd0;
                else
                    saturated <= saturate(scaled, channel == 2);
            end
        end
        assign saturated_rgb[channel * 8 +: 8] = saturated;
    end
endgenerate

always @(posedge clk) begin
    if (!rst_n)
        post_img_out_r <= 8'd0;
    else
        post_img_out_r <= saturated_rgb[23:16];

    // 末级绿、蓝输出每拍采样，复位拍也沿用这一规则。
    {post_img_out_g, post_img_out_b} <= saturated_rgb[15:0];
end

endmodule

// 倒数表输出采用 1 位整数和 31 位小数，保留对时钟变化的原触发方式。
module Reciprocal_cnn(
    input clk,
    input wire[7:0]     Average,
    output reg[31:0]    Recip
);

    always @ (clk)
        begin
            case(Average)
                8'd0: Recip<= 32'h00000000;
                8'd1: Recip<= 32'h80000000;
                8'd2: Recip<= 32'h40000000;
                8'd3: Recip<= 32'h2aaaaaaa;
                8'd4: Recip<= 32'h20000000;
                8'd5: Recip<= 32'h19999999;
                8'd6: Recip<= 32'h15555555;
                8'd7: Recip<= 32'h12492492;
                8'd8: Recip<= 32'h10000000;
                8'd9: Recip<= 32'h0e38e38e;
                8'd10: Recip <= 32'h0ccccccc;
                8'd11: Recip <= 32'h0ba2e8ba;
                8'd12: Recip <= 32'h0aaaaaaa;
                8'd13: Recip <= 32'h09d89d89;
                8'd14: Recip <= 32'h09249249;
                8'd15: Recip <= 32'h08888888;
                8'd16: Recip <= 32'h08000000;
                8'd17: Recip <= 32'h07878787;
                8'd18: Recip <= 32'h071c71c7;
                8'd19: Recip <= 32'h06bca1af;
                8'd20: Recip <= 32'h06666666;
                8'd21: Recip <= 32'h06186186;
                8'd22: Recip <= 32'h05d1745d;
                8'd23: Recip <= 32'h0590b216;
                8'd24: Recip <= 32'h05555555;
                8'd25: Recip <= 32'h051eb851;
                8'd26: Recip <= 32'h04ec4ec4;
                8'd27: Recip <= 32'h04bda12f;
                8'd28: Recip <= 32'h04924924;
                8'd29: Recip <= 32'h0469ee58;
                8'd30: Recip <= 32'h04444444;
                8'd31: Recip <= 32'h04210842;
                8'd32: Recip <= 32'h04000000;
                8'd33: Recip <= 32'h03e0f83e;
                8'd34: Recip <= 32'h03c3c3c3;
                8'd35: Recip <= 32'h03a83a83;
                8'd36: Recip <= 32'h038e38e3;
                8'd37: Recip <= 32'h03759f22;
                8'd38: Recip <= 32'h035e50d7;
                8'd39: Recip <= 32'h03483483;
                8'd40: Recip <= 32'h03333333;
                8'd41: Recip <= 32'h031f3831;
                8'd42: Recip <= 32'h030c30c3;
                8'd43: Recip <= 32'h02fa0be8;
                8'd44: Recip <= 32'h02e8ba2e;
                8'd45: Recip <= 32'h02d82d82;
                8'd46: Recip <= 32'h02c8590b;
                8'd47: Recip <= 32'h02b93105;
                8'd48: Recip <= 32'h02aaaaaa;
                8'd49: Recip <= 32'h029cbc14;
                8'd50: Recip <= 32'h028f5c28;
                8'd51: Recip <= 32'h02828282;
                8'd52: Recip <= 32'h02762762;
                8'd53: Recip <= 32'h026a439f;
                8'd54: Recip <= 32'h025ed097;
                8'd55: Recip <= 32'h0253c825;
                8'd56: Recip <= 32'h02492492;
                8'd57: Recip <= 32'h023ee08f;
                8'd58: Recip <= 32'h0234f72c;
                8'd59: Recip <= 32'h022b63cb;
                8'd60: Recip <= 32'h02222222;
                8'd61: Recip <= 32'h02192e29;
                8'd62: Recip <= 32'h02108421;
                8'd63: Recip <= 32'h02082082;
                8'd64: Recip <= 32'h02000000;
                8'd65: Recip <= 32'h01f81f81;
                8'd66: Recip <= 32'h01f07c1f;
                8'd67: Recip <= 32'h01e9131a;
                8'd68: Recip <= 32'h01e1e1e1;
                8'd69: Recip <= 32'h01dae607;
                8'd70: Recip <= 32'h01d41d41;
                8'd71: Recip <= 32'h01cd8568;
                8'd72: Recip <= 32'h01c71c71;
                8'd73: Recip <= 32'h01c0e070;
                8'd74: Recip <= 32'h01bacf91;
                8'd75: Recip <= 32'h01b4e81b;
                8'd76: Recip <= 32'h01af286b;
                8'd77: Recip <= 32'h01a98ef6;
                8'd78: Recip <= 32'h01a41a41;
                8'd79: Recip <= 32'h019ec8e9;
                8'd80: Recip <= 32'h01999999;
                8'd81: Recip <= 32'h01948b0f;
                8'd82: Recip <= 32'h018f9c18;
                8'd83: Recip <= 32'h018acb90;
                8'd84: Recip <= 32'h01861861;
                8'd85: Recip <= 32'h01818181;
                8'd86: Recip <= 32'h017d05f4;
                8'd87: Recip <= 32'h0178a4c8;
                8'd88: Recip <= 32'h01745d17;
                8'd89: Recip <= 32'h01702e05;
                8'd90: Recip <= 32'h016c16c1;
                8'd91: Recip <= 32'h01681681;
                8'd92: Recip <= 32'h01642c85;
                8'd93: Recip <= 32'h01605816;
                8'd94: Recip <= 32'h015c9882;
                8'd95: Recip <= 32'h0158ed23;
                8'd96: Recip <= 32'h01555555;
                8'd97: Recip <= 32'h0151d07e;
                8'd98: Recip <= 32'h014e5e0a;
                8'd99: Recip <= 32'h014afd6a;
                8'd100: Recip<= 32'h0147ae14;
                8'd101: Recip<= 32'h01446f86;
                8'd102: Recip<= 32'h01414141;
                8'd103: Recip<= 32'h013e22cb;
                8'd104: Recip<= 32'h013b13b1;
                8'd105: Recip<= 32'h01381381;
                8'd106: Recip<= 32'h013521cf;
                8'd107: Recip<= 32'h01323e34;
                8'd108: Recip<= 32'h012f684b;
                8'd109: Recip<= 32'h012c9fb4;
                8'd110: Recip<= 32'h0129e412;
                8'd111: Recip<= 32'h0127350b;
                8'd112: Recip<= 32'h01249249;
                8'd113: Recip<= 32'h0121fb78;
                8'd114: Recip<= 32'h011f7047;
                8'd115: Recip<= 32'h011cf06a;
                8'd116: Recip<= 32'h011a7b96;
                8'd117: Recip<= 32'h01181181;
                8'd118: Recip<= 32'h0115b1e5;
                8'd119: Recip<= 32'h01135c81;
                8'd120: Recip<= 32'h01111111;
                8'd121: Recip<= 32'h010ecf56;
                8'd122: Recip<= 32'h010c9714;
                8'd123: Recip<= 32'h010a6810;
                8'd124: Recip<= 32'h01084210;
                8'd125: Recip<= 32'h010624dd;
                8'd126: Recip<= 32'h01041041;
                8'd127: Recip<= 32'h01020408;
                8'd128: Recip<= 32'h01000000;
                8'd129: Recip<= 32'h00fe03f8;
                8'd130: Recip<= 32'h00fc0fc0;
                8'd131: Recip<= 32'h00fa232c;
                8'd132: Recip<= 32'h00f83e0f;
                8'd133: Recip<= 32'h00f6603d;
                8'd134: Recip<= 32'h00f4898d;
                8'd135: Recip<= 32'h00f2b9d6;
                8'd136: Recip<= 32'h00f0f0f0;
                8'd137: Recip<= 32'h00ef2eb7;
                8'd138: Recip<= 32'h00ed7303;
                8'd139: Recip<= 32'h00ebbdb2;
                8'd140: Recip<= 32'h00ea0ea0;
                8'd141: Recip<= 32'h00e865ac;
                8'd142: Recip<= 32'h00e6c2b4;
                8'd143: Recip<= 32'h00e52598;
                8'd144: Recip<= 32'h00e38e38;
                8'd145: Recip<= 32'h00e1fc78;
                8'd146: Recip<= 32'h00e07038;
                8'd147: Recip<= 32'h00dee95c;
                8'd148: Recip<= 32'h00dd67c8;
                8'd149: Recip<= 32'h00dbeb61;
                8'd150: Recip<= 32'h00da740d;
                8'd151: Recip<= 32'h00d901b2;
                8'd152: Recip<= 32'h00d79435;
                8'd153: Recip<= 32'h00d62b80;
                8'd154: Recip<= 32'h00d4c77b;
                8'd155: Recip<= 32'h00d3680d;
                8'd156: Recip<= 32'h00d20d20;
                8'd157: Recip<= 32'h00d0b69f;
                8'd158: Recip<= 32'h00cf6474;
                8'd159: Recip<= 32'h00ce168a;
                8'd160: Recip<= 32'h00cccccc;
                8'd161: Recip<= 32'h00cb8727;
                8'd162: Recip<= 32'h00ca4587;
                8'd163: Recip<= 32'h00c907da;
                8'd164: Recip<= 32'h00c7ce0c;
                8'd165: Recip<= 32'h00c6980c;
                8'd166: Recip<= 32'h00c565c8;
                8'd167: Recip<= 32'h00c4372f;
                8'd168: Recip<= 32'h00c30c30;
                8'd169: Recip<= 32'h00c1e4bb;
                8'd170: Recip<= 32'h00c0c0c0;
                8'd171: Recip<= 32'h00bfa02f;
                8'd172: Recip<= 32'h00be82fa;
                8'd173: Recip<= 32'h00bd6910;
                8'd174: Recip<= 32'h00bc5264;
                8'd175: Recip<= 32'h00bb3ee7;
                8'd176: Recip<= 32'h00ba2e8b;
                8'd177: Recip<= 32'h00b92143;
                8'd178: Recip<= 32'h00b81702;
                8'd179: Recip<= 32'h00b70fbb;
                8'd180: Recip<= 32'h00b60b60;
                8'd181: Recip<= 32'h00b509e6;
                8'd182: Recip<= 32'h00b40b40;
                8'd183: Recip<= 32'h00b30f63;
                8'd184: Recip<= 32'h00b21642;
                8'd185: Recip<= 32'h00b11fd3;
                8'd186: Recip<= 32'h00b02c0b;
                8'd187: Recip<= 32'h00af3add;
                8'd188: Recip<= 32'h00ae4c41;
                8'd189: Recip<= 32'h00ad602b;
                8'd190: Recip<= 32'h00ac7691;
                8'd191: Recip<= 32'h00ab8f69;
                8'd192: Recip<= 32'h00aaaaaa;
                8'd193: Recip<= 32'h00a9c84a;
                8'd194: Recip<= 32'h00a8e83f;
                8'd195: Recip<= 32'h00a80a80;
                8'd196: Recip<= 32'h00a72f05;
                8'd197: Recip<= 32'h00a655c4;
                8'd198: Recip<= 32'h00a57eb5;
                8'd199: Recip<= 32'h00a4a9cf;
                8'd200: Recip<= 32'h00a3d70a;
                8'd201: Recip<= 32'h00a3065e;
                8'd202: Recip<= 32'h00a237c3;
                8'd203: Recip<= 32'h00a16b31;
                8'd204: Recip<= 32'h00a0a0a0;
                8'd205: Recip<= 32'h009fd809;
                8'd206: Recip<= 32'h009f1165;
                8'd207: Recip<= 32'h009e4cad;
                8'd208: Recip<= 32'h009d89d8;
                8'd209: Recip<= 32'h009cc8e1;
                8'd210: Recip<= 32'h009c09c0;
                8'd211: Recip<= 32'h009b4c6f;
                8'd212: Recip<= 32'h009a90e7;
                8'd213: Recip<= 32'h0099d722;
                8'd214: Recip<= 32'h00991f1a;
                8'd215: Recip<= 32'h009868c8;
                8'd216: Recip<= 32'h0097b425;
                8'd217: Recip<= 32'h0097012e;
                8'd218: Recip<= 32'h00964fda;
                8'd219: Recip<= 32'h0095a025;
                8'd220: Recip<= 32'h0094f209;
                8'd221: Recip<= 32'h00944580;
                8'd222: Recip<= 32'h00939a85;
                8'd223: Recip<= 32'h0092f113;
                8'd224: Recip<= 32'h00924924;
                8'd225: Recip<= 32'h0091a2b3;
                8'd226: Recip<= 32'h0090fdbc;
                8'd227: Recip<= 32'h00905a38;
                8'd228: Recip<= 32'h008fb823;
                8'd229: Recip<= 32'h008f1779;
                8'd230: Recip<= 32'h008e7835;
                8'd231: Recip<= 32'h008dda52;
                8'd232: Recip<= 32'h008d3dcb;
                8'd233: Recip<= 32'h008ca29c;
                8'd234: Recip<= 32'h008c08c0;
                8'd235: Recip<= 32'h008b7034;
                8'd236: Recip<= 32'h008ad8f2;
                8'd237: Recip<= 32'h008a42f8;
                8'd238: Recip<= 32'h0089ae40;
                8'd239: Recip<= 32'h00891ac7;
                8'd240: Recip<= 32'h00888888;
                8'd241: Recip<= 32'h0087f780;
                8'd242: Recip<= 32'h008767ab;
                8'd243: Recip<= 32'h0086d905;
                8'd244: Recip<= 32'h00864b8a;
                8'd245: Recip<= 32'h0085bf37;
                8'd246: Recip<= 32'h00853408;
                8'd247: Recip<= 32'h0084a9f9;
                8'd248: Recip<= 32'h00842108;
                8'd249: Recip<= 32'h00839930;
                8'd250: Recip<= 32'h0083126e;
                8'd251: Recip<= 32'h00828cbf;
                8'd252: Recip<= 32'h00820820;
                8'd253: Recip<= 32'h0081848d;
                8'd254: Recip<= 32'h00810204;
                8'd255: Recip<= 32'h00808080;
                default:Recip<= 32'h00000000;
            endcase
        end
endmodule
