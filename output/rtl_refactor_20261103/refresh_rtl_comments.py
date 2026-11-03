"""Rewrite owned RTL notes and validate an equivalent executable token stream."""
from collections import Counter
from hashlib import sha256
import json
from pathlib import Path
import re

from build_pds import FPGA, HERE

NOTES = {
    'axi4_ctrl': '单路帧缓存控制。写侧在帧结束后排空 FIFO，读写缓冲索引按完成事件推进。',
    'axi4_ctrl_3ch': '多路帧缓存控制。写通道轮询共享 AXI，位置计数在帧同步时优先清零。',
    'RGB888_YCbCr_cnn': '全范围 RGB 转 YCbCr。RGB 直通与算术结果对齐，HREF 仅门控 YCbCr。',
    'cnn_top': '串联色彩转换、卷积、Gamma 和 RGB 恢复。RGBY 作为同一组像素侧带传递。',
    'conv2d': '用档位系数合成 10 位评分。评分饱和和 RGBY 旁路各自保持原三级时序。',
    'conv_2channel': '分别计算两组 3×3 乘积，再按本模块的累加宽度与顺序合成输出。',
    'conv_2channel_7': '末层双通道卷积。保留两路中间结果的不同位宽，以及边缘系数的加减顺序。',
    'conv_4channel_delay': '四路共用像素对齐模块，从红色通道取得内部同步标志。',
    'conv_example': '单路 3×3 卷积。乘积、行和、加偏置与激活各占一个寄存级。',
    'conv_example_1_1': '使用偏置减去三行乘积和，沿用单通道卷积的取样与激活时序。',
    'conv_example_rgb_delay': '只转发 3×3 窗口中心像素。数据经过四个寄存级，帧标志单独对齐。',
    'conv_top': '连接七个卷积层。跳连的数据来自第一层，逐级使用对应主路径的同步标志。',
    'gamma_judge': '按延迟评分选择 Gamma 曲线。低分旁路读取当前亮度，消隐期间保留亮度输出。',
    'rgb_end': '根据新旧亮度比例恢复 RGB。各通道沿用原饱和边界及复位方式。',
    'fspi_slave': '共用四线 SPI 传输引擎。输入在上升沿采样，返回半字节在下降沿更新。',
    'fspi_slave_top': '连接 S 端 FSPI 时钟、传输和控制寄存器。板级 PLL 保持独立实例。',
    'spi_ram_crtl': '在寄存器时钟下降沿解析读写。完整地址锁存后，已排队的写入仍在下一拍提交。',
    'image_pcie_capture': 'S 端板级连接。整合相机、图像处理、DDR、PCIe DMA 与车控接口。',
    'Classifier': '按颜色、数字和形状匹配规则。数字结果优先，重复匹配沿用最后命中的规则。',
    'ISP': '颜色识别通路。转换后的像素经过二值筛选、先膨胀后腐蚀和区域叠加。',
    'color_binary': '根据 HSV 与 YCbCr 条件产生二值掩码，同时登记原像素、坐标与同步信号。',
    'color_dt': '根据区域边界生成方框叠加。边缘判定、颜色和输出各自沿用原寄存拍。',
    'color_en_generate': '统计有效区域内的颜色像素。计数相同时按红、绿、蓝、黄的次序取优先项。',
    'matrix_3x3_1bit': '用两条 FIFO 生成二值 3×3 窗口。顶部和左侧用最近的像素补齐。',
    'rgb2hsv': '计算 HSV，同时将原 RGB 与同步信号延迟三拍交给后级。',
    'rgb2ycbcr': '按可配置系数转换 RGB。像素、坐标和同步标志通过三级无复位侧带流水线。',
    'dialate': '按行对二值窗口执行或归约，实现膨胀。输出数据与原像素侧带对齐。',
    'erode': '按行对二值窗口执行与归约，实现腐蚀。组合模块可按参数选择两级执行顺序。',
    'fps_calculation': '将 9 位帧率分为十进制百、十、个位，十位和个位由商与余数得到。',
    'fps_count': '在参考时钟域生成统计窗口，在像素时钟域统计帧沿并登记显示值。',
    'fps_display': '根据帧率和识别编号绘制字形。保持原字形表触发条件，绿字在掩码登记后叠加。',
    'isp_ctrl': '将 RGB565 输入送入识别通路，并把颜色叠加结果与显示同步信号一起交给 VGA。',
    'matrix_3x3_8bit': '用行 FIFO 建立 8 位 3×3 邻域。窗口寄存器在消隐拍也更新，行列计数由有效信号控制。',
    'middle_filter': '先排序三行，再对行极值和中值做列归约，最后选取邻域中值。',
    'image_rgb2hsv': '形状识别使用的 HSV 转换，帧、行及数据有效标志经过三级对齐。',
    'image_rgb2ycbcr': '形状识别使用的 YCbCr 转换，颜色分量和像素坐标分别走算术与侧带流水线。',
    'shape': '轮廓识别通路。阈值结果先腐蚀后膨胀，再进行投影和形状判断。',
    'threshold_binary': '在识别区域内按颜色优先级生成掩码。无匹配的像素保留黑色延续状态。',
    'sort': '共用三项输入的有向比较结果。中值分支沿用原选择条件和保持行为。',
    'Line_Shift_RAM': '将两条行延迟串联，后一条接收前一条的输出。',
    'Matrix_Generate_3X3_Buf': '为卷积建立三个像素行的窗口。行无效时沿用原定宽填充值。',
    'RAMshift_taps': '环形行存储。地址达到终点时可在禁写周期回绕，复位拍的写使能仍独立有效。',
    'line_shift_ram_8bit_new': '使用两块 RAM 保存历史行，两个写端分别使用不同的地址、像素和使能延迟。',
    'fspi_slave_m_imu': 'M 端传输适配层。共用 FSPI 引擎，并为状态返回追加锁存的 IMU 快照。',
    'fspi_slave_top_m_imu': '连接 M 端 FSPI、控制寄存器和 IMU 快照输入，保留板级时钟路径。',
    'image_pcie_capture_m_imu': 'M 端板级连接。在相机、DDR 和 PCIe 数据通路旁接入 IMU 与车控。',
    'imu_i2c_master': '开漏 I²C 事务引擎。四分之一周期控制位相，超时释放总线并报告完成。',
    'mpu9250_sampler': '独立探测、配置和采集 IMU。主机请求冻结快照，CRC 在采集时钟域计算。',
    'cmos_capture_data': '采集 OV5640 的 RGB565 字节。启动帧门控、像素有效和坐标计数分别处理。',
    'ov5640_top': '连接各路相机的上电、SCCB 配置与图像采集。像素时钟测量逐路跨域返回。',
    'reg_config': '按表向 OV5640 写 SCCB 寄存器。只有前一笔传输结束才推进配置索引。',
    'remote_vehicle_control': '车控输入同步、挡位估计与按键调度。独立模块负责 STOP、REARM 和 HMI 回显。',
    'uart_lcd': '用滑动字节窗识别屏幕报文。配置、挡位和牵引租期独立更新。',
    'uart_rx': '同步串口输入，在位周期中点采样，结束时更新并行字节与完成标志。',
    'uart_tx': '按起始位、低位优先的数据与停止位串行发送，沿用旧 BUSY 释放时刻。',
}

