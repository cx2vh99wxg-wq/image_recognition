"""Revise explanation comments while preserving every executable source slice.

No application, simulation, sensor reader, compiler or board program is run.
The output records distinguish rewording from documentation-style cleanup.
"""
import argparse
from collections import Counter, defaultdict
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import sys

from inventory import OUT, ROOT, audit, comment_spans
from rewrite_tables import BY_ID, LABELS

sys.stdout.reconfigure(encoding='utf-8')

def digest(raw):
    return hashlib.sha256(raw).hexdigest()

def content(raw):
    if raw.startswith('/*'):
        text = raw[2:-2]
        text = re.sub(r'(?m)^[ \t]*\*[ \t]?', '', text)
    elif raw.startswith('//'):
        text = raw[2:]
    else:
        text = raw[1:]
    return re.sub(r'\s+', ' ', text).strip()

def scope(path):
    p = path.as_posix()
    scopes = [('/common/image/', '输入图像'), ('/common/perception/', '感知接口'),
        ('/common/transport/', '视频报文'), ('/common/steering/', '转向契约'),
        ('/imu/', '角度采集'), ('/steering/', '转向控制'), ('/fspi/', '寄存器访问'),
        ('/x11/', '显示合成'), ('/udp/', '网络传输'), ('/pcie/', '图像处理'),
        ('/shared_memory.', '共享数据'), ('/mpu9250/', 'IMU 硬件'),
        ('/cnn/', '卷积流水线'), ('/ov5640', '摄像头时序'),
        ('/axi_ctrl/', '存储接口'), ('/martix/', '像素窗口'), ('/isp/', '图像流水线'),
        ('/HDMI/', '屏幕时序'), ('remote_vehicle_control', '车控仲裁')]
    for fragment, label in scopes:
        if fragment in p:
            return label
    return '本模块'

def source_example(text):
    return bool(re.match(r'(?:printf|fprintf|std::|memcpy|memset|free|malloc|rknn_|assign\b|always\b|wire\b|reg\b|input\b|output\b|if\s*\(|for\s*\(|while\s*\()', text) or
                re.match(r'[A-Za-z_]\w*(?:\[[^\]]+\])?\s*(?:<=|=(?!=))\s*[^,]+;', text) or
                re.search(r'\balways\s*@|\b(?:wire|reg)\s*\[|\.[A-Za-z_]\w*\s*\([^)]*\)\s*[,;]|\}\s*<=',text))

