from pathlib import Path
import sys, json, re, tokenize, io, subprocess
sys.dont_write_bytecode = True
work = Path(__file__).resolve().parent
sys.path.insert(0, str(work.parent/'submission_similarity_20261102'))
import compare_directories as compare
import check_submission as audit

def positions(root, path):
    source = (root/path).read_bytes()
    try: source = source.decode('utf-8-sig')
    except UnicodeDecodeError: source = source.decode('gb18030')
    if path.endswith('.py'):
        ignored = {tokenize.COMMENT, tokenize.NL, tokenize.ENCODING, tokenize.ENDMARKER}
        return [tok.start[0] for tok in tokenize.generate_tokens(io.StringIO(source).readline) if tok.type not in ignored]
    if path.endswith(tuple(audit.SCRIPT_EXT)) or path.lower().endswith('/makefile'):
        source = re.sub(r'(?m)^[ \t]*#[^\n]*', lambda m: ' '*len(m[0]), source)
        lexer = audit.S_LEX
    else:
        lexer = audit.C_LEX
    original = source
    joins = [m for m in re.finditer(r'\\\r?\n', source)]
    source = re.sub(r'\\\r?\n', '', source)
    from bisect import bisect_right
    removed = 0
    offsets = []
    lengths = [0]
    for m in joins:
        offsets.append(m.start()-removed)
        removed += len(m[0])
        lengths.append(removed)
    newlines = [m.start() for m in re.finditer('\n', original)]
    return [1+bisect_right(newlines, m.start()+lengths[bisect_right(offsets,m.start())]-1)
            for m in lexer.finditer(source) if not m[0].startswith(('//','/*'))]

audit.TOPS = ('udp_rk_rk_yolo',)
live = audit.ROOT
if len(sys.argv)>2:
    audit.ROOT = Path(sys.argv[2]).resolve()
current, _, _, missing = audit.scan(audit.ROOT)
origin, _, _, other_missing = audit.scan(audit.ORIGIN)
def historical(r):
    return r['path'].endswith('/S/udp/csrc/main_old.cpp') or r['path'].endswith('/S/udp/include/pcie_dma_read_test.h')
retired = {
    'udp_rk_rk_yolo/S/pcie/csrc/fspi_module.c',
    'udp_rk_rk_yolo/S/pcie/include/fspi_module.h',
    'udp_rk_rk_yolo/M/pcie/include/pcie_capture.h',
    'udp_rk_rk_yolo/M/udp/include/udp_network.h',
}
def historical_expanded(r):
    return historical(r) or r['path'] in retired
def vendor(r):
    return r['category'] == 'vendor_IP_SDK_or_PCIe_support'
scopes = {}
for name, select in [
    ('all', lambda r: True), ('previous_app', audit.application),
    ('app', lambda r: audit.application(r) and not historical(r)),
    ('vendor', vendor), ('history', historical),
    ('history_expanded',historical_expanded),
    ('active_app',lambda r: audit.application(r) and not historical_expanded(r)),
    ('tests', lambda r: r['category']=='tests')]:
    left = [r for r in current if select(r)]
    right = [r for r in origin if select(r)] if name in {'previous_app','app','active_app'} else origin
    metrics, blocks = compare.compare(left, right)
    for b in blocks:
        n = b['longest_match_tokens']
        for key, root in [('current', audit.ROOT), ('origin', audit.ORIGIN)]:
            lines = positions(root, b[key])
            i = b[key+'_start_token']
            b[key+'_lines'] = [lines[i], lines[i+n-1]]
    scopes[name] = dict(metrics=metrics, blocks=blocks)
    print(name, metrics['covered_tokens'], metrics['tokens'], metrics['coverage_percent'], metrics['large_regions'],
          blocks[0]['longest_match_tokens'] if blocks else 0)
(work/(sys.argv[1]+'.json')).write_text(json.dumps(dict(scopes=scopes,unreadable=missing+other_missing),indent=2),encoding='utf-8')