SECTIONS = {
    'axi4_ctrl': [(rb'\bwire\s+write_payload_ready', '检测到帧尾后，允许不足常规突发门槛的 FIFO 尾数据继续写出。')],
    'axi4_ctrl_3ch': [(rb'\bwire\s+last_write_beat', '仅最后一个被接收的写拍推进列位置。同拍帧同步覆盖该更新。')],
    'spi_ram_crtl': [
        (rb'\balways\s*@', '下降沿各过程读取更新前的寄存器值，片选撤销与已排队写入可在同拍出现。'),
        (rb'\bmem\s*\[ram_addr\[2:0\]\]', '地址范围在锁存时预解码。写数据取提交边沿前已保存的字节。'),
    ],
    'gamma_judge': [(rb'\bif\s*\(per_data_1\s*<=\s*10\x27d570\)', '旁路采用当前输入 Y。曲线分支采用已登记的曲线输出。')],
    'cmos_capture_data': [(rb'\bmodule\s+cmos_capture_data\b', '保持两级同步输出和奇数字节行尾的原有效脉冲行为。')],
    'fspi_slave': [(rb'\breg\s*\[23:0\]\s*status_header', '地址状态读按六个半字节接收命令与地址，随后锁存返回字节。')],
    'rgb_end': [
        (rb'\bif\s*\(channel\s*==\s*1\)', '绿色饱和级使用同步复位。红色和蓝色饱和级在复位拉低时立即清零。'),
        (rb'\{post_img_out_g, post_img_out_b\}\s*<=', '末级绿、蓝输出每拍采样，复位拍也沿用这一规则。'),
        (rb'\bmodule\s+Reciprocal_cnn\b', '倒数表输出采用 1 位整数和 31 位小数，保留对时钟变化的原触发方式。'),
    ],
    'remote_vehicle_control': [
        (rb'\bmodule\s+hmi_gear_feedback\b', '档位文本带 GB2312 后缀，连续三个 FF 结束命令。滑块就绪后追加数值回显。'),
        (rb'\bmodule\s+remote_speed_controller\b', '自动脉冲按状态调度。同拍命令按目标、人工接管、空闲校准的既有顺序覆盖。'),
        (rb'\bmodule\s+remote_stop_controller\b', '制动锁存与档位调度独立。重新使能须遵守原报警及方向空闲条件。'),
    ],
    'fps_display': [(rb'\breg\s*\[79:0\]\s*result_char', '字形表保留原常量过程的事件触发方式。')],
    'fps_count': [(rb'\bwire\s+clear_pulse', '计数清零取像素域同步后的上升沿，显示寄存器仍按原清除电平取样。')],
    'image_pcie_capture': [
        (rb'\bmodule\s+video_dma_line_formatter\b', '行首先发送 15 个全一填充块。填充期间暂停向 AXI 取像素。'),
        (rb'\breg\s*\[11:0\]\s*o_dma_write_addr_dly1', '根据相邻两拍的地址判断行尾。累计 480 行后产生帧读请求。'),
    ],
    'image_pcie_capture_m_imu': [(rb'\bvideo_dma_line_formatter\s+u_dma_line_formatter', 'DMA 使用两端共用的行填充逻辑，IMU 快照走独立 FSPI 返回路径。')],
    'imu_i2c_master': [(rb'\bwire\s+timeout_reached', '主机等待从机放开 SCL 时仍计算事务超时，避免总线被长期占用。')],
    'Matrix_Generate_3X3_Buf': [(rb'\bBLANK_ROW\s*=', '填充常量先按单行位宽扩展或截断，再复制到三行。')],
    'ov5640_top': [(rb'\bmodule\s+ov5640_clock_meter\b', '在像素域锁存计数，再用参考时钟域的秒脉冲逐级取回结果。')],
    'reg_config': [(rb'\bcase\s*\(config_step\)', '发送寄存器字、等待传输完成、递增索引依次占用三个状态。')],
    'mpu9250_sampler': [(rb'\breg\s*\[511:0\]\s*staging', '快照字段在开始 CRC 前一次锁存，计算完毕才更新对外数据。')],
    'threshold_binary': [(rb'\bwire\s+outside_window', '区域边界按包含端点处理。区域外清除黑色延续状态。')],
    'uart_lcd': [
        (rb'\bwire\s+frame_complete', '字节窗须同时满足帧头、长度与间隔条件，才更新对应命令寄存器。'),
        (rb'\bwire\s+traction_active', '方向命令可在租期到期的同一拍刷新状态，命令更新优先。'),
    ],
}

