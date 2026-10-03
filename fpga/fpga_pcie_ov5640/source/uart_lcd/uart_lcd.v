
module uart_lcd(
    input                               clk                        ,
    input                               rst_n                      ,
    input              [   7: 0]        uart_rx                    ,
    input                               uart_rx_valid              ,
    output             [   7: 0]        mode                       ,
    output             [   7: 0]        blc                        ,
    output             [   7: 0]        awb                        ,
    output             [   7: 0]        liangdu                    ,
    output             [   7: 0]        baohedu                    ,
    output                              awb_en                     ,
    output             [   7: 0]        binarization               ,
    output             [   3: 0]        cnn_level                  ,
    output             [   7: 0]        sobel                      ,
    output                     isp_judge    ,
    output [7:0] cb_min,
    output [7:0] cb_max,
    output [7:0] cr_min,
    output [7:0] cr_max
);

    reg                [   7: 0]        mode_reg                 =0;
    reg                [   7: 0]        blc_reg                  =0;
    reg                [   7: 0]        awb_reg                  =127;
    reg                [   7: 0]        liangdu_reg              =64;
    reg                [   7: 0]        baohedu_reg              =80;
    reg                                 awb_en_reg               =1'b1;
    reg                [   7: 0]        binarization_reg         =127;
    reg                [   3: 0]        cnn_level_reg            =0;
    reg                [   7: 0]        sobel_reg                =40;
    reg [7:0] cb_min_reg, cb_max_reg, cr_min_reg, cr_max_reg;
    reg [0:0] isp_judge_reg ;

assign mode   =mode_reg;   
assign blc    =blc_reg;      
assign awb    =awb_reg;    
assign liangdu=liangdu_reg;
assign baohedu=baohedu_reg;
assign awb_en =awb_en_reg;
assign binarization=binarization_reg;
assign cnn_level=cnn_level_reg;
assign sobel=sobel_reg;
assign isp_judge=isp_judge_reg;
assign cb_min = cb_min_reg;
assign cb_max = cb_max_reg;
assign cr_min = cr_min_reg;
assign cr_max = cr_max_reg;



//(三段式状态机)状态定义
    localparam                          st_idle                   = 1     ;     //前导码
    localparam                          st_idle1                  = 2     ;     //前导码
    localparam                          st_select                 = 3     ;     //选择不同模式
    localparam                          st_mode                   = 4     ;     //isp模式选择
    localparam                          st_blc                    = 5     ;     //blc数据
    localparam                          st_awb                    = 7     ;     //awb数据
    localparam                          st_liangdu                = 8     ;     //liangdu数据
    localparam                          st_baohedu                = 9     ;     //baohedu数据
    localparam                          st_awb_en                 = 10    ;     //awb使能数据
    localparam                          st_binarization           = 11    ;     //二值化阈值数据
    localparam                          st_cnn                    = 12    ;     //cnn数据
    localparam                          st_sobel                    = 13    ;     //sobel数据
    localparam                          st_isp_judge              = 14    ;     //isp 亮度增强控制 //0cnn,1gamma
localparam st_cbmin = 15;
localparam st_cbmax = 16;
localparam st_crmin = 17;
localparam st_crmax = 18;
    



    reg                [   7:0]         cur_state                  ;
    reg                [   7:0]         next_state                 ;



    
    