def wording(text, path, manual):
    if text in manual:
        return manual[text], 'reviewed_rewording'
    if text in LABELS:
        return LABELS[text], 'reviewed_label'
    if re.fullmatch(r'[/*=\-_*~#\s]+', text) or not text:
        return '── '+scope(path)+' ──', 'section_layout'
    if source_example(text):
        return '停用示例，仅供对照：'+text, 'inactive_example_label'
    # Numeric references, units and formulas retain every value and unit.
    if re.fullmatch(r'[0-9\sA-Za-z_\'"()\[\].,+*/=<>%:;?\-]+',text) and re.search(r'\d',text) and not re.search(r'[A-Za-z]{8,}',text):
        return '参数记录为 '+text+'。', 'parameter_annotation'
    if re.search(r'[�]|\?\?',text):
        return '旧说明含编码损坏，原记录为：'+text,'damaged_text_annotation'
    if '函数名称:' in text or '函数名称：' in text:
        revised = text
        for old,new in [('函数名称','接口'),('功能描述','执行行为'),('输入参数','输入约定'),('算法说明','处理顺序')]:
            revised=revised.replace(old,new)
        revised=re.sub(r'\*{2,}', '',revised).strip()
        return revised,'reviewed_header_layout'
    if re.match(r'^[*/=\-]{3,}\s*\S',text):
        return re.sub(r'^[*/=\-]+\s*','',text).strip('*/=- ')+'。','section_layout'
    # A pure named boundary is more useful as a complete scope annotation.
    match = re.fullmatch(r'(?:namespace\s+)?([A-Za-z_]\w*(?:::\w+)*)', text)
    if match:
        return match[1]+'：本节对应的标识或作用域。', 'named_scope_label'
    # Turn terse Chinese field labels into complete, unambiguous descriptions.
    if re.search(r'[\u4e00-\u9fff]',text) and len(text)<55 and not re.search(r'[。；;{}]',text):
        rules = [
            (r'^(.+?)的定义$',r'本节定义\1。'),
            (r'^(.+?)定义$',r'本节给出\1的定义。'),
            (r'^(.+?)声明$',r'本节声明\1。'),
            (r'^(.+?)函数实现$',r'下面实现\1接口。'),
            (r'^(.+?)函数$',r'处理\1的接口。'),
            (r'^(.+?)结构体$',r'描述\1的数据结构。'),
            (r'^(.+?)有效标志$',r'标记\1是否有效。'),
            (r'^(.+?)有效信号$',r'指示\1是否处于有效区间。'),
            (r'^(.+?)使能信号$',r'用于决定\1是否执行。'),
            (r'^(.+?)使能控制$',r'决定\1的使能状态。'),
            (r'^(.+?)标志寄存$',r'寄存\1对应的标志。'),
            (r'^(.+?)计数器$',r'对\1进行累计计数。'),
            (r'^(.+?)参数$',r'\1所使用的配置参数。'),
            (r'^(.+?)例化$',r'把\1连接到本模块。'),
            (r'^(.+?)实例化$',r'创建并连接\1模块。'),
            (r'^(.+?)接口$',r'用于连接\1的端口。'),
            (r'^(.+?)信号$',r'本信号用于\1。'),
            (r'^(.+?)标志$',r'记录\1状态的标志。'),
            (r'^将(.+?)转换为(.+?)$',r'把\1转为\2。'),
            (r'^将(.+?)转换到(.+?)$',r'把\1映射到\2。'),
            (r'^计算(.+?)$',r'求出\1。'),
            (r'^检查(.+?)$',r'核对\1。'),
            (r'^设置(.+?)$',r'配置\1。'),
            (r'^保存(.+?)$',r'保留\1以供后续使用。'),
            (r'^缓存(.+?)$',r'把\1保存在本地。'),
            (r'^更新(.+?)$',r'按本轮结果推进\1。'),
            (r'^检测到(.+?)$',r'已识别出\1。'),
            (r'^读取(.+?)$',r'取得\1的当前内容。'),
            (r'^释放(.+?)$',r'归还已取得的\1。'),
            (r'^等待(.+?)$',r'等候\1。'),
            (r'^输出(.+?)$',r'向下一级提供\1。'),
        ]
        for pattern,replacement in rules:
            if re.fullmatch(pattern,text):
                return re.sub(pattern,replacement,text),'field_prose_rewording'
    # Reword complete prose without changing identifiers, thresholds or values.
    updated = text
    pairs = [
        ('不应该','不应'), ('不需要','无需'), ('不允许','禁止'), ('只需要','只需'),
        ('注意：','需注意：'), ('注意:','需注意：'), ('如果','若'), ('否则','其他情况下'),
        ('确保','保证'), ('用于','用来'), ('通过调用','调用'), ('进行初始化','建立初始状态'),
        ('动态分配','按需申请'), ('申请内存','申请存储空间'), ('释放内存','归还存储空间'),
        ('清理资源','回收已取得的资源'), ('保持不变','保持原样'), ('默认情况下','采用默认设置时'),
        ('情况下','情形下'), ('以便','以供'), ('此时','在这一阶段'), ('这里','本处'),
        ('最后','末尾'), ('首先','先'), ('然后','随后'), ('之后','以后'), ('之前','以前'),
        ('获取当前','读取当前'), ('计算每个','逐项计算'), ('检查是否','判断是否'),
        ('无效参数','不合法参数'),
    ]
    for old, new in pairs:
        updated = updated.replace(old, new)
    match = re.match(r'^初始化\s*(\S.*)$', updated)
    if match:
        updated = '为'+match[1]+'建立初始状态。'
    match = re.match(r'^分配\s*(\S.*)$', updated)
    if match:
        updated = '为'+match[1]+'申请所需空间。'
    match = re.match(r'^定义\s*(\S.*)$', updated)
    if match:
        updated = '本节声明'+match[1]+'。'
    match = re.match(r'^获取\s*(\S.*)$', updated)
    if match:
        updated = '读取'+match[1]+'。'
    match = re.match(r'^存储\s*(\S.*)$', updated)
    if match:
        updated = '保存'+match[1]+'。'
    if updated != text:
        return updated, 'terminology_rewording'
    return text, 'style_only'

