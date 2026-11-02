"""Apply a consistent layout while rejecting any non-comment token change.

This is formatting, not an originality claim. Vendor files, tests, calibration,
models, HMI, wire values and source/copyright notices are retained.
"""
import hashlib
import json
from pathlib import Path
import re
import subprocess

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
CLANG=Path('C:/Users/ASUS/.vscode/extensions/ms-vscode.cpptools-1.29.0-win32-x64/LLVM/bin/clang-format.exe')
TOKEN=re.compile(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|'
    r'[A-Za-z_$][A-Za-z0-9_$]*|\d+(?:\.\d+)?|[^\s]',re.MULTILINE)
def tokens(text):
    text=re.sub(r'\\\r?\n','',text)
    return [x.group() for x in TOKEN.finditer(text) if not x.group().startswith(('//','/*'))]
def function_macros(text):
    text=re.sub(r'\\\r?\n','',text)
    return [(name,tail.startswith('(')) for name,tail in re.findall(r'^\s*#\s*define\s+(\w+)([^\n]*)',text,re.MULTILINE)]

selected=[]
for base in [ROOT/'udp_rk_rk_yolo',ROOT/'fpga/fpga_pcie_ov5640/source']:
    for path in base.rglob('*'):
        if not path.is_file() or path.suffix.lower() not in {'.c','.cpp','.h','.hpp','.v','.vh','.sv'}:continue
        relative=path.relative_to(ROOT).as_posix()
        if any(part in {'build','logs','tests','test','ipcore','pcie'} for part in path.relative_to(base).parts[:-1]):
            # Software's pcie application is ours; only FPGA PCIe support is excluded.
            if not (base.name=='udp_rk_rk_yolo' and not any(part in {'build','logs','tests','test','ipcore'} for part in path.relative_to(base).parts[:-1])):continue
        if path.name.startswith(('test_','rknn_','stb_','ips2l_','ipm2l_','GTP_')):continue
        raw=path.read_bytes()
        try:text=raw.decode('utf-8-sig')
        except UnicodeDecodeError:continue
        if re.search(r'Copyright|SPDX-License|confidential|All rights reserved',text[:4096],re.IGNORECASE):continue
        if re.search(r'\b__(?:LINE|FILE|COUNTER)__\b',text):continue
        selected.append((path,raw,text))
results=[]
for path,raw,text in selected:
    process=subprocess.run([str(CLANG),'--style=file','--assume-filename='+str(path)],
                           input=raw,capture_output=True,cwd=ROOT)
    relative=path.relative_to(ROOT).as_posix()
    if process.returncode:
        results.append(dict(path=relative,changed=False,skipped=process.stderr.decode(errors='replace')))
        continue
    formatted=process.stdout.decode('utf-8-sig')
    if tokens(text)!=tokens(formatted) or function_macros(text)!=function_macros(formatted):
        results.append(dict(path=relative,changed=False,skipped='Token or function-macro change rejected'))
        continue
    # Preserve byte order mark and line ending convention for existing editors.
    normalized=formatted.replace('\r\n','\n')
    newline='\r\n' if b'\r\n' in raw else '\n'
    updated=normalized.replace('\n',newline).encode('utf-8')
    if raw.startswith(b'\xef\xbb\xbf'):updated=b'\xef\xbb\xbf'+updated
    if updated!=raw:path.write_bytes(updated)
    results.append(dict(path=relative,changed=updated!=raw,tokens_preserved=True,
                        before_sha256=hashlib.sha256(raw).hexdigest(),after_sha256=hashlib.sha256(updated).hexdigest()))
(OUT/'format_manifest.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
print('LAYOUT_NORMALIZED considered=%u changed=%u rejected_or_skipped=%u; no runtime tests' % (
    len(results),sum(x['changed'] for x in results),sum('skipped' in x for x in results)))