COMMENT = rb'//[^\r\n]*|/\*[\s\S]*?\*/'
STRING = rb'"(?:\\[\s\S]|[^"\\])*"'
ESCAPED = rb'\\[^\s]+'
LITERAL = rb"(?<![\w$])32'([sS]?)[bB]([01xXzZ?_]+)(?![\w$?])"
SCAN = re.compile(rb'(?P<comment>' + COMMENT + rb')|(?P<string>' + STRING +
                  rb')|(?P<escaped>' + ESCAPED + rb')|(?P<literal>' + LITERAL + rb')')
TOOL_DIRECTIVE = re.compile(rb'(?i)\b(?:synthesis|synopsys|pragma|translate_off|translate_on|verilator|lint_off|lint_on)\b')
PROVENANCE = re.compile(rb'(?i)copyright|all rights reserved|spdx|licensed under|created (?:by|date)|'
                        rb'last modified date|create date\s*:|\bauthor\s*:|\bengineer\s*:\s*\S|\bmail\s*:')


def literal_hex(value):
    match = re.fullmatch(LITERAL, value)
    signed, digits = match.groups()
    digits = digits.replace(b'_', b'').lower().replace(b'?', b'z')
    if len(digits) != 32:
        return value
    hex_digits = []
    for start in range(0, 32, 4):
        nibble = digits[start:start + 4]
        if nibble in (b'xxxx', b'zzzz'):
            hex_digits.append(nibble[:1])
        elif set(nibble) <= {48, 49}:
            hex_digits.append(format(int(nibble, 2), 'x').encode())
        else:
            return value
    return b"32'" + signed.lower() + b'h' + b''.join(hex_digits)


