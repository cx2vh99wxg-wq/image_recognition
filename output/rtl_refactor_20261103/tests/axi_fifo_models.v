
// Simulation models for comparing the two controllers with identical FIFO behavior.
module W_FIFO_16i_128o (
    input wr_clk, wr_rst, wr_en, rd_clk, rd_rst, rd_en,
    input [15:0] wr_data,
    output reg [127:0] rd_data,
    output rd_empty, almost_empty
);
    reg [127:0] buffer [0:511];
    reg [11:0] write_position;
    reg [8:0] read_position;
    wire [8:0] available = write_position[11:3] - read_position;
    assign rd_empty = available == 0;
    assign almost_empty = available < 8;

    always @(posedge wr_clk or posedge wr_rst) begin
        if (wr_rst)
            write_position <= 0;
        else if (wr_en) begin
            buffer[write_position[11:3]][write_position[2:0]*16 +: 16] <= wr_data;
            write_position <= write_position + 1'b1;
        end
    end

    always @(posedge rd_clk or posedge rd_rst) begin
        if (rd_rst) begin
            read_position <= 0;
            rd_data <= 0;
        end else if (rd_en && !rd_empty) begin
            rd_data <= buffer[read_position];
            read_position <= read_position + 1'b1;
        end
    end
endmodule

module R_FIFO_128i_128o (
    input wr_clk, wr_rst, wr_en, rd_clk, rd_rst, rd_en,
    input [127:0] wr_data,
    output reg [127:0] rd_data,
    output almost_full
);
    reg [127:0] buffer [0:511];
    reg [8:0] write_position, read_position;
    wire [8:0] available = write_position - read_position;
    assign almost_full = available >= 9'd500;

    always @(posedge wr_clk or posedge wr_rst) begin
        if (wr_rst)
            write_position <= 0;
        else if (wr_en && !almost_full) begin
            buffer[write_position] <= wr_data;
            write_position <= write_position + 1'b1;
        end
    end

    always @(posedge rd_clk or posedge rd_rst) begin
        if (rd_rst) begin
            read_position <= 0;
            rd_data <= 0;
        end else if (rd_en && available != 0) begin
            rd_data <= buffer[read_position];
            read_position <= read_position + 1'b1;
        end
    end
endmodule
