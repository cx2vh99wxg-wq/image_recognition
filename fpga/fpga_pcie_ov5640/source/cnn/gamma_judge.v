// 2026-11-03 注释校订。按延迟评分选择 Gamma 曲线。低分旁路读取当前亮度，消隐期间保留亮度输出。
// VSCODE plug-in author : Jiang Percy

//****************************************Copyright (c)***********************************//
// Copyright(C)            company
// All rights reserved

// 卷积流水线：Last modified Date: 2024/10/29 10:08:47

// 卷积流水线：Created by: 黄煜宏
// 参数记录为 Created date: 2024/10/29 10:08:47。
// 卷积流水线：mail : 1807092357@qq.com

module gamma_judge
(
    input                               clk                        ,
    input                               rst_n                      ,
    input              [   7: 0]        per_img_r                  ,
    input              [   7: 0]        per_img_g                  ,
    input              [   7: 0]        per_img_b                  ,
    output reg         [   7: 0]        post_img_r                 ,
    output reg         [   7: 0]        post_img_g                 ,
    output reg         [   7: 0]        post_img_b                 ,

    input                               per_img_vsync              ,
    input                               per_img_href               ,
    input              [   7: 0]        per_img_Y                  ,

    output reg                          post_img_vsync             ,
    output reg                          post_img_href              ,
    output reg         [   7: 0]        post_img_Y                 ,
    output reg         [   7: 0]        post_img_Y_dalay           ,

    input              [   9: 0]        per_data

);


wire [7:0] curve_luminance [18:90];

    reg                [   7: 0]        post_img_r_1               ;
    reg                [   7: 0]        post_img_g_1               ;
    reg                [   7: 0]        post_img_b_1               ;
    reg                                 vsync_stage1           ;
    reg                                 href_stage1            ;
    reg                [7:0]                 original_y_stage1         ;
    reg [   9: 0]        score_stage1               ;

always@(posedge clk)
begin
post_img_r_1        <=per_img_r;
post_img_g_1        <=per_img_g;
post_img_b_1        <=per_img_b;
vsync_stage1    <=per_img_vsync;
href_stage1     <=per_img_href ;
original_y_stage1  <=per_img_Y;
score_stage1        <=per_data;
end

