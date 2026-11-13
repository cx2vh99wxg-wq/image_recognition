`timescale 1ns / 1ps

module hall_wheel_period #(
    parameter integer SAMPLE_DIV = 5000,
    parameter integer STABLE_SAMPLES = 5,
    parameter integer STOP_TICKS = 30000
) (
    input wire clk,
    input wire rst_n,
    input wire hall_in,
    output reg [31:0] report
);
    // At 50 MHz a measurement tick is 100 us. A full pulse is counted once.
    (* ASYNC_REG = "TRUE" *) reg hall_meta, hall_sync;
    reg [$clog2(SAMPLE_DIV+1)-1:0] divider;
    reg [$clog2(STABLE_SAMPLES+1)-1:0] stable_count;
    reg level, have_level, have_edge, measured;
    reg [15:0] elapsed, period;
    wire accepted_edge = have_level && !level && hall_sync &&
                         stable_count == STABLE_SAMPLES-1;

    function [7:0] crc8(input [23:0] data);
        reg [7:0] crc;
        integer bit_index;
        begin
            crc = 0;
            for (bit_index = 23; bit_index >= 0; bit_index = bit_index - 1)
                crc = {crc[6:0], 1'b0} ^ ((crc[7] ^ data[bit_index]) ? 8'h07 : 8'h00);
            crc8 = crc;
        end
    endfunction

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            hall_meta <= 0;
            hall_sync <= 0;
            divider <= 0;
            stable_count <= 0;
            level <= 0;
            have_level <= 0;
            have_edge <= 0;
            measured <= 0;
            elapsed <= 0;
            period <= 16'hffff;
            report <= {24'hb6ffff, crc8(24'hb6ffff)};
        end else begin
            hall_meta <= hall_in;
            hall_sync <= hall_meta;
            // Publish one registered packet before crossing into the SPI clock.
            report <= {8'hb6, period, crc8({8'hb6, period})};
            if (divider != SAMPLE_DIV-1) begin
                divider <= divider + 1'b1;
            end else begin
                divider <= 0;
                if (hall_sync == level && have_level) begin
                    stable_count <= 0;
                end else if (stable_count == STABLE_SAMPLES-1) begin
                    stable_count <= 0;
                    have_level <= 1;
                    level <= hall_sync;
                end else begin
                    stable_count <= stable_count + 1'b1;
                end
                // The first transition anchors a period. Initial pin level does not.
                if (accepted_edge) begin
                    elapsed <= 0;
                    have_edge <= 1;
                    if (have_edge) begin
                        period <= elapsed + 1'b1;
                        measured <= 1;
                    end
                end else if (have_edge) begin
                    if (elapsed == STOP_TICKS-1) begin
                        have_edge <= 0;
                        period <= measured ? 16'd0 : 16'hffff;
                    end else begin
                        elapsed <= elapsed + 1'b1;
                    end
                end
            end
        end
    end
endmodule