def description(stem):
    curve = re.fullmatch(r'Curve_Gamma_(\d+)', stem)
    if curve:
        return f'Gamma 0.{curve[1]} 的定值查找表。每个上升沿登记对应亮度，无匹配项时保持输出。'
    layer = re.fullmatch(r'conv([1-7])', stem)
    if layer:
        return f'第 {layer[1]} 层卷积。卷积支路与 RGBY 对齐支路并行推进，各自保留原取样级。'
    return NOTES[stem]


def executable(data):
    """Keep every non-comment byte and normalize only the requested numeric spelling."""
    def token(match):
        if match['comment'] is not None and not TOOL_DIRECTIVE.search(match[0]):
            return b' ' + b'\n' * match[0].count(b'\n')
        return literal_hex(match[0]) if match['literal'] is not None else match[0]
    # Outside strings, escaped identifiers, and directives, horizontal and vertical
    # whitespace delimit the same tokens. No touched source uses macro continuation.
    parts = re.split(rb'(' + STRING + rb'|' + ESCAPED + rb')', SCAN.sub(token, data))
    return tuple(part if index % 2 else tuple(part.split()) for index, part in enumerate(parts))


def directives(data):
    return [match[0] for match in SCAN.finditer(data)
            if match['comment'] is not None and TOOL_DIRECTIVE.search(match[0])]


def rewrite(data, stem):
    stats = Counter()
    def replace(match):
        value = match[0]
        if match['comment'] is not None:
            if TOOL_DIRECTIVE.search(value) or PROVENANCE.search(value):
                return value
            stats['ordinary_comments_replaced'] += 1
            return b' ' + b'\n' * value.count(b'\n')
        if match['literal'] is not None:
            converted = literal_hex(value)
            if converted != value:
                stats['binary_literals_converted'] += 1
            return converted
        return value

    if re.search(rb'(?m)\\[ \t]*\r?$', data):
        raise ValueError(f'{stem}: macro continuation requires a separate review')
    newline = b'\r\n' if b'\r\n' in data else b'\n'
    header = ('// 2026-11-03 注释校订。' + description(stem)).encode() + newline
    result = SCAN.sub(replace, data)
    # Remove empty runs left by stale prose and commented-out code.
    result = re.sub(rb'(?m)[ \t]+(?=\r?$)', b'', result)
    result = re.sub(rb'(?:\r?\n[ \t]*){2,}\r?\n', newline * 2, result)
    for anchor, note in SECTIONS.get(stem, ()):
        match = re.search(anchor, result)
        if match:
            offset = result.rfind(b'\n', 0, match.start()) + 1
            indent = re.match(rb'[ \t]*', result[offset:]).group()
            result = result[:offset] + indent + b'// ' + note.encode() + newline + result[offset:]
    result = header + result.lstrip(b'\r\n \t')
    result.decode('utf-8')
    if executable(data) != executable(result) or directives(data) != directives(result):
        raise ValueError(f'{stem}: executable tokens or tool directives changed')
    return result, stats


def main():
    inventory = json.loads((HERE / 'expanded_source_inventory.json').read_text(encoding='utf-8'))
    before = HERE / 'pds_final'
    report_path = HERE / 'comment_refresh_results.json'
    previous = json.loads(report_path.read_text(encoding='utf-8'))['files'] if report_path.exists() else {}
    files = {}
    pending = []
    for name, info in inventory['files'].items():
        if 'original_sha256' not in info:
            continue
        old = (before / name).read_bytes()
        revised, counts = rewrite(old, Path(name).stem)
        path = FPGA / name
        current = path.read_bytes()
        if current not in (old, revised) and sha256(current).hexdigest() != previous.get(name, {}).get('sha256'):
            raise ValueError(f'{path}: unexpected local edit')
        pending.append((path, revised))
        files[name] = {**counts, 'sha256': sha256(revised).hexdigest()}
    for path, data in pending:
        if path.read_bytes() == data:
            continue
        tmp = path.with_suffix('.comment-refresh')
        with tmp.open('xb') as stream:
            stream.write(data)
        tmp.replace(path)
    report = {'files': files, 'total_files': len(files),
              'converted_literals': sum(row.get('binary_literals_converted', 0) for row in files.values())}
    report_path.write_text(
        json.dumps(report, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
    print(f"Verified {report['total_files']} sources and {report['converted_literals']} literal conversions")


if __name__ == '__main__':
    main()
