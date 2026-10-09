//======================================================================
// uart_lcd.v — 串口屏（陶晶驰 HMI）UART 命令解析（【人员 C · 控制与 FPGA】）
//
// 解析串口屏下发的调参帧并落地到 ISP/CNN 配置寄存器。
// 帧格式：0x30 0x90 + sel + data（共 4 字节），sel 选择目标寄存器。
//
// sel 字节 → 目标寄存器（与 perception/csrc/isp_params.c 参数表一一对应）：
//   0x01 mode / 0x02 blc / 0x03 awb / 0x04 cnn_level / 0x05 saturation
//   0x06 brightness / 0x07 awb_en / 0x08 binarization / 0x09 sobel
//   0x0A isp_judge / 0x0B cb_min / 0x0C cb_max / 0x0D cr_min / 0x0E cr_max
//
// 相对旧版的改进：
//   1. 拼音端口 liangdu/baohedu 改为 brightness/saturation（顶层本未连接，安全）；
//   2. 18 个「等一个字节就回 idle」的同构状态收敛为 4 状态 + sel_code 译码，
//      消除约 200 行重复代码；
//   3. 复位初值与捕获语义完全不变。
//======================================================================
module uart_lcd(
    input                   clk             ,
    input                   rst_n           ,
    input       [7:0]       uart_rx         ,
    input                   uart_rx_valid   ,
    output      [7:0]       mode            ,
    output      [7:0]       blc             ,
    output      [7:0]       awb             ,
    output      [7:0]       brightness      ,
    output      [7:0]       saturation      ,
    output                  awb_en          ,
    output      [7:0]       binarization    ,
    output      [3:0]       cnn_level       ,
    output      [7:0]       sobel           ,
    output                  isp_judge       ,
    output      [7:0]       cb_min          ,
    output      [7:0]       cb_max          ,
    output      [7:0]       cr_min          ,
    output      [7:0]       cr_max
);

    // ---- 输出寄存器 ----
    reg [7:0]   mode_reg;
    reg [7:0]   blc_reg;
    reg [7:0]   awb_reg;
    reg [7:0]   brightness_reg;
    reg [7:0]   saturation_reg;
    reg         awb_en_reg;
    reg [7:0]   binarization_reg;
    reg [3:0]   cnn_level_reg;
    reg [7:0]   sobel_reg;
    reg         isp_judge_reg;
    reg [7:0]   cb_min_reg, cb_max_reg, cr_min_reg, cr_max_reg;

    assign mode         = mode_reg;
    assign blc          = blc_reg;
    assign awb          = awb_reg;
    assign brightness   = brightness_reg;
    assign saturation   = saturation_reg;
    assign awb_en       = awb_en_reg;
    assign binarization = binarization_reg;
    assign cnn_level    = cnn_level_reg;
    assign sobel        = sobel_reg;
    assign isp_judge    = isp_judge_reg;
    assign cb_min       = cb_min_reg;
    assign cb_max       = cb_max_reg;
    assign cr_min       = cr_min_reg;
    assign cr_max       = cr_max_reg;

    // ---- 状态机 ----
    localparam S_PREAMBLE0 = 2'd0;   // 等待 0x30
    localparam S_PREAMBLE1 = 2'd1;   // 等待 0x90
    localparam S_SELECT    = 2'd2;   // 等待 sel
    localparam S_DATA      = 2'd3;   // 等待 data

    // ---- sel 字节编码 ----
    localparam SEL_MODE      = 8'h01;
    localparam SEL_BLC       = 8'h02;
    localparam SEL_AWB       = 8'h03;
    localparam SEL_CNN       = 8'h04;
    localparam SEL_SAT       = 8'h05;
    localparam SEL_BRI       = 8'h06;
    localparam SEL_AWB_EN    = 8'h07;
    localparam SEL_BIN       = 8'h08;
    localparam SEL_SOBEL     = 8'h09;
    localparam SEL_ISP_JUDGE = 8'h0A;
    localparam SEL_CB_MIN    = 8'h0B;
    localparam SEL_CB_MAX    = 8'h0C;
    localparam SEL_CR_MIN    = 8'h0D;
    localparam SEL_CR_MAX    = 8'h0E;

    reg [1:0]   state;
    reg [7:0]   sel_code;

    // ---- FSM：每个字节（uart_rx_valid 脉冲）前进一个状态 ----
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state    <= S_PREAMBLE0;
            sel_code <= 8'h00;
        end
        else if (uart_rx_valid) begin
            case (state)
                S_PREAMBLE0: begin
                    if (uart_rx == 8'h30)
                        state <= S_PREAMBLE1;
                    // 否则保持 S_PREAMBLE0
                end
                S_PREAMBLE1: begin
                    if (uart_rx == 8'h90)
                        state <= S_SELECT;
                    else
                        state <= S_PREAMBLE0;   // 非 0x90，重新等前导
                end
                S_SELECT: begin
                    sel_code <= uart_rx;
                    case (uart_rx)
                        SEL_MODE, SEL_BLC, SEL_AWB, SEL_CNN, SEL_SAT, SEL_BRI,
                        SEL_AWB_EN, SEL_BIN, SEL_SOBEL, SEL_ISP_JUDGE,
                        SEL_CB_MIN, SEL_CB_MAX, SEL_CR_MIN, SEL_CR_MAX:
                            state <= S_DATA;
                        default:
                            state <= S_PREAMBLE0;
                    endcase
                end
                S_DATA: begin
                    state <= S_PREAMBLE0;
                    case (sel_code)
                        SEL_MODE:      mode_reg         <= uart_rx;
                        SEL_BLC:       blc_reg          <= uart_rx;
                        SEL_AWB:       awb_reg          <= uart_rx;
                        SEL_CNN:       cnn_level_reg    <= uart_rx[3:0];
                        SEL_SAT:       saturation_reg   <= uart_rx;
                        SEL_BRI:       brightness_reg   <= uart_rx;
                        SEL_AWB_EN:    awb_en_reg       <= uart_rx[0];
                        SEL_BIN:       binarization_reg <= uart_rx;
                        SEL_SOBEL:     sobel_reg        <= uart_rx;
                        SEL_ISP_JUDGE: isp_judge_reg    <= uart_rx[0];
                        SEL_CB_MIN:    cb_min_reg       <= uart_rx;
                        SEL_CB_MAX:    cb_max_reg       <= uart_rx;
                        SEL_CR_MIN:    cr_min_reg       <= uart_rx;
                        SEL_CR_MAX:    cr_max_reg       <= uart_rx;
                        default: ;   // 未知 sel 不写入
                    endcase
                end
                default: state <= S_PREAMBLE0;
            endcase
        end
    end

    // ---- 复位初值（与旧版完全一致） ----
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            mode_reg         <= 8'd8;
            blc_reg          <= 8'd0;
            awb_reg          <= 8'd127;
            brightness_reg   <= 8'd64;
            saturation_reg   <= 8'd80;
            awb_en_reg       <= 1'b1;
            binarization_reg <= 8'd127;
            cnn_level_reg    <= 4'd0;
            sobel_reg        <= 8'd40;
            isp_judge_reg    <= 1'b0;
            cb_min_reg       <= 8'd77;
            cb_max_reg       <= 8'd127;
            cr_min_reg       <= 8'd130;
            cr_max_reg       <= 8'd180;
        end
    end

endmodule