always@(posedge clk)
begin
post_img_r        <=post_img_r_1      ;
post_img_g        <=post_img_g_1      ;
post_img_b        <=post_img_b_1      ;
post_img_vsync    <=vsync_stage1  ;
post_img_href     <=href_stage1   ;
post_img_Y_dalay  <=original_y_stage1;
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) post_img_Y <= 8'd0;
    else if (href_stage1) begin
        // 旁路采用当前输入 Y。曲线分支采用已登记的曲线输出。
        if (score_stage1 <= 10'd570) post_img_Y <= per_img_Y;
        else if (score_stage1 <= 10'd1012) begin
            if (score_stage1 <= 10'd758) begin
                if (score_stage1 <= 10'd669) begin
                    if (score_stage1 <= 10'd625) begin
                        if (score_stage1 <= 10'd599) begin
                            if (score_stage1 <= 10'd586) begin
                                post_img_Y <= curve_luminance[90];
                            end else begin
                                if (score_stage1 <= 10'd593) begin
                                    post_img_Y <= curve_luminance[89];
                                end else begin
                                    post_img_Y <= curve_luminance[87];
                                end
                            end
                        end else begin
                            if (score_stage1 <= 10'd612) begin
                                if (score_stage1 <= 10'd606) begin
                                    post_img_Y <= curve_luminance[85];
                                end else begin
                                    post_img_Y <= curve_luminance[83];
                                end
                            end else begin
                                if (score_stage1 <= 10'd618) begin
                                    post_img_Y <= curve_luminance[81];
                                end else begin
                                    post_img_Y <= curve_luminance[79];
                                end
                            end
                        end
                    end else begin
                        if (score_stage1 <= 10'd644) begin
                            if (score_stage1 <= 10'd631) begin
                                post_img_Y <= curve_luminance[77];
                            end else begin
                                if (score_stage1 <= 10'd637) begin
                                    post_img_Y <= curve_luminance[75];
                                end else begin
                                    post_img_Y <= curve_luminance[74];
                                end
                            end
                        end else begin
                            if (score_stage1 <= 10'd656) begin
                                if (score_stage1 <= 10'd650) begin
                                    post_img_Y <= curve_luminance[72];
                                end else begin
                                    post_img_Y <= curve_luminance[70];
                                end
                            end else begin
                                if (score_stage1 <= 10'd663) begin
                                    post_img_Y <= curve_luminance[69];
                                end else begin
                                    post_img_Y <= curve_luminance[67];
                                end
                            end
                        end
                    end
                end else begin
                    if (score_stage1 <= 10'd714) begin
                        if (score_stage1 <= 10'd688) begin
                            if (score_stage1 <= 10'd675) begin
                                post_img_Y <= curve_luminance[66];
                            end else begin
                                if (score_stage1 <= 10'd682) begin
                                    post_img_Y <= curve_luminance[65];
                                end else begin
                                    post_img_Y <= curve_luminance[63];
                                end
                            end
                        end else begin
                            if (score_stage1 <= 10'd701) begin
                                if (score_stage1 <= 10'd695) begin
                                    post_img_Y <= curve_luminance[62];
                                end else begin
                                    post_img_Y <= curve_luminance[60];
                                end
                            end else begin
                                if (score_stage1 <= 10'd707) begin
                                    post_img_Y <= curve_luminance[59];
                                end else begin
                                    post_img_Y <= curve_luminance[57];
                                end
                            end
                        end
                    end else begin
                        if (score_stage1 <= 10'd733) begin
                            if (score_stage1 <= 10'd720) begin
                                post_img_Y <= curve_luminance[56];
                            end else begin
                                if (score_stage1 <= 10'd726) begin
                                    post_img_Y <= curve_luminance[55];
                                end else begin
                                    post_img_Y <= curve_luminance[53];
                                end
                            end
                        end else begin
                            if (score_stage1 <= 10'd745) begin
                                if (score_stage1 <= 10'd739) begin
                                    post_img_Y <= curve_luminance[52];
                                end else begin
                                    post_img_Y <= curve_luminance[51];
                                end
                            end else begin
                                if (score_stage1 <= 10'd752) begin
                                    post_img_Y <= curve_luminance[50];
                                end else begin
                                    post_img_Y <= curve_luminance[49];
                                end
                            end
                        end
                    end
                end
            end else begin
                if (score_stage1 <= 10'd854) begin
                    if (score_stage1 <= 10'd803) begin
                        if (score_stage1 <= 10'd777) begin
                            if (score_stage1 <= 10'd765) begin
                                post_img_Y <= curve_luminance[47];
                            end else begin
                                if (score_stage1 <= 10'd771) begin
                                    post_img_Y <= curve_luminance[46];
                                end else begin
                                    post_img_Y <= curve_luminance[45];
                                end
                            end
                        end else begin
                            if (score_stage1 <= 10'd790) begin
                                if (score_stage1 <= 10'd784) begin
                                    post_img_Y <= curve_luminance[44];
                                end else begin
                                    post_img_Y <= curve_luminance[43];
                                end
                            end else begin
                                if (score_stage1 <= 10'd797) begin
                                    post_img_Y <= curve_luminance[42];
                                end else begin
                                    post_img_Y <= curve_luminance[41];
                                end
                            end
                        end
                    end else begin
                        if (score_stage1 <= 10'd822) begin
                            if (score_stage1 <= 10'd810) begin
                                post_img_Y <= curve_luminance[40];
                            end else begin
                                if (score_stage1 <= 10'd816) begin
                                    post_img_Y <= curve_luminance[39];
                                end else begin
                                    post_img_Y <= curve_luminance[38];
                                end
                            end
                        end else begin
                            if (score_stage1 <= 10'd841) begin
                                if (score_stage1 <= 10'd835) begin
                                    post_img_Y <= curve_luminance[37];
                                end else begin
                                    post_img_Y <= curve_luminance[36];
                                end
                            end else begin
                                if (score_stage1 <= 10'd847) begin
                                    post_img_Y <= curve_luminance[35];
                                end else begin
                                    post_img_Y <= curve_luminance[34];
                                end
                            end
                        end
                    end
                end else begin
                    if (score_stage1 <= 10'd912) begin
                        if (score_stage1 <= 10'd882) begin
                            if (score_stage1 <= 10'd860) begin
                                post_img_Y <= curve_luminance[33];
                            end else begin
                                if (score_stage1 <= 10'd867) begin
                                    post_img_Y <= curve_luminance[32];
                                end else begin
                                    post_img_Y <= curve_luminance[31];
                                end
                            end
                        end else begin
                            if (score_stage1 <= 10'd901) begin
                                if (score_stage1 <= 10'd895) begin
                                    post_img_Y <= curve_luminance[30];
                                end else begin
                                    post_img_Y <= curve_luminance[29];
                                end
                            end else begin
                                if (score_stage1 <= 10'd907) begin
                                    post_img_Y <= curve_luminance[28];
                                end else begin
                                    post_img_Y <= curve_luminance[27];
                                end
                            end
                        end
                    end else begin
                        if (score_stage1 <= 10'd957) begin
                            if (score_stage1 <= 10'd934) begin
                                if (score_stage1 <= 10'd918) begin
                                    post_img_Y <= curve_luminance[26];
                                end else begin
                                    post_img_Y <= curve_luminance[25];
                                end
                            end else begin
                                if (score_stage1 <= 10'd945) begin
                                    post_img_Y <= curve_luminance[24];
                                end else begin
                                    post_img_Y <= curve_luminance[23];
                                end
                            end
                        end else begin
                            if (score_stage1 <= 10'd982) begin
                                if (score_stage1 <= 10'd969) begin
                                    post_img_Y <= curve_luminance[22];
                                end else begin
                                    post_img_Y <= curve_luminance[21];
                                end
                            end else begin
                                if (score_stage1 <= 10'd994) begin
                                    post_img_Y <= curve_luminance[20];
                                end else begin
                                    post_img_Y <= curve_luminance[19];
                                end
                            end
                        end
                    end
                end
            end
        end else if (score_stage1 > 10'd1012) post_img_Y <= curve_luminance[18];
    end
end

 Curve_Gamma_18 Curve_Gamma_18(
   .clk(clk),
    .Pre_Data (per_img_Y),
    .Post_Data(curve_luminance[18])
 );

 Curve_Gamma_19 u_Curve_Gamma_19(
   .clk(clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[19]  )
 );

 Curve_Gamma_20 u_Curve_Gamma_20(
   .clk(clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[20]  )
 );

 Curve_Gamma_21 u_Curve_Gamma_21(
      .clk(clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[21]  )
 );

 Curve_Gamma_22 u_Curve_Gamma_22(
   .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[22]  )
 );

 Curve_Gamma_23 u_Curve_Gamma_23(
    .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[23]  )
 );

 Curve_Gamma_24 u_Curve_Gamma_24(
    .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[24]  )
 );

 Curve_Gamma_25 u_Curve_Gamma_25(
    .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[25]  )
 );

 Curve_Gamma_26 u_Curve_Gamma_26(
    .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[26]  )
 );

    Curve_Gamma_27 u_Curve_Gamma_27(
       .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[27]  )
 );

    Curve_Gamma_28 u_Curve_Gamma_28(
       .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[28]  )
 );

    Curve_Gamma_29 u_Curve_Gamma_29(
       .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[29]  )
 );

    Curve_Gamma_30 u_Curve_Gamma_30(
       .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[30]  )
 );

    Curve_Gamma_31 u_Curve_Gamma_31
    (
       .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[31]  )
 );

    Curve_Gamma_32 u_Curve_Gamma_32(

      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[32]  )
 );

    Curve_Gamma_33 u_Curve_Gamma_33(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[33]  )
 );

    Curve_Gamma_34 u_Curve_Gamma_34(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[34]  )
 );

    Curve_Gamma_35 u_Curve_Gamma_35(
.clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[35]  )
 );

    Curve_Gamma_36 u_Curve_Gamma_36(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[36]  )
 );

    Curve_Gamma_37 u_Curve_Gamma_37(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[37]  )
 );

    Curve_Gamma_38 u_Curve_Gamma_38(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[38]  )
 );

    Curve_Gamma_39 u_Curve_Gamma_39(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[39]  )
 );

    Curve_Gamma_40 u_Curve_Gamma_40(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[40]  )
 );

    Curve_Gamma_41 u_Curve_Gamma_41(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[41]  )
 );

    Curve_Gamma_42 u_Curve_Gamma_42(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[42]  )
 );

    Curve_Gamma_43 u_Curve_Gamma_43(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[43]  )
 );

    Curve_Gamma_44 u_Curve_Gamma_44(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[44]  )
 );

    Curve_Gamma_45 u_Curve_Gamma_45(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[45]  )
 );

    Curve_Gamma_46 u_Curve_Gamma_46(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[46]  )
 );

    Curve_Gamma_47 u_Curve_Gamma_47(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[47]  )
 );

    Curve_Gamma_48 u_Curve_Gamma_48(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[48]  )
 );

    Curve_Gamma_49 u_Curve_Gamma_49(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[49]  )
 );

    Curve_Gamma_50 u_Curve_Gamma_50(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[50]  )
 );

Curve_Gamma_51 u_Curve_Gamma_51(
   .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[51]  )
 );

    Curve_Gamma_52 u_Curve_Gamma_52(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[52]  )
 );

    Curve_Gamma_53 u_Curve_Gamma_53(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[53]  )
 );

    Curve_Gamma_54 u_Curve_Gamma_54(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[54]  )
 );

    Curve_Gamma_55 u_Curve_Gamma_55(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[55]  )
 );

    Curve_Gamma_56 u_Curve_Gamma_56(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[56]  )
 );

    Curve_Gamma_57 u_Curve_Gamma_57(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[57]  )
 );

    Curve_Gamma_58 u_Curve_Gamma_58(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[58]  )
 );

    Curve_Gamma_59 u_Curve_Gamma_59(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[59]  )
 );

    Curve_Gamma_60 u_Curve_Gamma_60(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[60]  )
 );

    Curve_Gamma_61 u_Curve_Gamma_61(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[61]  )
 );

    Curve_Gamma_62 u_Curve_Gamma_62(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[62]  )
 );

    Curve_Gamma_63 u_Curve_Gamma_63(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[63]  )
 );

    Curve_Gamma_64 u_Curve_Gamma_64(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[64]  )
 );

    Curve_Gamma_65 u_Curve_Gamma_65(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[65]  )
 );

    Curve_Gamma_66 u_Curve_Gamma_66(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[66]  )
 );

    Curve_Gamma_67 u_Curve_Gamma_67(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[67]  )
 );

    Curve_Gamma_68 u_Curve_Gamma_68(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[68]  )
 );

    Curve_Gamma_69 u_Curve_Gamma_69(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[69]  )
 );

    Curve_Gamma_70 u_Curve_Gamma_70(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[70]  )
 );

    Curve_Gamma_71 u_Curve_Gamma_71(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[71]  )
 );

    Curve_Gamma_72 u_Curve_Gamma_72(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[72]  )
 );

    Curve_Gamma_73 u_Curve_Gamma_73(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[73]  )
 );

    Curve_Gamma_74 u_Curve_Gamma_74(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[74]  )
 );

    Curve_Gamma_75 u_Curve_Gamma_75(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[75]  )
 );

    Curve_Gamma_76 u_Curve_Gamma_76(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[76]  )
 );

    Curve_Gamma_77 u_Curve_Gamma_77(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[77]  )
 );

    Curve_Gamma_78 u_Curve_Gamma_78(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[78]  )
 );

    Curve_Gamma_79 u_Curve_Gamma_79(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[79]  )
 );

    Curve_Gamma_80 u_Curve_Gamma_80(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[80]  )
 );

    Curve_Gamma_81 u_Curve_Gamma_81(
.clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[81]  )
 );

    Curve_Gamma_82 u_Curve_Gamma_82(
 .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[82]  )
 );

    Curve_Gamma_83 u_Curve_Gamma_83(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[83]  )
 );

    Curve_Gamma_84 u_Curve_Gamma_84(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[84]  )
 );

    Curve_Gamma_85 u_Curve_Gamma_85(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[85]  )
 );

    Curve_Gamma_86 u_Curve_Gamma_86(
.clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[86]  )
 );

    Curve_Gamma_87 u_Curve_Gamma_87(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[87]  )
 );

    Curve_Gamma_88 u_Curve_Gamma_88(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[88]  )
 );

    Curve_Gamma_89 u_Curve_Gamma_89(
.clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[89]  )
 );

    Curve_Gamma_90 u_Curve_Gamma_90(
    .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( curve_luminance[90]  )
 );

endmodule