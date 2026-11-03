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

wire [7:0] gamma_18;
wire [7:0] gamma_19;
wire [7:0] gamma_20;
wire [7:0] gamma_21;
wire [7:0] gamma_22;
wire [7:0] gamma_23;
wire [7:0] gamma_24;
wire [7:0] gamma_25;
wire [7:0] gamma_26;
wire [7:0] gamma_27;
wire [7:0] gamma_28;
wire [7:0] gamma_29;
wire [7:0] gamma_30;
wire [7:0] gamma_31;
wire [7:0] gamma_32;
wire [7:0] gamma_33;
wire [7:0] gamma_34;
wire [7:0] gamma_35;
wire [7:0] gamma_36;
wire [7:0] gamma_37;
wire [7:0] gamma_38;
wire [7:0] gamma_39;
wire [7:0] gamma_40;
wire [7:0] gamma_41;
wire [7:0] gamma_42;
wire [7:0] gamma_43;
wire [7:0] gamma_44;
wire [7:0] gamma_45;
wire [7:0] gamma_46;
wire [7:0] gamma_47;
wire [7:0] gamma_48;
wire [7:0] gamma_49;
wire [7:0] gamma_50;
wire [7:0] gamma_51;
wire [7:0] gamma_52;
wire [7:0] gamma_53;
wire [7:0] gamma_54;
wire [7:0] gamma_55;
wire [7:0] gamma_56;
wire [7:0] gamma_57;
wire [7:0] gamma_58;
wire [7:0] gamma_59;
wire [7:0] gamma_60;
wire [7:0] gamma_61;
wire [7:0] gamma_62;
wire [7:0] gamma_63;
wire [7:0] gamma_64;
wire [7:0] gamma_65;
wire [7:0] gamma_66;
wire [7:0] gamma_67;
wire [7:0] gamma_68;
wire [7:0] gamma_69;
wire [7:0] gamma_70;
wire [7:0] gamma_71;
wire [7:0] gamma_72;
wire [7:0] gamma_73;
wire [7:0] gamma_74;
wire [7:0] gamma_75;
wire [7:0] gamma_76;
wire [7:0] gamma_77;
wire [7:0] gamma_78;
wire [7:0] gamma_79;
wire [7:0] gamma_80;
wire [7:0] gamma_81;
wire [7:0] gamma_82;
wire [7:0] gamma_83;
wire [7:0] gamma_84;
wire [7:0] gamma_85;
wire [7:0] gamma_86;
wire [7:0] gamma_87;
wire [7:0] gamma_88;
wire [7:0] gamma_89;
wire [7:0] gamma_90;

    reg                [   7: 0]        post_img_r_1               ;
    reg                [   7: 0]        post_img_g_1               ;
    reg                [   7: 0]        post_img_b_1               ;
    reg                                 post_img_vsync_1           ;
    reg                                 post_img_href_1            ;
    reg                [7:0]                 post_img_Y_dalay_1         ;
    reg [   9: 0]        per_data_1               ;

always@(posedge clk)
begin
post_img_r_1        <=per_img_r;
post_img_g_1        <=per_img_g;
post_img_b_1        <=per_img_b;
post_img_vsync_1    <=per_img_vsync;
post_img_href_1     <=per_img_href ;
post_img_Y_dalay_1  <=per_img_Y;
per_data_1        <=per_data;
end

