"""Inventory editable explanation comments without changing application sources."""
import importlib.util
import io
import json
from pathlib import Path
import re
import tokenize

OUT = Path(__file__).resolve().parent
ROOT = OUT.parents[1]
spec = importlib.util.spec_from_file_location('audit_lexer', ROOT / 'output/submission_similarity_20261102/check_submission.py')
audit = importlib.util.module_from_spec(spec)
spec.loader.exec_module(audit)
SKIP = audit.SKIP | {'tests', 'test', 'ipcore', 'third_party', '3rdparty'}
NOTICE = re.compile(r'copyright|SPDX|license|all rights reserved|author\s*[:：]|作者\s*[:：]|版权所有|confidential|科研轮转|Pango Microsystems|Rockchip Electronics', re.I)
DIRECTIVE = re.compile(r'synthesis|synopsys|syn_keep|syn_preserve|translate_off|translate_on|verilator|lint_|NOLINT|clang-format|IWYU|fall.?through|\btype\s*:|noqa|pylint|pyright|mypy|nosec|pragma|\bfmt\s*:|coding[:=]|^#!', re.I)

def candidates():
    for base in [ROOT / 'udp_rk_rk_yolo', ROOT / 'fpga/fpga_pcie_ov5640/source']:
        for path in sorted(base.rglob('*')):
            if not path.is_file() or set(path.relative_to(base).parts[:-1]) & SKIP:
                continue
            if base.name == 'source' and 'pcie' in path.relative_to(base).parts[:-1]:
                continue
            if path.suffix.lower() not in audit.C_EXT | audit.V_EXT | {'.py'}:
                continue
            raw = path.read_bytes()
            try:
                text = raw.decode('utf-8-sig')
            except UnicodeDecodeError:
                continue
            relative = path.relative_to(ROOT).as_posix()
            if audit.role(relative, text) in {'vendor_IP_SDK_or_PCIe_support', 'static_resource_source'}:
                continue
            if path.name.startswith(('rknn_', 'stb_', 'ips2l_', 'ipm2l_', 'GTP_')):
                continue
            yield path, text

def comment_spans(path, text):
    if path.suffix == '.py':
        offsets = [0]
        for line in text.splitlines(keepends=True):
            offsets.append(offsets[-1] + len(line))
        for token in tokenize.generate_tokens(io.StringIO(text).readline):
            if token.type == tokenize.COMMENT:
                yield offsets[token.start[0]-1] + token.start[1], offsets[token.end[0]-1] + token.end[1], token.string
    else:
        lexer = audit.V_LEX if path.suffix.lower() in audit.V_EXT else audit.C_LEX
        for match in lexer.finditer(text):
            if match.group().startswith(('//', '/*')):
                yield match.start(), match.end(), match.group()

def main():
    records = []
    files = []
    for path, text in candidates():
        relative = path.relative_to(ROOT).as_posix()
        count = 0
        for start, end, comment in comment_spans(path, text):
            reason = 'notice' if NOTICE.search(comment) else 'directive' if DIRECTIVE.search(comment) else 'line_splice' if re.search(r'\\[\r\n]', comment) or comment.rstrip().endswith('\\') else None
            records.append(dict(id=len(records), path=relative, start=start, end=end,
                                line=text.count('\n', 0, start)+1, comment=comment, preserve=reason))
            count += 1
        files.append(dict(path=relative, comments=count))
    (OUT/'inventory.json').write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding='utf-8')
    (OUT/'files.json').write_text(json.dumps(files, ensure_ascii=False, indent=2), encoding='utf-8')
    editable = [r for r in records if not r['preserve']]
    (OUT/'editable.txt').write_text('\n'.join(str(r['id'])+' '+r['path']+':'+str(r['line'])+'\n'+r['comment'] for r in editable), encoding='utf-8')
    print(json.dumps(dict(files=len(files), comments=len(records), editable=len(editable), preserved=len(records)-len(editable), editable_characters=sum(len(r['comment']) for r in editable)), ensure_ascii=False))

if __name__ == '__main__':
    main()