def render(original, text, path, category):
    trailing_cr = '\r' if original.endswith('\r') else ''
    normalized = original.replace('\r\n', '\n')
    breaks = normalized.count('\n')
    if original.startswith('//'):
        # Keep CR inside the same lexical comment span as in the input.
        # Moving it past a new block terminator would change an outside slice.
        return '// '+(scope(path)+'：' if category == 'style_only' else '')+text+trailing_cr
    if original.startswith('#'):
        # Python comments are tokens; docstrings and every other string stay intact.
        return '# '+(scope(path)+'：' if category == 'style_only' else '')+text
    if category == 'inactive_example_label':
        # Keep the exact disabled code and its layout, including old register
        # tables. Only its heading states explicitly that it is not compiled.
        return original[:2]+' 停用示例，仅供对照：'+original[2:]
    if not breaks:
        if category == 'style_only':
            text = scope(path)+'：'+text
        return ('/** ' if original.startswith('/**') else '/* ')+text+' */'
    # Preserve the count and exact spelling of line separators for __LINE__.
    separators = re.findall(r'\r\n|\n|\r', original)
    assert len(separators) == breaks
    count = breaks+1
    old_lines = normalized.split('\n')
    prefix = next((re.match(r'[ \t]*', line).group() for line in old_lines[1:] if line.strip()), ' ')
    pieces = []
    # Spread the paragraph by character count so long Chinese passages do not
    # become one enormous line; technical spellings and order are unchanged.
    rest = text
    for remaining in range(count, 0, -1):
        if remaining == 1:
            piece, rest = rest, ''
        else:
            target = max(1, (len(rest)+remaining-1)//remaining)
            def allowed(position):
                if position<=0 or position>=len(rest):
                    return True
                safe_ascii=r'[A-Za-z0-9_./\\:+?=<>\-]'
                return not (re.fullmatch(safe_ascii,rest[position-1]) and re.fullmatch(safe_ascii,rest[position]))
            choices = [pos+1 for pos in range(max(0,target-16),min(len(rest),target+17))
                       if rest[pos] in ' ，。；,; ' and allowed(pos+1)]
            if not choices:
                choices=[pos for pos in range(1,len(rest)+1) if allowed(pos)]
            split = min(choices,key=lambda value:abs(value-target)) if choices else len(rest)
            piece, rest = rest[:split].rstrip(), rest[split:].lstrip()
        pieces.append(piece)
    lines = [(''+'/** ' if original.startswith('/**') else '/* ')+pieces[0]]
    lines.extend(prefix+'* '+piece for piece in pieces[1:])
    lines[-1] += ' */'
    return ''.join(line+(separators[i] if i<len(separators) else '') for i,line in enumerate(lines))

def language(path):
    return 'python' if path.suffix=='.py' else 'verilog' if path.suffix.lower() in audit.V_EXT else 'c_family'

def function_macros(text):
    joined = re.sub(r'\\\r?\n', '', text)
    return [(name,tail.startswith('(')) for name,tail in re.findall(r'^\s*#\s*define\s+(\w+)([^\n]*)',joined,re.M)]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    records = json.loads((OUT/'inventory.json').read_text(encoding='utf-8'))
    manual = {content(records[index]['comment']): new for index,new in BY_ID.items()}
    grouped = defaultdict(list)
    for record in records:
        grouped[record['path']].append(record)
    results, revisions, pending = [], [], []
    kinds = Counter()
    for relative, entries in grouped.items():
        path = ROOT/relative
        raw = path.read_bytes()
        text = raw.decode('utf-8-sig')
        updated = text
        changed = 0
        for entry in reversed(entries):
            assert text[entry['start']:entry['end']]==entry['comment'],relative
            if entry['preserve']:
                continue
            old = entry['comment']
            replacement, category = wording(content(old), path, manual)
            replacement = render(old,replacement,path,category)
            if replacement==old:
                # An already normalized block keeps its content and receives
                # a scope label; the record is explicitly marked as style-only.
                replacement = render(old,scope(path)+'：'+content(old),path,'scope_layout')
                category = 'style_only'
            assert replacement!=old,(relative,entry['line'])
            assert replacement.count('\n')==old.count('\n'),relative
            assert replacement.count('\r')==old.count('\r'),relative
            updated = updated[:entry['start']]+replacement+updated[entry['end']:]
            changed += 1
            kinds[category] += 1
            revisions.append(dict(path=relative,line=entry['line'],kind=category,before=old,after=replacement))
        if not changed:
            continue
        # Check executable spelling, strings, numeric literals, macro kind,
        # comment boundaries and line count, before any source is written.
        assert audit.lex(text,language(path))==audit.lex(updated,language(path)),relative+' token mismatch'
        assert function_macros(text)==function_macros(updated),relative+' function macro mismatch'
        assert text.count('\n')==updated.count('\n'),relative+' line count mismatch'
        before_spans = list(comment_spans(path,text))
        after_spans = list(comment_spans(path,updated))
        assert len(before_spans)==len(after_spans),relative+' comment boundary mismatch'
        def executable_slices(source, spans):
            prior = 0
            parts = []
            for start,end,_ in spans:
                parts.append(source[prior:start])
                prior = end
            parts.append(source[prior:])
            return parts
        assert executable_slices(text,before_spans)==executable_slices(updated,after_spans),relative+' noncomment byte mismatch'
        data = updated.encode('utf-8')
        if raw.startswith(b'\xef\xbb\xbf'):
            data = b'\xef\xbb\xbf'+data
        result = dict(path=relative,comments_changed=changed,before_sha256=digest(raw),after_sha256=digest(data),
            noncomment_slices_identical=True,tokens_identical=True,function_macros_identical=True,line_count_identical=True)
        results.append(result)
        if args.apply:
            backup = OUT/'before'/relative
            assert not backup.exists(),'Snapshot already exists; do not overwrite it'
            pending.append((backup,path,raw,data))
    # Reject the complete batch before writing any application source if a
    # single file fails one of the lexical or byte-slice invariants above.
    for backup,path,raw,data in pending:
        assert path.read_bytes()==raw,'Source changed since preview: '+str(path)
    for backup,path,raw,data in pending:
        backup.parent.mkdir(parents=True,exist_ok=True)
        backup.write_bytes(raw)
        path.write_bytes(data)
    record = dict(applied=args.apply,files_changed=len(results),comments_changed=sum(x['comments_changed'] for x in results),
        categories=dict(kinds),preserved=[x for x in records if x['preserve']],files=results,
        runtime_tests_run=False,board_programs_changed=False,firmware_programmed=False)
    (OUT/('result.json' if args.apply else 'preview.json')).write_text(json.dumps(record,ensure_ascii=False,indent=2),encoding='utf-8')
    (OUT/('revisions.json' if args.apply else 'preview_revisions.json')).write_text(json.dumps(revisions,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps({key:value for key,value in record.items() if key not in {'files','preserved'}},ensure_ascii=False))

if __name__ == '__main__':
    main()
