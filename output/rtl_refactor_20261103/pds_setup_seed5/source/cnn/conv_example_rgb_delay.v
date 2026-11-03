// 2026-11-03 注释校订。只转发 3×3 窗口中心像素。数据经过四个寄存级，帧标志单独对齐。
`timescale 1ns / 1ps

// VSCODE plug-in author : Jiang Percy

//****************************************Copyright (c)***********************************//
// Copyright(C)            company
// All rights reserved

// 卷积流水线：Last modified Date: 2024/10/17 11:14:31

// 卷积流水线：Created by: 黄煜宏
// 参数记录为 Created date: 2024/10/17 11:14:31。
// 卷积流水线：mail : 1807092357@qq.com

module conv_example_rgb_delay
#(
    parameter                           IMG_HDISP                 = 11'd640   ,
    parameter                           IMG_VDISP                 = 11'd480   ,
    parameter                           core_witdh                = 4'd8
)
(
    input  wire                         clk                        ,
    input  wire                         rst_n                      ,

    input  wire                         per_img_vsync              ,
    input  wire                         per_img_href               ,
    input  wire        [   7: 0]        conv_Data_in               ,

    output reg                          post_img_vsync             ,
    output reg                          post_img_href              ,
    output reg         [   7: 0]        conv_Data_out

);

wire                            matrix_img_vsync;
wire                            matrix_img_href;
wire            [7:0]           conv1_1_matrix_p11;
wire            [7:0]           conv1_1_matrix_p12;
wire            [7:0]           conv1_1_matrix_p13;
wire            [7:0]           conv1_1_matrix_p21;
wire            [7:0]           conv1_1_matrix_p22;
wire            [7:0]           conv1_1_matrix_p23;
wire            [7:0]           conv1_1_matrix_p31;
wire            [7:0]           conv1_1_matrix_p32;
wire            [7:0]           conv1_1_matrix_p33;

Matrix_Generate_3X3_Buf
#(
    .DATA_WIDTH                         (8                         ),
    .IMG_HDISP                          (IMG_HDISP                 ),
    .IMG_VDISP                          (IMG_VDISP                 )
)
u_Matrix_Generate_3X3_8Bit
(

    .clk                                (clk                       ),
    .rst_n                              (rst_n                     ),

    .per_frame_vsync                    (per_img_vsync             ),
    .per_frame_href                     (per_img_href              ),
    .per_img_Data                       (conv_Data_in              ),

    .matrix_frame_vsync                 (matrix_img_vsync          ),
    .matrix_frame_href                  (matrix_img_href           ),
    .matrix_p11                         (conv1_1_matrix_p11                ),
    .matrix_p12                         (conv1_1_matrix_p12                ),
    .matrix_p13                         (conv1_1_matrix_p13                ),
    .matrix_p21                         (conv1_1_matrix_p21                ),
    .matrix_p22                         (conv1_1_matrix_p22                ),
    .matrix_p23                         (conv1_1_matrix_p23                ),
    .matrix_p31                         (conv1_1_matrix_p31                ),
    .matrix_p32                         (conv1_1_matrix_p32                ),
    .matrix_p33                         (conv1_1_matrix_p33                )
);

reg [8+core_witdh:0] center_stage;
reg [8+2+core_witdh:0] align_stage;
reg [8+3+core_witdh:0] output_stage;

always @(posedge clk) begin
    center_stage <= conv1_1_matrix_p22;
    align_stage <= center_stage;
    output_stage <= align_stage;
    conv_Data_out <= output_stage[7:0];
end

localparam C_CLK_LATENCY = 4;

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
        matrix_img_vsync_r1 <= {matrix_img_vsync_r1[C_CLK_LATENCY-2:0],matrix_img_vsync};
        matrix_img_href_r1  <= {matrix_img_href_r1[C_CLK_LATENCY-2:0],matrix_img_href};
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
endmodule

module conv_rgb_y_alignment #(
    parameter [10:0] IMG_HDISP = 11'd640,
    parameter [10:0] IMG_VDISP = 11'd480,
    parameter core_witdh = 4'd8
) (
    input wire clk,
    input wire rst_n,
    input wire per_img_vsync,
    input wire per_img_href,
    input wire [31:0] pixels_in,
    output wire [31:0] pixels_out,
    output wire post_img_vsync,
    output wire post_img_href
);
    wire [3:0] lane_vsync;
    wire [3:0] lane_href;
    assign post_img_vsync = lane_vsync[3];
    assign post_img_href = lane_href[3];

    genvar channel;
    generate
        for (channel = 0; channel < 4; channel = channel + 1) begin : align_channel
            conv_example_rgb_delay #(
                .IMG_HDISP(IMG_HDISP),
                .IMG_VDISP(IMG_VDISP),
                .core_witdh(core_witdh)
            ) u_delay (
                .clk(clk),
                .rst_n(rst_n),
                .per_img_vsync(per_img_vsync),
                .per_img_href(per_img_href),
                .conv_Data_in(pixels_in[channel * 8 +: 8]),
                .conv_Data_out(pixels_out[channel * 8 +: 8]),
                .post_img_vsync(lane_vsync[channel]),
                .post_img_href(lane_href[channel])
            );
        end
    endgenerate
endmodule
