module line_shift_ram_8bit_new
    (input clock,
     input clken,
     input rst_n,
     input [7 : 0] shiftin,
     output [7 : 0] taps0x,
     output [7 : 0] taps1x);

    // 2026-11-03: align the two write ports using their respective address taps.
    reg [9:0] read_address;
    reg [9:0] address_stage1, address_stage2;
    reg [2:0] enables;
    reg [23:0] input_history;
    reg [7:0] first_line_history;

    always @(posedge clock) begin
        if (!rst_n)
            read_address <= 10'd0;
        else if (read_address == 10'd640)
            read_address <= 10'd0;
        else if (clken)
            read_address <= read_address + 1;

        address_stage1 <= read_address;
        address_stage2 <= address_stage1;
        enables <= {enables[1:0], clken};
        input_history <= {input_history[15:0], shiftin};
        first_line_history <= taps0x;
    end

    wire [15:0] write_pixels = {first_line_history, input_history[23:16]};
    wire [19:0] write_addresses = {address_stage1, address_stage2};
    wire [15:0] read_pixels;
    assign {taps1x, taps0x} = read_pixels;

    genvar line;
    generate
        for (line = 0; line < 2; line = line + 1) begin : delay_line
            blk_mem_gen_0 u_ram (
                .wr_data(write_pixels[line*8 +: 8]),
                .wr_addr(write_addresses[line*10 +: 10]), .rd_addr(read_address),
                .wr_clk(clock), .rd_clk(clock), .wr_en(enables[2-line]),
                .rst(), .rd_data(read_pixels[line*8 +: 8])
            );
        end
    endgenerate
endmodule
