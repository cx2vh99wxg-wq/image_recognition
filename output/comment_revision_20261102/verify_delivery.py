"""Independently inspect source bytes and Python syntax; no runtime execution."""
import ast
from collections import Counter, defaultdict
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

from inventory import OUT, ROOT, audit, comment_spans

sys.stdout.reconfigure(encoding='utf-8')
def sha(data):
    return hashlib.sha256(data).hexdigest()

def pieces(source, spans):
    result=[]
    previous=0
    for start,end,_ in spans:
        result.append(source[previous:start])
        previous=end
    result.append(source[previous:])
    return result

def macros(source):
    source=re.sub(r'\\\r?\n','',source)
    return [(name,tail.startswith('(')) for name,tail in re.findall(r'^\s*#\s*define\s+(\w+)([^\n]*)',source,re.M)]

def main():
    result=json.loads((OUT/'result.json').read_text(encoding='utf-8'))
    inventory=json.loads((OUT/'inventory.json').read_text(encoding='utf-8'))
    grouped=defaultdict(list)
    for entry in inventory:
        grouped[entry['path']].append(entry)
    # Remove only trailing whitespace introduced inside revised block comments.
    revisions=json.loads((OUT/'revisions.json').read_text(encoding='utf-8'))
    by_file=defaultdict(list)
    for change in revisions:
        by_file[change['path']].append(change)
    polished=[]
    for entry in result['files']:
        path=ROOT/entry['path']
        raw=path.read_bytes()
        assert sha(raw)==entry['after_sha256'],entry['path']+' changed outside this batch'
        text=raw.decode('utf-8-sig')
        for change in by_file[entry['path']]:
            old=change['after']
            if not old.startswith('/*') or change['kind']=='inactive_example_label':
                continue
            new=re.sub(r'[ \t]+(?=\r?$)','',old,flags=re.M)
            if new!=old:
                assert text.count(old)==1,'Ambiguous block for whitespace polishing'
                text=text.replace(old,new,1)
                change['after']=new
                polished.append(dict(path=entry['path'],line=change['line']))
        data=text.encode('utf-8')
        if raw.startswith(b'\xef\xbb\xbf'):
            data=b'\xef\xbb\xbf'+data
        if data!=raw:
            path.write_bytes(data)
            entry['after_sha256']=sha(data)
    (OUT/'result.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    (OUT/'revisions.json').write_text(json.dumps(revisions,ensure_ascii=False,indent=2),encoding='utf-8')

    checked=[]
    python_files=0
    preserved=0
    changed=0
    for entry in result['files']:
        path=ROOT/entry['path']
        before_raw=(OUT/'before'/entry['path']).read_bytes()
        after_raw=path.read_bytes()
        assert sha(before_raw)==entry['before_sha256'],entry['path']+' backup mismatch'
        assert sha(after_raw)==entry['after_sha256'],entry['path']+' delivery mismatch'
        before=before_raw.decode('utf-8-sig')
        after=after_raw.decode('utf-8-sig')
        left=list(comment_spans(path,before))
        right=list(comment_spans(path,after))
        assert len(left)==len(right)==len(grouped[entry['path']]),entry['path']
        assert pieces(before,left)==pieces(after,right),entry['path']+' executable bytes changed'
        lang='python' if path.suffix=='.py' else 'verilog' if path.suffix.lower() in audit.V_EXT else 'c_family'
        assert audit.lex(before,lang)==audit.lex(after,lang),entry['path']+' tokens changed'
        assert macros(before)==macros(after),entry['path']+' macro kind changed'
        assert before.count('\n')==after.count('\n') and before.count('\r')==after.count('\r'),entry['path']+' line count changed'
        for record,old,new in zip(grouped[entry['path']],left,right):
            assert old[2]==record['comment']
            if record['preserve']:
                assert old[2]==new[2],entry['path']+' protected comment changed'
                preserved+=1
            else:
                assert old[2]!=new[2],entry['path']+' ordinary comment was not processed'
                changed+=1
        if path.suffix=='.py':
            assert ast.dump(ast.parse(before,type_comments=True),include_attributes=False)==ast.dump(ast.parse(after,type_comments=True),include_attributes=False),entry['path']+' AST changed'
            python_files+=1
        checked.append(entry['path'])
    assert changed==result['comments_changed']
    protected=[]
    baseline=json.loads((ROOT/'output/application_refactor_20261102/baseline_manifest.json').read_text(encoding='utf-8'))
    for entry in baseline['files']:
        name=entry['path']
        if Path(name).suffix in {'.fdc','.pcf','.rknn','.HMI','.tft','.cal','.table'} or name.endswith(('runtime.json','homing.json','auto_home.current.json')):
            assert sha((ROOT/name).read_bytes())==entry['sha256'],name+' protected data mismatch'
            protected.append(name)
    firmware=json.loads((ROOT/'output/application_refactor_20261102/firmware_candidates.json').read_text(encoding='utf-8'))
    for board in firmware:
        for extension in ['sbit','sfc']:
            assert sha(Path(board[extension]).read_bytes())==board[extension+'_sha256']
    branch=subprocess.check_output(['git','branch','--show-current'],cwd=ROOT,text=True).strip()
    assert branch=='prelim_code',branch
    groups=Counter('FPGA' if name.startswith('fpga/') else 'M' if '/M/' in name else 'S' if '/S/' in name else 'common' for name in checked)
    evidence=dict(branch=branch,files_checked=len(checked),comments_changed=changed,
        protected_comments_in_changed_files=preserved,protected_comments_total=len(result['preserved']),
        executable_source_slices_identical=True,language_tokens_identical=True,
        function_macro_forms_identical=True,line_counts_identical=True,python_AST_identical_files=python_files,
        protected_data_files=protected,firmware_files_unchanged=4,files_by_group=dict(groups),
        whitespace_polished=polished,runtime_tests_run=False,programs_started_or_stopped=False,
        board_files_replaced=False,FPGA_programmed=False,origin_modified=False)
    (OUT/'verification.json').write_text(json.dumps(evidence,ensure_ascii=False,indent=2),encoding='utf-8')
    counts=Counter(entry['preserve'] for entry in result['preserved'])
    lines=['注释整理及静态核对说明','日期：2026-11-02','工程：'+str(ROOT),'分支：'+branch,'',
        '本轮直接修改本地 '+str(len(checked))+' 个源文件，共处理 '+str(changed)+' 条普通注释。',
        '范围：udp_rk_rk_yolo 中的自编 C/C++/头文件/Python 注释，以及 fpga/fpga_pcie_ov5640/source 中排除厂商与 PCIe 支持目录后的 Verilog 注释。',
        'M/S 业务代码、公共模块及 FPGA 自编源文件均包含在内。','',
        '处理方式包括关键契约重述、短字段补全、术语调整、分节及作用域整理、停用示例标注和参数说明整理。',
        '其中 '+str(result['categories'].get('style_only',0))+' 条只整理注释格式或补充所属模块，原说明文字并未逐字重写；不把这部分称作全新内容。',
        '处理类别与数量：'+json.dumps(result['categories'],ensure_ascii=False),'',
        '保留项：'+str(len(result['preserved']))+' 条，原因分布为 '+json.dumps(dict(counts),ensure_ascii=False)+'。',
        '许可证、作者/来源、工具指令以及反斜杠续行相关注释保留原文。厂商 SDK/IP、生成资源、测试目录不做本轮全量改写。',
        'HMI/TFT 二进制、引脚与时序约束、模型、标定映射、配置、Shell/Make/Tcl 文件不在此次注释改写范围内。',
        'Python 文档字符串属于字符串值，并非 COMMENT token，本轮不改写；Python shebang、编码及类型/检查工具指令同样保留。','',
        '静态核对：所有改动文件的每一段注释外源码逐段一致；字符串、数值、标识符、运算符与宏形式一致；CR/LF 数量不变。',
        str(python_files)+' 个已修改 Python 文件的语法树（不计位置属性）一致。'+str(len(protected))+' 个模型/HMI/引脚/标定配置保护文件与既有基线哈希一致。',
        '先前交付的四个 S/M SBIT/SFC 文件哈希一致，本轮不需要因注释整理而重新烧录。','',
        '本轮仅在本地做静态源代码检查，不运行采集、识别、IMU、车控或模拟测试；没有连接、重启或覆盖 S/M 板正式程序，没有烧录 FPGA。',
        '本轮注释修改之后，先前源码候选包的完整文件哈希属于历史记录，不能再声称旧包与当前本地源码逐字节相同；其运行代码没有因本轮发生变化。',
        '注释格式改变不构成原创证明，不承诺任何官方代码相似度结果。','',
        '原文件备份：'+str(OUT/'before'),
        '逐条注释前后记录：'+str(OUT/'revisions.json'),
        '逐文件哈希与检查结果：'+str(OUT/'result.json')+'；'+str(OUT/'verification.json'),
        '','修改文件清单：',*map(lambda name:str(ROOT/name),checked),'']
    (ROOT/'注释整理核对说明_20261102.txt').write_text('\n'.join(lines),encoding='utf-8')
    print(json.dumps({key:value for key,value in evidence.items() if key not in {'protected_data_files','whitespace_polished'}},ensure_ascii=False))

if __name__=='__main__':
    main()