always@(posedge clk)
begin
post_img_r        <=post_img_r_1      ;
post_img_g        <=post_img_g_1      ;
post_img_b        <=post_img_b_1      ;
post_img_vsync    <=post_img_vsync_1  ;
post_img_href     <=post_img_href_1   ;
post_img_Y_dalay  <=post_img_Y_dalay_1;
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) post_img_Y <= 8'd0;
    else if (post_img_href_1) begin
        // 旁路采用当前输入 Y。曲线分支采用已登记的曲线输出。
        if (per_data_1 <= 10'd570) post_img_Y <= per_img_Y;
        else if (per_data_1 <= 10'd1012) begin
            if (per_data_1 <= 10'd758) begin
                if (per_data_1 <= 10'd669) begin
                    if (per_data_1 <= 10'd625) begin
                        if (per_data_1 <= 10'd599) begin
                            if (per_data_1 <= 10'd586) begin
                                post_img_Y <= gamma_90;
                            end else begin
                                if (per_data_1 <= 10'd593) begin
                                    post_img_Y <= gamma_89;
                                end else begin
                                    post_img_Y <= gamma_87;
                                end
                            end
                        end else begin
                            if (per_data_1 <= 10'd612) begin
                                if (per_data_1 <= 10'd606) begin
                                    post_img_Y <= gamma_85;
                                end else begin
                                    post_img_Y <= gamma_83;
                                end
                            end else begin
                                if (per_data_1 <= 10'd618) begin
                                    post_img_Y <= gamma_81;
                                end else begin
                                    post_img_Y <= gamma_79;
                                end
                            end
                        end
                    end else begin
                        if (per_data_1 <= 10'd644) begin
                            if (per_data_1 <= 10'd631) begin
                                post_img_Y <= gamma_77;
                            end else begin
                                if (per_data_1 <= 10'd637) begin
                                    post_img_Y <= gamma_75;
                                end else begin
                                    post_img_Y <= gamma_74;
                                end
                            end
                        end else begin
                            if (per_data_1 <= 10'd656) begin
                                if (per_data_1 <= 10'd650) begin
                                    post_img_Y <= gamma_72;
                                end else begin
                                    post_img_Y <= gamma_70;
                                end
                            end else begin
                                if (per_data_1 <= 10'd663) begin
                                    post_img_Y <= gamma_69;
                                end else begin
                                    post_img_Y <= gamma_67;
                                end
                            end
                        end
                    end
                end else begin
                    if (per_data_1 <= 10'd714) begin
                        if (per_data_1 <= 10'd688) begin
                            if (per_data_1 <= 10'd675) begin
                                post_img_Y <= gamma_66;
                            end else begin
                                if (per_data_1 <= 10'd682) begin
                                    post_img_Y <= gamma_65;
                                end else begin
                                    post_img_Y <= gamma_63;
                                end
                            end
                        end else begin
                            if (per_data_1 <= 10'd701) begin
                                if (per_data_1 <= 10'd695) begin
                                    post_img_Y <= gamma_62;
                                end else begin
                                    post_img_Y <= gamma_60;
                                end
                            end else begin
                                if (per_data_1 <= 10'd707) begin
                                    post_img_Y <= gamma_59;
                                end else begin
                                    post_img_Y <= gamma_57;
                                end
                            end
                        end
                    end else begin
                        if (per_data_1 <= 10'd733) begin
                            if (per_data_1 <= 10'd720) begin
                                post_img_Y <= gamma_56;
                            end else begin
                                if (per_data_1 <= 10'd726) begin
                                    post_img_Y <= gamma_55;
                                end else begin
                                    post_img_Y <= gamma_53;
                                end
                            end
                        end else begin
                            if (per_data_1 <= 10'd745) begin
                                if (per_data_1 <= 10'd739) begin
                                    post_img_Y <= gamma_52;
                                end else begin
                                    post_img_Y <= gamma_51;
                                end
                            end else begin
                                if (per_data_1 <= 10'd752) begin
                                    post_img_Y <= gamma_50;
                                end else begin
                                    post_img_Y <= gamma_49;
                                end
                            end
                        end
                    end
                end
            end else begin
                if (per_data_1 <= 10'd854) begin
                    if (per_data_1 <= 10'd803) begin
                        if (per_data_1 <= 10'd777) begin
                            if (per_data_1 <= 10'd765) begin
                                post_img_Y <= gamma_47;
                            end else begin
                                if (per_data_1 <= 10'd771) begin
                                    post_img_Y <= gamma_46;
                                end else begin
                                    post_img_Y <= gamma_45;
                                end
                            end
                        end else begin
                            if (per_data_1 <= 10'd790) begin
                                if (per_data_1 <= 10'd784) begin
                                    post_img_Y <= gamma_44;
                                end else begin
                                    post_img_Y <= gamma_43;
                                end
                            end else begin
                                if (per_data_1 <= 10'd797) begin
                                    post_img_Y <= gamma_42;
                                end else begin
                                    post_img_Y <= gamma_41;
                                end
                            end
                        end
                    end else begin
                        if (per_data_1 <= 10'd822) begin
                            if (per_data_1 <= 10'd810) begin
                                post_img_Y <= gamma_40;
                            end else begin
                                if (per_data_1 <= 10'd816) begin
                                    post_img_Y <= gamma_39;
                                end else begin
                                    post_img_Y <= gamma_38;
                                end
                            end
                        end else begin
                            if (per_data_1 <= 10'd841) begin
                                if (per_data_1 <= 10'd835) begin
                                    post_img_Y <= gamma_37;
                                end else begin
                                    post_img_Y <= gamma_36;
                                end
                            end else begin
                                if (per_data_1 <= 10'd847) begin
                                    post_img_Y <= gamma_35;
                                end else begin
                                    post_img_Y <= gamma_34;
                                end
                            end
                        end
                    end
                end else begin
                    if (per_data_1 <= 10'd912) begin
                        if (per_data_1 <= 10'd882) begin
                            if (per_data_1 <= 10'd860) begin
                                post_img_Y <= gamma_33;
                            end else begin
                                if (per_data_1 <= 10'd867) begin
                                    post_img_Y <= gamma_32;
                                end else begin
                                    post_img_Y <= gamma_31;
                                end
                            end
                        end else begin
                            if (per_data_1 <= 10'd901) begin
                                if (per_data_1 <= 10'd895) begin
                                    post_img_Y <= gamma_30;
                                end else begin
                                    post_img_Y <= gamma_29;
                                end
                            end else begin
                                if (per_data_1 <= 10'd907) begin
                                    post_img_Y <= gamma_28;
                                end else begin
                                    post_img_Y <= gamma_27;
                                end
                            end
                        end
                    end else begin
                        if (per_data_1 <= 10'd957) begin
                            if (per_data_1 <= 10'd934) begin
                                if (per_data_1 <= 10'd918) begin
                                    post_img_Y <= gamma_26;
                                end else begin
                                    post_img_Y <= gamma_25;
                                end
                            end else begin
                                if (per_data_1 <= 10'd945) begin
                                    post_img_Y <= gamma_24;
                                end else begin
                                    post_img_Y <= gamma_23;
                                end
                            end
                        end else begin
                            if (per_data_1 <= 10'd982) begin
                                if (per_data_1 <= 10'd969) begin
                                    post_img_Y <= gamma_22;
                                end else begin
                                    post_img_Y <= gamma_21;
                                end
                            end else begin
                                if (per_data_1 <= 10'd994) begin
                                    post_img_Y <= gamma_20;
                                end else begin
                                    post_img_Y <= gamma_19;
                                end
                            end
                        end
                    end
                end
            end
        end else if (per_data_1 > 10'd1012) post_img_Y <= gamma_18;
    end
end

 Curve_Gamma_18 Curve_Gamma_18(
   .clk(clk),
    .Pre_Data (per_img_Y),
    .Post_Data(gamma_18)
 );

 Curve_Gamma_19 u_Curve_Gamma_19(
   .clk(clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_19  )
 );

 Curve_Gamma_20 u_Curve_Gamma_20(
   .clk(clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_20  )
 );

 Curve_Gamma_21 u_Curve_Gamma_21(
      .clk(clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_21  )
 );

 Curve_Gamma_22 u_Curve_Gamma_22(
   .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_22  )
 );

 Curve_Gamma_23 u_Curve_Gamma_23(
    .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_23  )
 );

 Curve_Gamma_24 u_Curve_Gamma_24(
    .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_24  )
 );

 Curve_Gamma_25 u_Curve_Gamma_25(
    .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_25  )
 );

 Curve_Gamma_26 u_Curve_Gamma_26(
    .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_26  )
 );

    Curve_Gamma_27 u_Curve_Gamma_27(
       .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_27  )
 );

    Curve_Gamma_28 u_Curve_Gamma_28(
       .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_28  )
 );

    Curve_Gamma_29 u_Curve_Gamma_29(
       .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_29  )
 );

    Curve_Gamma_30 u_Curve_Gamma_30(
       .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_30  )
 );

    Curve_Gamma_31 u_Curve_Gamma_31
    (
       .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_31  )
 );

    Curve_Gamma_32 u_Curve_Gamma_32(

      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_32  )
 );

    Curve_Gamma_33 u_Curve_Gamma_33(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_33  )
 );

    Curve_Gamma_34 u_Curve_Gamma_34(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_34  )
 );

    Curve_Gamma_35 u_Curve_Gamma_35(
.clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_35  )
 );

    Curve_Gamma_36 u_Curve_Gamma_36(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_36  )
 );

    Curve_Gamma_37 u_Curve_Gamma_37(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_37  )
 );

    Curve_Gamma_38 u_Curve_Gamma_38(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_38  )
 );

    Curve_Gamma_39 u_Curve_Gamma_39(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_39  )
 );

    Curve_Gamma_40 u_Curve_Gamma_40(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_40  )
 );

    Curve_Gamma_41 u_Curve_Gamma_41(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_41  )
 );

    Curve_Gamma_42 u_Curve_Gamma_42(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_42  )
 );

    Curve_Gamma_43 u_Curve_Gamma_43(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_43  )
 );

    Curve_Gamma_44 u_Curve_Gamma_44(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_44  )
 );

    Curve_Gamma_45 u_Curve_Gamma_45(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_45  )
 );

    Curve_Gamma_46 u_Curve_Gamma_46(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_46  )
 );

    Curve_Gamma_47 u_Curve_Gamma_47(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_47  )
 );

    Curve_Gamma_48 u_Curve_Gamma_48(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_48  )
 );

    Curve_Gamma_49 u_Curve_Gamma_49(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_49  )
 );

    Curve_Gamma_50 u_Curve_Gamma_50(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_50  )
 );

Curve_Gamma_51 u_Curve_Gamma_51(
   .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_51  )
 );

    Curve_Gamma_52 u_Curve_Gamma_52(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_52  )
 );

    Curve_Gamma_53 u_Curve_Gamma_53(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_53  )
 );

    Curve_Gamma_54 u_Curve_Gamma_54(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_54  )
 );

    Curve_Gamma_55 u_Curve_Gamma_55(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_55  )
 );

    Curve_Gamma_56 u_Curve_Gamma_56(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_56  )
 );

    Curve_Gamma_57 u_Curve_Gamma_57(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_57  )
 );

    Curve_Gamma_58 u_Curve_Gamma_58(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_58  )
 );

    Curve_Gamma_59 u_Curve_Gamma_59(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_59  )
 );

    Curve_Gamma_60 u_Curve_Gamma_60(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_60  )
 );

    Curve_Gamma_61 u_Curve_Gamma_61(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_61  )
 );

    Curve_Gamma_62 u_Curve_Gamma_62(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_62  )
 );

    Curve_Gamma_63 u_Curve_Gamma_63(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_63  )
 );

    Curve_Gamma_64 u_Curve_Gamma_64(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_64  )
 );

    Curve_Gamma_65 u_Curve_Gamma_65(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_65  )
 );

    Curve_Gamma_66 u_Curve_Gamma_66(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_66  )
 );

    Curve_Gamma_67 u_Curve_Gamma_67(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_67  )
 );

    Curve_Gamma_68 u_Curve_Gamma_68(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_68  )
 );

    Curve_Gamma_69 u_Curve_Gamma_69(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_69  )
 );

    Curve_Gamma_70 u_Curve_Gamma_70(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_70  )
 );

    Curve_Gamma_71 u_Curve_Gamma_71(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_71  )
 );

    Curve_Gamma_72 u_Curve_Gamma_72(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_72  )
 );

    Curve_Gamma_73 u_Curve_Gamma_73(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_73  )
 );

    Curve_Gamma_74 u_Curve_Gamma_74(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_74  )
 );

    Curve_Gamma_75 u_Curve_Gamma_75(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_75  )
 );

    Curve_Gamma_76 u_Curve_Gamma_76(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_76  )
 );

    Curve_Gamma_77 u_Curve_Gamma_77(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_77  )
 );

    Curve_Gamma_78 u_Curve_Gamma_78(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_78  )
 );

    Curve_Gamma_79 u_Curve_Gamma_79(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_79  )
 );

    Curve_Gamma_80 u_Curve_Gamma_80(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_80  )
 );

    Curve_Gamma_81 u_Curve_Gamma_81(
.clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_81  )
 );

    Curve_Gamma_82 u_Curve_Gamma_82(
 .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_82  )
 );

    Curve_Gamma_83 u_Curve_Gamma_83(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_83  )
 );

    Curve_Gamma_84 u_Curve_Gamma_84(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_84  )
 );

    Curve_Gamma_85 u_Curve_Gamma_85(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_85  )
 );

    Curve_Gamma_86 u_Curve_Gamma_86(
.clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_86  )
 );

    Curve_Gamma_87 u_Curve_Gamma_87(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_87  )
 );

    Curve_Gamma_88 u_Curve_Gamma_88(
      .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_88  )
 );

    Curve_Gamma_89 u_Curve_Gamma_89(
.clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_89  )
 );

    Curve_Gamma_90 u_Curve_Gamma_90(
    .clk           (clk),
    .Pre_Data  	( per_img_Y   ),
    .Post_Data 	( gamma_90  )
 );

endmodule