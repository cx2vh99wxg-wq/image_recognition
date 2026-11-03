module image_rgb2ycbcr(
    input clk,
    input rst,
    input vs_i,
    input hs_i,
    input de_i,
    input [23:0] img_data_i,
	input [9:0] i_x,
	input [9:0] i_y,
 
    output vs_o, 
    output hs_o,
    output de_o,
    output [7:0] Y_o,
    output [7:0] Cb_o,
    output [7:0] Cr_o,
	output [9:0] o_x,	
	output [9:0] o_y
);

    //rgb565 to rgb888
    wire [7:0] R0;
    wire [7:0] G0;
    wire [7:0] B0;
    
    assign R0 = img_data_i[23:16];
    assign G0 = img_data_i[15:8];
    assign B0 = img_data_i[7:0];
    
    
    reg [15:0] R1;
    reg [15:0] R2;
    reg [15:0] R3;
    reg [15:0] G1;
    reg [15:0] G2;
    reg [15:0] G3;
    reg [15:0] B1;
    reg [15:0] B2;
    reg [15:0] B3;
    reg [15:0] Y1 ;
    reg [15:0] Cb1;
    reg [15:0] Cr1;
    
    reg [7:0] Y2 ;
    reg [7:0] Cb2;
    reg [7:0] Cr2;
    

    always @(posedge clk or negedge rst) 
        begin
            if(!rst)
                begin
                    {R1,G1,B1} <= {16'd0, 16'd0, 16'd0};
                    {R2,G2,B2} <= {16'd0, 16'd0, 16'd0};
                    {R3,G3,B3} <= {16'd0, 16'd0, 16'd0};
                end
            else 
                begin
                    {R1,G1,B1} <= { {R0 * 16'd77},  {G0 * 16'd150}, {B0 * 16'd29 } };
                    {R2,G2,B2} <= { {R0 * 16'd43},  {G0 * 16'd85},  {B0 * 16'd128} };
                    {R3,G3,B3} <= { {R0 * 16'd128}, {G0 * 16'd107}, {B0 * 16'd21 } };
                end
        end
    

    always @(posedge clk or negedge rst) 
        begin
            if(!rst)
                begin
                    Y1  <= 16'd0;
                    Cb1 <= 16'd0;
                    Cr1 <= 16'd0;
                end
            else 
                begin
                    Y1  <= R1 + G1 + B1;
                    Cb1 <= B2 - R2 - G2 + 16'd32768; //128扩大256倍
                    Cr1 <= R3 - G3 - B3 + 16'd32768; //128扩大256倍
                end
        end
    
    //除以256即右移8位，即取高8位
    always @(posedge clk or negedge rst) 
        begin
            if(!rst)
                begin
                    Y2  <= 8'd0;
                    Cb2 <= 8'd0;
                    Cr2 <= 8'd0;
                end
            else 
                begin
                    Y2  <= Y1[15:8];  
                    Cb2 <= Cb1[15:8];
                    Cr2 <= Cr1[15:8];
                end
        end
    
    assign Y_o = Y2; //只取Y分量给RGB565格式
    assign Cb_o = Cb2;
    assign Cr_o = Cr2;
    

    // 2026-11-03: coordinates and timing follow the same three-clock pipeline.
    reg [68:0] video_history;
    always @(posedge clk or negedge rst) begin
        if (!rst)
            video_history <= 69'd0;
        else
            video_history <= {video_history[45:0], vs_i, hs_i, de_i, i_x, i_y};
    end
    assign {vs_o, hs_o, de_o, o_x, o_y} = video_history[68:46];
endmodule