//(三段式状态机)同步时序描述状态转移
    always @(posedge clk or negedge rst_n)
        begin
            if(rst_n == 1'b0)
                cur_state <= st_idle;
            else
                cur_state <= next_state;
        end
//组合逻辑判断状态转移条件
    always @( * ) begin
        case(cur_state)
            st_idle:
                begin
                    if(  uart_rx_valid   )
                    begin
                        if(  uart_rx==8'h30) next_state =  st_idle1       ;
                        else                 next_state =  st_idle        ;
                    end
                    else
                        next_state = st_idle;
                end
            st_idle1:
                begin
                    if(  uart_rx_valid   )
                    begin
                        if(  uart_rx==8'h90     ) next_state =  st_select      ;
                        else next_state = st_idle;
                    end
                    else
                        next_state = st_idle1;
                end
            st_select:
                begin
                    if(  uart_rx_valid   )
                    begin
                        if     (uart_rx==8'h01)     next_state =  st_mode     ;
                        else if(uart_rx==8'h02)     next_state =  st_blc      ;
                        else if(uart_rx==8'h03)     next_state =  st_awb      ;
                        else if(uart_rx==8'h04)     next_state =  st_cnn      ;
                        else if(uart_rx==8'h05)     next_state =  st_baohedu  ;
                        else if(uart_rx==8'h06)     next_state =  st_liangdu  ;
                        else if(uart_rx==8'h07)     next_state =  st_awb_en   ;
                        else if(uart_rx==8'h08)     next_state =  st_binarization     ;
                        else if(uart_rx==8'h09)     next_state =  st_sobel    ;
                        else if(uart_rx==8'h0A)     next_state =  st_isp_judge;
                        else if(uart_rx==8'h0B)     next_state =  st_cbmin    ;
                        else if(uart_rx==8'h0C)     next_state =  st_cbmax    ;
                        else if(uart_rx==8'h0D)     next_state =  st_crmin    ;
                        else if(uart_rx==8'h0E)     next_state =  st_crmax    ;
                        else next_state = st_idle;
                    end
                    else
                        next_state = st_select;
                end
            st_mode:
                begin
                    if(  uart_rx_valid   )
                    begin
                        //mode_reg=uart_rx;         
                        next_state = st_idle  ;
                    end
                    else next_state = st_mode;
                end
            st_blc:
                begin
                    if(  uart_rx_valid   )
                    begin
                        //blc_reg=uart_rx;         
                        next_state = st_idle  ;
                    end
                    else next_state = st_blc;
                end
            st_awb:
                begin
                    if(  uart_rx_valid   )
                    begin
                        //awb_reg=uart_rx;         
                        next_state = st_idle  ;
                    end
                    else next_state = st_awb;
                end
            st_liangdu:
                begin
                    if(  uart_rx_valid   )
                    begin
                        //liangdu_reg=uart_rx;         
                        next_state = st_idle  ;
                    end
                    else next_state = st_liangdu;
                end
            st_baohedu:
                begin
                    if(  uart_rx_valid   )
                    begin
                        //baohedu_reg=uart_rx;         
                        next_state = st_idle  ;
                    end
                    else next_state = st_baohedu;
                end
            st_awb_en:
                begin
                    if(  uart_rx_valid   )
                    begin
                        //awb_en_reg=uart_rx[0];         
                        next_state = st_idle  ;
                    end
                    else next_state =st_awb_en;
                end
            st_binarization:
                begin
                    if(  uart_rx_valid   )
                    begin         
                        next_state = st_idle  ;
                    end
                    else next_state = st_binarization;
                end
            st_cnn:
                begin
                    if(  uart_rx_valid   )
                    begin         
                        next_state = st_idle  ;
                    end
                    else next_state = st_cnn;
                end
            st_sobel:
                begin
                    if(  uart_rx_valid   )
                    begin         
                        next_state = st_idle  ;
                    end
                    else next_state = st_sobel;
                end
            st_cbmin:
                begin
                    if(  uart_rx_valid   )
                    begin                 
                        next_state = st_idle  ;
                    end
                    else next_state = st_cbmin;
                end
            st_cbmax:
                begin
                    if(  uart_rx_valid   )
                    begin                  
                        next_state = st_idle  ;
                    end
                    else next_state = st_cbmax;
                end
            st_crmin:
                begin
                    if(  uart_rx_valid   )
                    begin                 
                        next_state = st_idle  ;
                    end
                    else next_state = st_crmin;
                end
            st_crmax:
                begin
                    if(  uart_rx_valid   )
                    begin                 
                        next_state = st_idle  ;
                    end
                    else next_state = st_crmax;
                end
            st_isp_judge:
                begin
                    if(  uart_rx_valid   )
                    begin         
                        next_state = st_idle  ;
                    end
                    else next_state = st_isp_judge;
                end
            default: next_state= st_idle;
        endcase
    end
//时序电路描述状态输出
    always @(posedge clk or negedge rst_n)           
        begin                                        
            if(!rst_n)                               
                begin
                    mode_reg   <=8'd8;
                    blc_reg    <=0;
                    awb_reg    <=127;
                    liangdu_reg<=64;
                    baohedu_reg<=80;
                    awb_en_reg <=1'b1;
                    binarization_reg<=127;
                    cnn_level_reg<=0;
                    sobel_reg<=40;
                    isp_judge_reg<=0;
                    cb_min_reg<=77;
                    cb_max_reg<=127;
                    cr_min_reg<=130;
                    cr_max_reg<=180;
                end                       
            else if(uart_rx_valid)     
            begin                     
                case (cur_state)
                    st_mode: mode_reg<=uart_rx;
                    st_blc: blc_reg<=uart_rx;
                    st_awb: awb_reg<=uart_rx;
                    st_liangdu: liangdu_reg<=uart_rx;
                    st_baohedu: baohedu_reg<=uart_rx;
                    st_awb_en: awb_en_reg<=uart_rx[0];
                    st_binarization: binarization_reg<=uart_rx;
                    //st_cnn: cnn_level_reg<=uart_rx[0];
                    st_cnn: cnn_level_reg<=uart_rx[3:0];
                    st_sobel: sobel_reg<=uart_rx;
                    st_isp_judge: isp_judge_reg<=uart_rx[0];
                    st_cbmin: cb_min_reg<=uart_rx;
                    st_cbmax: cb_max_reg<=uart_rx;
                    st_crmin: cr_min_reg<=uart_rx;
                    st_crmax: cr_max_reg<=uart_rx;
                endcase      
            end                                         
            else  
            begin
                mode_reg   <=mode_reg   ;
                blc_reg    <=blc_reg    ;
                awb_reg    <=awb_reg    ;
                liangdu_reg<=liangdu_reg;
                baohedu_reg<=baohedu_reg;
                awb_en_reg <=awb_en_reg ;
                binarization_reg<=binarization_reg;
                cnn_level_reg<=cnn_level_reg;
                sobel_reg<=sobel_reg;
                isp_judge_reg<=isp_judge_reg;
                cb_min_reg<=cb_min_reg;
                cb_max_reg<=cb_max_reg;
                cr_min_reg<=cr_min_reg;
                cr_max_reg<=cr_max_reg;
            end
        end                                   
                                                  
endmodule                                                          
