"""Write the audit metrics in plain language without changing any project code."""
import json
import sys
from pathlib import Path

OUT=Path(__file__).resolve().parent
sys.stdout.reconfigure(encoding='utf-8')
ROOT=OUT.parents[1]
data=json.loads((OUT/'report.json').read_text(encoding='utf-8'))
metrics=data['metrics']
rows=[('全部可分析源码（含 SDK/IP 支持及测试）','all_text_source'),
      ('应用与板级支持（排除已识别 SDK/IP 和测试）','application_excluding_vendor_and_tests'),
      ('软件应用合计','software_application_all'),('M 端软件应用','M_software_application'),
      ('S 端软件应用','S_software_application'),('公共应用源码','shared_application'),
      ('FPGA 应用及板级支持（不含已识别 IP/PCIe 支持）','FPGA_application_and_board_support'),
      ('SDK/IP/PCIe 支持','vendor_IP_SDK_or_PCIe_support')]
lines=['提交代码相似度检测报告','日期：2026-11-02','',
    '当前工程：'+data['current_root'],'只读对照：'+data['origin_root'],
    '提交范围：udp_rk_rk_yolo、fpga、HMI、common 四个根目录。','',
    '结论口径',
    '本报告给出工程内部跨文件重复、以及当前工程与 origin 的本地相似性指标。没有访问比赛查重系统、互联网源码数据库或其他参赛队代码，因此不能给出官方查重率、联网查重率或通过概率。',
    '内部重复包括正常的复用、协议接口和相同 IP 副本，外部匹配也不等同于抄袭判定或代码权属认定。','',
    '一、当前工程与 origin 的对比','',
    '下面每组依次列出：源码文件数；字节完全相同文件；忽略注释/排版后整文件相同；连续代码片段覆盖率。',
    '文件相同比例按文件数统计；片段覆盖率按当前工程有效 token 数加权，不能把这两种百分比当作同一指标。','']
for label,key in rows:
    item=metrics[key]
    lines.extend([label,
        '  文件数：{files}；有效 token：{tokens}'.format(**item),
        '  字节完全相同：{exact_external_files}/{files}（{exact_external_file_percent:.2f}%）'.format(**item),
        '  归一化后整文件相同：{normalized_external_files}/{files}（{normalized_external_file_percent:.2f}%）'.format(**item),
        '  origin 片段覆盖当前代码：{external_fragment_token_percent:.2f}%'.format(**item),
        '  去除同 token 整文件副本后，origin 片段覆盖率：{content_deduplicated_external_fragment_percent:.2f}%'.format(**item),''])
lines.extend(['二、当前工程内部重复','',
    '内部跨文件片段覆盖率：当前代码中，有至少 40 个连续有效 token 可在其他文件找到的比例。未将同一文件内部重复单独计入。',
    '整文件副本冗余率：把同 token 文件归组，每组仅保留一份时可去除的 token 占比；它只度量整文件重复，范围小于片段重复。',''])
for label,key in rows:
    item=metrics[key]
    lines.extend([label,
        '  跨文件片段重复覆盖率：{internal_cross_file_fragment_token_percent:.2f}%'.format(**item),
        '  同 token 整文件多余副本：{internal_normalized_extra_copies}；整文件 token 冗余率：{internal_normalized_extra_token_percent:.2f}%'.format(**item),''])
lines.extend(['三、范围与排除项','',
    '排除编译/综合/布局布线输出、build、日志、tmp、output、pds_work、Git 元数据、模型权重及普通图片文档。保留源文件和构建脚本。',
    '完整提交范围内含多个 FPGA 工程及其 IP 目录副本，因此全量口径会重复计入相同 IP。报告同时给出去除同 token 整文件副本后的指标。',
    '厂商/SDK 分类依据路径和明确的版权标识，是工程分类，不是对来源或作者身份的证明。软件应用分类仍包含继承的 SPI、后处理和共享内存等基础实现。',
    '根目录 common 当前只有 .o/.exe 编译产物，没有可分析源码；真正的公共应用源码位于 udp_rk_rk_yolo/common。',
    'main_old.cpp 等虽不参与当前默认编译，但仍在提交目录内，因此仍计入本次源码对比。',''])
