//****************************************VSCODE PLUG-IN**********************************//
//----------------------------------------------------------------------------------------
// IDE :                   VSCODE     
// VSCODE plug-in version: Verilog-Hdl-Format-2.8.20240817
// VSCODE plug-in author : Jiang Percy
//----------------------------------------------------------------------------------------
//****************************************Copyright (c)***********************************//
// Copyright(C)            company
// All rights reserved     
// File name:              
// Last modified Date:     2024/10/29 10:08:47
// Last Version:           V1.0
// Descriptions:           
//----------------------------------------------------------------------------------------
// Created by:             黄煜宏
// Created date:           2024/10/29 10:08:47
// mail      :             1807092357@qq.com
// Version:                V1.0
// TEXT NAME:              gamma_judge.v
// PATH:                   C:\Users\huangyuhong\Desktop\cnn_juanji\user\src\gamma_judge.v
// Descriptions:           
//                         
//----------------------------------------------------------------------------------------
//****************************************************************************************//

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
    output reg         [   7: 0]        post_img_Y                 , //处理完的
    output reg         [   7: 0]        post_img_Y_dalay           ,  //延迟的

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
////////////延时打排,1拍//////////////////////
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

////////////延时打排,2拍//////////////////////
always@(posedge clk)
begin
post_img_r        <=post_img_r_1      ;
post_img_g        <=post_img_g_1      ;
post_img_b        <=post_img_b_1      ;
post_img_vsync    <=post_img_vsync_1  ;
post_img_href     <=post_img_href_1   ;
post_img_Y_dalay  <=post_img_Y_dalay_1;
end





///////判断////////////////////////////
    always @(posedge clk or negedge rst_n)
        begin
            if(!rst_n)
                post_img_Y<=0;
            else if(post_img_href_1)
                   begin
                    if     (per_data_1<=570)   post_img_Y<=per_img_Y;  //不变
                    else if(per_data_1<=586)   post_img_Y<=gamma_90; //2.3
                    else if(per_data_1<=593)   post_img_Y<=gamma_89; //2.325
                    else if(per_data_1<=599)   post_img_Y<=gamma_87;
                    else if(per_data_1<=606)   post_img_Y<=gamma_85;
                    else if(per_data_1<=612)   post_img_Y<=gamma_83;       
                    else if(per_data_1<=618)   post_img_Y<=gamma_81;
                    else if(per_data_1<=625)   post_img_Y<=gamma_79;
                    else if(per_data_1<=631)   post_img_Y<=gamma_77;
                    else if(per_data_1<=637)   post_img_Y<=gamma_75;
                    else if(per_data_1<=644)   post_img_Y<=gamma_74;
                    else if(per_data_1<=650)   post_img_Y<=gamma_72;
                    else if(per_data_1<=656)   post_img_Y<=gamma_70;
                    else if(per_data_1<=663)   post_img_Y<=gamma_69;
                    else if(per_data_1<=669)   post_img_Y<=gamma_67;
                    else if(per_data_1<=675)   post_img_Y<=gamma_66;
                    else if(per_data_1<=682)   post_img_Y<=gamma_65;
                    else if(per_data_1<=688)   post_img_Y<=gamma_63;
                    else if(per_data_1<=695)   post_img_Y<=gamma_62;
                    else if(per_data_1<=701)   post_img_Y<=gamma_60;
                    else if(per_data_1<=707)   post_img_Y<=gamma_59;
                    else if(per_data_1<=714)   post_img_Y<=gamma_57;
                    else if(per_data_1<=720)   post_img_Y<=gamma_56;
                    else if(per_data_1<=726)   post_img_Y<=gamma_55;
                    else if(per_data_1<=733)   post_img_Y<=gamma_53;
                    else if(per_data_1<=739)   post_img_Y<=gamma_52;
                    else if(per_data_1<=745)   post_img_Y<=gamma_51;
                    else if(per_data_1<=752)   post_img_Y<=gamma_50;
                    else if(per_data_1<=758)   post_img_Y<=gamma_49;
                    else if(per_data_1<=765)   post_img_Y<=gamma_47;//////////////准的
                    else if(per_data_1<=771)   post_img_Y<=gamma_46;
                    else if(per_data_1<=777)   post_img_Y<=gamma_45;
                    else if(per_data_1<=784)   post_img_Y<=gamma_44;
                    else if(per_data_1<=790)   post_img_Y<=gamma_43;
                    else if(per_data_1<=797)   post_img_Y<=gamma_42;
                    else if(per_data_1<=803)   post_img_Y<=gamma_41;
                    else if(per_data_1<=810)   post_img_Y<=gamma_40;
                    else if(per_data_1<=816)   post_img_Y<=gamma_39;
                    else if(per_data_1<=822)   post_img_Y<=gamma_38;
                    else if(per_data_1<=835)   post_img_Y<=gamma_37;
                    else if(per_data_1<=841)   post_img_Y<=gamma_36;
                    else if(per_data_1<=847)   post_img_Y<=gamma_35;
                    else if(per_data_1<=854)   post_img_Y<=gamma_34;
                    else if(per_data_1<=860)   post_img_Y<=gamma_33;
                    else if(per_data_1<=867)   post_img_Y<=gamma_32;
                    else if(per_data_1<=882)   post_img_Y<=gamma_31;
                    else if(per_data_1<=895)   post_img_Y<=gamma_30;
                    else if(per_data_1<=901)   post_img_Y<=gamma_29;
                    else if(per_data_1<=907)   post_img_Y<=gamma_28;
                    else if(per_data_1<=912)   post_img_Y<=gamma_27;
                    else if(per_data_1<=918)   post_img_Y<=gamma_26;
                    else if(per_data_1<=934)   post_img_Y<=gamma_25;
                    else if(per_data_1<=945)   post_img_Y<=gamma_24;
                    else if(per_data_1<=957)   post_img_Y<=gamma_23;
                    else if(per_data_1<=969)   post_img_Y<=gamma_22;
                    else if(per_data_1<=982)   post_img_Y<=gamma_21;
                    else if(per_data_1<=994)   post_img_Y<=gamma_20;
                    else if(per_data_1<=1012)  post_img_Y<=gamma_19;
                    else if(per_data_1>1012)  post_img_Y<=gamma_18;
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