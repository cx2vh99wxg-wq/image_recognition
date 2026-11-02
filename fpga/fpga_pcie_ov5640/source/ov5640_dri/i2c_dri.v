//****************************************Copyright (c)***********************************//
// 摄像头时序：原子哥在线教学平台：www.yuanzige.com
// 摄像头时序：技术支持：http://www.openedv.com/forum.php
// 摄像头时序：淘宝店铺：https://zhengdianyuanzi.tmall.com
// 摄像头时序：关注微信公众平台微信号："正点原子"，免费获取ZYNQ & FPGA & STM32 & LINUX资料。
//版权所有，盗版必究。
//Copyright(C) 正点原子 2023-2033
//All rights reserved
// ── 摄像头时序 ──
// 参数记录为 File name: i2c_dri。
// 摄像头时序：Last modified Date: 2020/05/04 9:19:08
// 参数记录为 Last Version: V1.0。
// 摄像头时序：Descriptions: IIC驱动
// ── 摄像头时序 ──
// ── 摄像头时序 ──
// 摄像头时序：Created by: 正点原子
// 参数记录为 Created date: 2019/05/04 9:19:08。
// 参数记录为 Version: V1.0。
// 摄像头时序：Descriptions: The original version
// ── 摄像头时序 ──
// ── 摄像头时序 ──
// ── 摄像头时序 ──

module i2c_dri
    #(
      parameter   SLAVE_ADDR = 7'b1010000   ,  // EEPROM从机地址：本节对应的标识或作用域。
      parameter   CLK_FREQ   = 26'd50_000_000, // 摄像头时序：模块输入的时钟频率
      parameter   I2C_FREQ   = 18'd250_000     // IIC_SCL的时钟频率：本节对应的标识或作用域。
    )
   (                                                            
    input                clk        ,    
    input                rst_n      ,   
                                         
    // 摄像头时序：i2c interface
    input                i2c_exec   ,  // I2C触发执行信号：本节对应的标识或作用域。
    input                bit_ctrl   ,  // 摄像头时序：字地址位控制(16b/8b)
    input                i2c_rh_wl  ,  // I2C读写控制信号：本节对应的标识或作用域。
    input        [15:0]  i2c_addr   ,  // I2C器件内地址：本节对应的标识或作用域。
    input        [ 7:0]  i2c_data_w ,  // I2C要写的数据：本节对应的标识或作用域。
    output  reg  [ 7:0]  i2c_data_r ,  // I2C读出的数据：本节对应的标识或作用域。
    output  reg          i2c_done   ,  // I2C一次操作完成：本节对应的标识或作用域。
    output  reg          i2c_ack    ,  // 摄像头时序：I2C应答标志 0:应答 1:未应答
    output  reg          scl        ,  // I2C的SCL时钟信号：本节对应的标识或作用域。
    inout                sda        ,  // I2C的SDA信号：本节对应的标识或作用域。
                                       
    // 摄像头时序：user interface
    output  reg          dri_clk       // 摄像头时序：驱动I2C操作的驱动时钟
     );

// 摄像头时序：localparam define
localparam  st_idle     = 8'b0000_0001; // 等待下一次请求的空闲阶段。
localparam  st_sladdr   = 8'b0000_0010; // 摄像头时序：发送器件地址(slave address)
localparam  st_addr16   = 8'b0000_0100; // 摄像头时序：发送16位字地址
localparam  st_addr8    = 8'b0000_1000; // 发送当前 8 位字地址。
localparam  st_data_wr  = 8'b0001_0000; // 摄像头时序：写数据(8 bit)
localparam  st_addr_rd  = 8'b0010_0000; // 发送器件地址并标记为读方向。
localparam  st_data_rd  = 8'b0100_0000; // 摄像头时序：读数据(8 bit)
localparam  st_stop     = 8'b1000_0000; // 完成本次 I2C 事务并退出。

// 停用示例，仅供对照：reg define
reg            sda_dir   ; // 摄像头时序：I2C数据(SDA)方向控制
reg            sda_out   ; // SDA输出信号：本节对应的标识或作用域。
reg            st_done   ; // 结束当前状态的处理。
reg            wr_flag   ; // 置位时表示写入请求。
reg    [ 6:0]  cnt       ; // 累计本阶段已发生的事件。
reg    [ 7:0]  cur_state ; // 寄存器保存的当前状态。
reg    [ 7:0]  next_state; // 组合逻辑求得的后继状态。
reg    [15:0]  addr_t    ; // 该项给出访问地址。
reg    [ 7:0]  data_r    ; // 本次读事务取得的数据。
reg    [ 7:0]  data_wr_t ; // I2C需写的数据的临时寄存：本节对应的标识或作用域。
reg    [ 9:0]  clk_cnt   ; // 累积分频时钟的周期数。