for kind,label in [('HMI_binary','HMI/HMI编译产物'),('protected_vendor_IP','受保护 IP 文件')]:
    items=[x for x in data['opaque_artifacts'] if x['kind']==kind]
    matched=sum(bool(x['exact_origin_matches']) for x in items)
    lines.append('%s：%u 个，%u 个与 origin 字节完全相同；仅比较 SHA-256，不计入源码词法覆盖率。'%(label,len(items),matched))
lines.extend(['HMI 事件脚本未从二进制工程中提取，不能把 HMI 文件哈希结果当作事件脚本查重率。',
    '无法解码的源码数：%u。按文件类型和命名识别的静态资源单列；嵌入普通源码文件中的常量数据仍可能计入 token。'%len(data['unreadable_sources']),'',
    '四、方法及限制','',
    '1. 字节相同：对所有候选原始文件做 SHA-256，不要求文件同名或路径相同。',
    '2. 词法归一化：忽略 C/C++/Verilog 的注释和空白；Python 使用 tokenize 并保留缩进结构；脚本只忽略明确的整行注释。标识符、字符串、数值均保留。',
    '3. 片段匹配：在同语言的全部对照文件中匹配至少 40 个连续有效 token，不限同名文件；对哈希命中逐段核对实际 token，再对覆盖区间取并集。能识别同一段代码移动到其他文件的情况。',
    '4. 不做变量改名归一化、AST 或语义等价检测。因此这些结果既可能漏掉改名后的同源实现，也会计入标准算法、接口和正常复用。',
    '5. 格式与注释变化会降低字节相同率，但不会自动消除归一化后的相同实现。',
    '6. 本轮只读取代码；没有修改工程源码、没有连接板端、没有上传代码给第三方。','',
    '详细数据：output/submission_similarity_20261102/report.json',
    '高重合文件及内部副本组：output/submission_similarity_20261102/高重合文件与副本清单.txt'])
(ROOT/'提交代码相似度检测报告_20261102.txt').write_text('\n'.join(lines)+'\n',encoding='utf-8')
details=['高重合源码文件与内部副本清单','说明：片段重合不是抄袭判定，也不是官方查重分数。','',
    '应用与板级支持：片段覆盖率 >=70%、有效 token >=200 的文件','']
selected=[x for x in data['files'] if x['category'] in {'software_application_and_tools','FPGA_application_and_board_support'} and x['tokens']>=200 and x['fragment_percent']>=70]
for item in sorted(selected,key=lambda x:(-x['fragment_percent'],-x['tokens'])):
    details.extend([item['path'],'  origin 片段覆盖 %.2f%%，有效 token %u'%(item['fragment_percent'],item['tokens']),
        '  完全相同：'+str(bool(item['exact_matches']))+'；归一化整文件相同：'+str(bool(item['normalized_matches']))])
    for path,count in item['representative_origin_contributors']:
        details.append('  代表性命中文件：%s（新增覆盖 token %u）'%(path,count))
    details.append('')
details.extend(['内部归一化整文件重复组',''])
for group in sorted(data['internal_duplicate_groups'],key=lambda x:-(x['copies']-1)*x['tokens']):
    details.append('副本 %u，有效 token %u'%(group['copies'],group['tokens']))
    details.extend('  '+path for path in group['files'])
    details.append('')
(OUT/'高重合文件与副本清单.txt').write_text('\n'.join(details)+'\n',encoding='utf-8')
print('REPORT_SAVED '+str(ROOT/'提交代码相似度检测报告_20261102.txt'))
print('HIGH_OVERLAP_APPLICATION_FILES',len(selected))
