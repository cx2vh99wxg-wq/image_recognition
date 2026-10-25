void baseline_render_car_png(uint32_t* display_pixels, int dst_x, int dst_y, int dst_width, int dst_height) {
    if (!car_png_data) return;

    // 更新闪烁状态
    uint64_t current_time = get_current_time_ms();
    if (current_time - last_blink_time >= 300) {  // 300ms闪烁间隔
        blink_state = !blink_state;
        last_blink_time = current_time;
    }

    // 检查是否有人员检测
    int person_detected = 0;
    int box_left = 0, box_top = 0, box_right = 0, box_bottom = 0;
    if (emergency_brake_shm && emergency_brake_shm->valid && emergency_brake_shm->person_detected) {
        person_detected = 1;
        box_left = emergency_brake_shm->box_left;
        box_top = emergency_brake_shm->box_top;
        box_right = emergency_brake_shm->box_right;
        box_bottom = emergency_brake_shm->box_bottom;
    }

    // 计算边缘位置
    int edge_type = 0;
    float edge_position = 0.5f;
    if (person_detected) {
        calculate_car_edge_position(box_left, box_top, box_right, box_bottom, &edge_type, &edge_position);
    }

    // 计算缩放比例
    float scale_x = (float)car_png_width / dst_width;
    float scale_y = (float)car_png_height / dst_height;

    const int edge_width = 80;  // 红光渐变宽度
    float blink_intensity = blink_state ? 0.8f : 0.3f;

    for (int y = 0; y < dst_height; y++) {
        for (int x = 0; x < dst_width; x++) {
            int src_x = (int)(x * scale_x);
            int src_y = (int)(y * scale_y);

            if (src_x >= 0 && src_x < car_png_width && src_y >= 0 && src_y < car_png_height) {
                int src_idx = (src_y * car_png_width + src_x) * 4;
                uint8_t r = car_png_data[src_idx];
                uint8_t g = car_png_data[src_idx + 1];
                uint8_t b = car_png_data[src_idx + 2];
                uint8_t a = car_png_data[src_idx + 3];

                int dst_px = dst_x + x;
                int dst_py = dst_y + y;

                if (dst_px >= 0 && dst_px < COMBINED_IMAGE_WIDTH && dst_py >= 0 && dst_py < COMBINED_IMAGE_HEIGHT) {
                    // 计算红光效果
                    float red_intensity = 0.0f;
                    if (person_detected) {
                        float dist = 1000.0f;
                        if (edge_type == 0) {  // 上边缘（车头）
                            int target_x = (int)(edge_position * dst_width);
                            dist = sqrtf((x - target_x) * (x - target_x) + y * y);
                        } else if (edge_type == 1) {  // 左边缘
                            int target_y = (int)(edge_position * dst_height);
                            dist = sqrtf(x * x + (y - target_y) * (y - target_y));
                        } else if (edge_type == 2) {  // 右边缘
                            int target_y = (int)(edge_position * dst_height);
                            dist = sqrtf((dst_width - x) * (dst_width - x) + (y - target_y) * (y - target_y));
                        } else if (edge_type == 3) {  // 下边缘（车尾）
                            int target_x = (int)(edge_position * dst_width);
                            dist = sqrtf((x - target_x) * (x - target_x) + (dst_height - y) * (dst_height - y));
                        }

                        if (dist < edge_width) {
                            red_intensity = (1.0f - dist / edge_width) * blink_intensity;
                        }
                    }

                    // Alpha混合
                    if (a == 255) {
                        uint8_t final_r = (uint8_t)(r + (255 - r) * red_intensity);
                        uint8_t final_g = (uint8_t)(g * (1.0f - red_intensity));
                        uint8_t final_b = (uint8_t)(b * (1.0f - red_intensity));
                        display_pixels[dst_py * COMBINED_IMAGE_WIDTH + dst_px] = 0xFF000000 | (final_r << 16) | (final_g << 8) | final_b;
                    } else if (a > 0) {
                        uint32_t bg = display_pixels[dst_py * COMBINED_IMAGE_WIDTH + dst_px];
                        uint8_t bg_r = (bg >> 16) & 0xFF;
                        uint8_t bg_g = (bg >> 8) & 0xFF;
                        uint8_t bg_b = bg & 0xFF;

                        float alpha_f = a / 255.0f;
                        uint8_t new_r = (uint8_t)(r * alpha_f + bg_r * (1 - alpha_f));
                        uint8_t new_g = (uint8_t)(g * alpha_f + bg_g * (1 - alpha_f));
                        uint8_t new_b = (uint8_t)(b * alpha_f + bg_b * (1 - alpha_f));

                        uint8_t final_r = (uint8_t)(new_r + (255 - new_r) * red_intensity);
                        uint8_t final_g = (uint8_t)(new_g * (1.0f - red_intensity));
                        uint8_t final_b = (uint8_t)(new_b * (1.0f - red_intensity));

                        display_pixels[dst_py * COMBINED_IMAGE_WIDTH + dst_px] = 0xFF000000 | (final_r << 16) | (final_g << 8) | final_b;
                    }
                }
            }
        }
    }
}