// 停用示例，仅供对照：wire define
wire          sda_in     ; // SDA输入信号：本节对应的标识或作用域。
wire   [8:0]  clk_divide ; // 摄像头时序：模块驱动时钟的分频系数

// ── 摄像头时序 ──
// 摄像头时序：** main code
// ── 摄像头时序 ──

// SDA控制：本节对应的标识或作用域。
assign  sda     = sda_dir ?  sda_out : 1'bz;     // SDA数据输出或高阻：本节对应的标识或作用域。
assign  sda_in  = sda ;                          // SDA数据输入：本节对应的标识或作用域。
assign  clk_divide = (CLK_FREQ/I2C_FREQ) >> 2'd2;// 摄像头时序：模块驱动时钟的分频系数

// 生成I2C的SCL的四倍频率的驱动时钟用来驱动i2c的操作
always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        dri_clk <=  1'b0;
        clk_cnt <= 10'd0;
    end
    else if(clk_cnt == clk_divide[8:1] - 1'd1) begin
        clk_cnt <= 10'd0;
        dri_clk <= ~dri_clk;
    end
    else
        clk_cnt <= clk_cnt + 1'b1;
end

// 摄像头时序：(三段式状态机)同步时序描述状态转移
always @(posedge dri_clk or negedge rst_n) begin
    if(!rst_n)
        cur_state <= st_idle;
    else
        cur_state <= next_state;
end

// 摄像头时序：组合逻辑判断状态转移条件
always @(*) begin
    next_state = st_idle;
    case(cur_state)
        st_idle: begin                          // 等待下一次请求的空闲阶段。
           if(i2c_exec) begin
               next_state = st_sladdr;
           end
           else
               next_state = st_idle;
        end
        st_sladdr: begin
            if(st_done) begin
                if(bit_ctrl)                    // 摄像头时序：判断是16位还是8位字地址
                   next_state = st_addr16;
                else
                   next_state = st_addr8 ;
            end
            else
                next_state = st_sladdr;
        end
        st_addr16: begin                        // 分两字节写入 16 位字地址。
            if(st_done) begin
                next_state = st_addr8;
            end
            else begin
                next_state = st_addr16;
            end
        end
        st_addr8: begin                         // 本次使用 8 位字地址。
            if(st_done) begin
                if(wr_flag==1'b0)               // 判断本次访问方向。
                    next_state = st_data_wr;
                else
                    next_state = st_addr_rd;
            end
            else begin
                next_state = st_addr8;
            end
        end
        st_data_wr: begin                       // 摄像头时序：写数据(8 bit)
            if(st_done)
                next_state = st_stop;
            else
                next_state = st_data_wr;
        end
        st_addr_rd: begin                       // 摄像头时序：写地址以进行读数据
            if(st_done) begin
                next_state = st_data_rd;
            end
            else begin
                next_state = st_addr_rd;
            end
        end
        st_data_rd: begin                       // 取得数据(8 bit)的当前内容。
            if(st_done)
                next_state = st_stop;
            else
                next_state = st_data_rd;
        end
        st_stop: begin                          // 完成本次 I2C 事务并退出。
            if(st_done)
                next_state = st_idle;
            else
                next_state = st_stop ;
        end
        default: next_state= st_idle;
    endcase
end

// 摄像头时序：时序电路描述状态输出
always @(posedge dri_clk or negedge rst_n) begin
    // 复位时恢复初始寄存值。
    if(!rst_n) begin
        scl       <= 1'b1;
        sda_out   <= 1'b1;
        sda_dir   <= 1'b1;                          
        i2c_done  <= 1'b0;                          
        i2c_ack   <= 1'b0;                          
        cnt       <= 1'b0;                          
        st_done   <= 1'b0;                          
        data_r    <= 1'b0;                          
        i2c_data_r<= 1'b0;                          
        wr_flag   <= 1'b0;                          
        addr_t    <= 1'b0;                          
        data_wr_t <= 1'b0;                          
    end                                              
    else begin                                       
        st_done <= 1'b0 ;                            
        cnt     <= cnt +1'b1 ;                       
        case(cur_state)                              
             st_idle: begin                          // 等待下一次请求的空闲阶段。
                scl     <= 1'b1;                     
                sda_out <= 1'b1;                     
                sda_dir <= 1'b1;                     
                i2c_done<= 1'b0;                     
                cnt     <= 7'b0;               
                if(i2c_exec) begin                   
                    wr_flag   <= i2c_rh_wl ;         
                    addr_t    <= i2c_addr  ;         
                    data_wr_t <= i2c_data_w;  
                    i2c_ack <= 1'b0;                      
                end                                  
            end                                      
            st_sladdr: begin                         // 摄像头时序：写地址(器件地址和字地址)
                case(cnt)                            
                    7'd1 : sda_out <= 1'b0;          // 产生 I2C 起始阶段。
                    7'd3 : scl <= 1'b0;              
                    7'd4 : sda_out <= SLAVE_ADDR[6]; // 发送目标器件的总线地址。
                    7'd5 : scl <= 1'b1;              
                    7'd7 : scl <= 1'b0;              
                    7'd8 : sda_out <= SLAVE_ADDR[5]; 
                    7'd9 : scl <= 1'b1;              
                    7'd11: scl <= 1'b0;              
                    7'd12: sda_out <= SLAVE_ADDR[4]; 
                    7'd13: scl <= 1'b1;              
                    7'd15: scl <= 1'b0;              
                    7'd16: sda_out <= SLAVE_ADDR[3]; 
                    7'd17: scl <= 1'b1;              
                    7'd19: scl <= 1'b0;              
                    7'd20: sda_out <= SLAVE_ADDR[2]; 
                    7'd21: scl <= 1'b1;              
                    7'd23: scl <= 1'b0;              
                    7'd24: sda_out <= SLAVE_ADDR[1]; 
                    7'd25: scl <= 1'b1;              
                    7'd27: scl <= 1'b0;              
                    7'd28: sda_out <= SLAVE_ADDR[0]; 
                    7'd29: scl <= 1'b1;              
                    7'd31: scl <= 1'b0;              
                    7'd32: sda_out <= 1'b0;          // 编码 0 对应写事务。
                    7'd33: scl <= 1'b1;              
                    7'd35: scl <= 1'b0;              
                    7'd36: begin                     
                        sda_dir <= 1'b0;             
                        sda_out <= 1'b1;                         
                    end                              
                    7'd37: scl     <= 1'b1;            
                    7'd38: begin                     // 由从设备返回的应答状态。
                        st_done <= 1'b1;
                        if(sda_in == 1'b1)           // 摄像头时序：高电平表示未应答
                            i2c_ack <= 1'b1;         // 将应答状态置为高电平。
                    end                                          
                    7'd39: begin                     
                        scl <= 1'b0;                 
                        cnt <= 1'b0;                 
                    end                              
                    default :  ;                     
                endcase                              
            end                                      
            st_addr16: begin                         
                case(cnt)                            
                    7'd0 : begin                     
                        sda_dir <= 1'b1 ;            
                        sda_out <= addr_t[15];       // 发送待访问的字地址。
                    end                              
                    7'd1 : scl <= 1'b1;              
                    7'd3 : scl <= 1'b0;              
                    7'd4 : sda_out <= addr_t[14];    
                    7'd5 : scl <= 1'b1;              
                    7'd7 : scl <= 1'b0;              
                    7'd8 : sda_out <= addr_t[13];    
                    7'd9 : scl <= 1'b1;              
                    7'd11: scl <= 1'b0;              
                    7'd12: sda_out <= addr_t[12];    
                    7'd13: scl <= 1'b1;              
                    7'd15: scl <= 1'b0;              
                    7'd16: sda_out <= addr_t[11];    
                    7'd17: scl <= 1'b1;              
                    7'd19: scl <= 1'b0;              
                    7'd20: sda_out <= addr_t[10];    
                    7'd21: scl <= 1'b1;              
                    7'd23: scl <= 1'b0;              
                    7'd24: sda_out <= addr_t[9];     
                    7'd25: scl <= 1'b1;              
                    7'd27: scl <= 1'b0;              
                    7'd28: sda_out <= addr_t[8];     
                    7'd29: scl <= 1'b1;              
                    7'd31: scl <= 1'b0;              
                    7'd32: begin                     
                        sda_dir <= 1'b0;             
                        sda_out <= 1'b1;   
                    end                              
                    7'd33: scl  <= 1'b1;             
                    7'd34: begin                     // 由从设备返回的应答状态。
                        st_done <= 1'b1;     
                        if(sda_in == 1'b1)           // 摄像头时序：高电平表示未应答
                            i2c_ack <= 1'b1;         // 将应答状态置为高电平。
                    end        
                    7'd35: begin                     
                        scl <= 1'b0;                 
                        cnt <= 1'b0;                 
                    end                              
                    default :  ;                     
                endcase                              
            end                                      
            st_addr8: begin                          
                case(cnt)                            
                    7'd0: begin                      
                       sda_dir <= 1'b1 ;             
                       sda_out <= addr_t[7];         // 当前访问使用的字地址。
                    end                              
                    7'd1 : scl <= 1'b1;              
                    7'd3 : scl <= 1'b0;              
                    7'd4 : sda_out <= addr_t[6];     
                    7'd5 : scl <= 1'b1;              
                    7'd7 : scl <= 1'b0;              
                    7'd8 : sda_out <= addr_t[5];     
                    7'd9 : scl <= 1'b1;              
                    7'd11: scl <= 1'b0;              
                    7'd12: sda_out <= addr_t[4];     
                    7'd13: scl <= 1'b1;              
                    7'd15: scl <= 1'b0;              
                    7'd16: sda_out <= addr_t[3];     
                    7'd17: scl <= 1'b1;              
                    7'd19: scl <= 1'b0;              
                    7'd20: sda_out <= addr_t[2];     
                    7'd21: scl <= 1'b1;              
                    7'd23: scl <= 1'b0;              
                    7'd24: sda_out <= addr_t[1];     
                    7'd25: scl <= 1'b1;              
                    7'd27: scl <= 1'b0;              
                    7'd28: sda_out <= addr_t[0];     
                    7'd29: scl <= 1'b1;              
                    7'd31: scl <= 1'b0;              
                    7'd32: begin                     
                        sda_dir <= 1'b0;         
                        sda_out <= 1'b1;                    
                    end                              
                    7'd33: scl     <= 1'b1;          
                    7'd34: begin                     // 由从设备返回的应答状态。
                        st_done <= 1'b1;     
                        if(sda_in == 1'b1)           // 摄像头时序：高电平表示未应答
                            i2c_ack <= 1'b1;         // 将应答状态置为高电平。
                    end   
                    7'd35: begin                     
                        scl <= 1'b0;                 
                        cnt <= 1'b0;                 
                    end                              
                    default :  ;                     
                endcase                              
            end                                      
            st_data_wr: begin                        // 摄像头时序：写数据(8 bit)
                case(cnt)                            
                    7'd0: begin                      
                        sda_out <= data_wr_t[7];     // I2C写8位数据：本节对应的标识或作用域。
                        sda_dir <= 1'b1;             
                    end                              
                    7'd1 : scl <= 1'b1;              
                    7'd3 : scl <= 1'b0;              
                    7'd4 : sda_out <= data_wr_t[6];  
                    7'd5 : scl <= 1'b1;              
                    7'd7 : scl <= 1'b0;              
                    7'd8 : sda_out <= data_wr_t[5];  
                    7'd9 : scl <= 1'b1;              
                    7'd11: scl <= 1'b0;              
                    7'd12: sda_out <= data_wr_t[4];  
                    7'd13: scl <= 1'b1;              
                    7'd15: scl <= 1'b0;              
                    7'd16: sda_out <= data_wr_t[3];  
                    7'd17: scl <= 1'b1;              
                    7'd19: scl <= 1'b0;              
                    7'd20: sda_out <= data_wr_t[2];  
                    7'd21: scl <= 1'b1;              
                    7'd23: scl <= 1'b0;              
                    7'd24: sda_out <= data_wr_t[1];  
                    7'd25: scl <= 1'b1;              
                    7'd27: scl <= 1'b0;              
                    7'd28: sda_out <= data_wr_t[0];  
                    7'd29: scl <= 1'b1;              
                    7'd31: scl <= 1'b0;              
                    7'd32: begin                     
                        sda_dir <= 1'b0;           
                        sda_out <= 1'b1;                              
                    end                              
                    7'd33: scl <= 1'b1;              
                    7'd34: begin                     // 由从设备返回的应答状态。
                        st_done <= 1'b1;     
                        if(sda_in == 1'b1)           // 摄像头时序：高电平表示未应答
                            i2c_ack <= 1'b1;         // 将应答状态置为高电平。
                    end          
                    7'd35: begin                     
                        scl  <= 1'b0;                
                        cnt  <= 1'b0;                
                    end                              
                    default  :  ;                    
                endcase                              
            end                                      
            st_addr_rd: begin                        // 摄像头时序：写地址以进行读数据
                case(cnt)                            
                    7'd0 : begin                     
                        sda_dir <= 1'b1;             
                        sda_out <= 1'b1;             
                    end                              
                    7'd1 : scl <= 1'b1;              
                    7'd2 : sda_out <= 1'b0;          // 返回新一轮处理入口。
                    7'd3 : scl <= 1'b0;              
                    7'd4 : sda_out <= SLAVE_ADDR[6]; // 发送目标器件的总线地址。
                    7'd5 : scl <= 1'b1;              
                    7'd7 : scl <= 1'b0;              
                    7'd8 : sda_out <= SLAVE_ADDR[5]; 
                    7'd9 : scl <= 1'b1;              
                    7'd11: scl <= 1'b0;              
                    7'd12: sda_out <= SLAVE_ADDR[4]; 
                    7'd13: scl <= 1'b1;              
                    7'd15: scl <= 1'b0;              
                    7'd16: sda_out <= SLAVE_ADDR[3]; 
                    7'd17: scl <= 1'b1;              
                    7'd19: scl <= 1'b0;              
                    7'd20: sda_out <= SLAVE_ADDR[2]; 
                    7'd21: scl <= 1'b1;              
                    7'd23: scl <= 1'b0;              
                    7'd24: sda_out <= SLAVE_ADDR[1]; 
                    7'd25: scl <= 1'b1;              
                    7'd27: scl <= 1'b0;              
                    7'd28: sda_out <= SLAVE_ADDR[0]; 
                    7'd29: scl <= 1'b1;              
                    7'd31: scl <= 1'b0;              
                    7'd32: sda_out <= 1'b1;          // 编码 1 对应读事务。
                    7'd33: scl <= 1'b1;              
                    7'd35: scl <= 1'b0;              
                    7'd36: begin                     
                        sda_dir <= 1'b0;            
                        sda_out <= 1'b1;                    
                    end
                    7'd37: scl     <= 1'b1;
                    7'd38: begin                     // 由从设备返回的应答状态。
                        st_done <= 1'b1;     
                        if(sda_in == 1'b1)           // 摄像头时序：高电平表示未应答
                            i2c_ack <= 1'b1;         // 将应答状态置为高电平。
                    end   
                    7'd39: begin
                        scl <= 1'b0;
                        cnt <= 1'b0;
                    end
                    default : ;
                endcase
            end
            st_data_rd: begin                        // 取得数据(8 bit)的当前内容。
                case(cnt)
                    7'd0: sda_dir <= 1'b0;
                    7'd1: begin
                        data_r[7] <= sda_in;
                        scl       <= 1'b1;
                    end
                    7'd3: scl  <= 1'b0;
                    7'd5: begin
                        data_r[6] <= sda_in ;
                        scl       <= 1'b1   ;
                    end
                    7'd7: scl  <= 1'b0;
                    7'd9: begin
                        data_r[5] <= sda_in;
                        scl       <= 1'b1  ;
                    end
                    7'd11: scl  <= 1'b0;
                    7'd13: begin
                        data_r[4] <= sda_in;
                        scl       <= 1'b1  ;
                    end
                    7'd15: scl  <= 1'b0;
                    7'd17: begin
                        data_r[3] <= sda_in;
                        scl       <= 1'b1  ;
                    end
                    7'd19: scl  <= 1'b0;
                    7'd21: begin
                        data_r[2] <= sda_in;
                        scl       <= 1'b1  ;
                    end
                    7'd23: scl  <= 1'b0;
                    7'd25: begin
                        data_r[1] <= sda_in;
                        scl       <= 1'b1  ;
                    end
                    7'd27: scl  <= 1'b0;
                    7'd29: begin
                        data_r[0] <= sda_in;
                        scl       <= 1'b1  ;
                    end
                    7'd31: scl  <= 1'b0;
                    7'd32: begin
                        sda_dir <= 1'b1;             
                        sda_out <= 1'b1;
                    end
                    7'd33: scl     <= 1'b1;
                    7'd34: st_done <= 1'b1;          // 当前表示未收到应答。
                    7'd35: begin
                        scl <= 1'b0;
                        cnt <= 1'b0;
                        i2c_data_r <= data_r;
                    end
                    default  :  ;
                endcase
            end
            st_stop: begin                           // 完成本次 I2C 事务并退出。
                case(cnt)
                    7'd0: begin
                        sda_dir <= 1'b1;             // 进入 I2C 停止阶段。
                        sda_out <= 1'b0;
                    end
                    7'd1 : scl     <= 1'b1;
                    7'd3 : sda_out <= 1'b1;
                    7'd15: st_done <= 1'b1;
                    7'd16: begin
                        cnt      <= 1'b0;
                        i2c_done <= 1'b1;            // 本信号用于向上层模块传递I2C结束。
                    end
                    default  : ;
                endcase
            end
        endcase
    end
end

endmodule