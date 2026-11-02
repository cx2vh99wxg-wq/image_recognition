"""Local source-overlap audit; these metrics are not a contest plagiarism score."""
from collections import Counter, defaultdict
import hashlib
import json
import os
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
ORIGIN = ROOT.parent / 'image_recognition_origin'
OUT = Path(__file__).resolve().parent
SOURCE = {'.c','.cc','.cpp','.cxx','.h','.hh','.hpp','.v','.vh','.sv','.svh','.py','.sh','.ps1','.tcl'}
SKIP = {'.git','tmp','output','pds_work','build','__pycache__','log','logs','logbackup',
        'compile','synthesize','device_map','place_route','report_timing','generate_bitstream','generate_netlist'}
TOKEN = re.compile(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|'
                   r'[A-Za-z_$][A-Za-z0-9_$]*|\d+(?:\.\d+)?|[^\s]', re.MULTILINE)

def classify(relative):
    lower = relative.lower()
    if '/ipcore/' in lower or '/source/pcie/' in lower or '/source/pcie_dma' in lower:
        return 'vendor_IP_or_PCIe_support_review_separately'
    if any(x in lower for x in ['/3rdparty/','/third_party/','/third-party/','/rknn_api','/rknn_matmul','/stb_','/rga/','/librga/']):
        return 'vendor_library_or_API_review_separately'
    if '/tests/' in lower or '/test/' in lower or Path(relative).name.startswith('test_'):
        return 'tests'
    if relative.startswith('fpga/'):
        return 'FPGA_application_or_support'
    return 'application_or_tool'

def scan(root):
    records = []
    for folder, directories, filenames in os.walk(root, followlinks=False):
        directories[:] = [x for x in directories if x not in SKIP]
        for name in filenames:
            path = Path(folder)/name
            if path.suffix.lower() not in SOURCE and name not in {'Makefile','makefile'}:
                continue
            data = path.read_bytes()
            if len(data) > 4*1024**2:
                continue
            try:
                text = data.decode('utf-8-sig')
            except UnicodeDecodeError:
                text = data.decode('gb18030',errors='replace')
            tokens = [x.group() for x in TOKEN.finditer(text) if not x.group().startswith(('//','/*'))]
            normalized = ' '.join(tokens)
            relative = path.relative_to(root).as_posix()
            records.append(dict(path=relative, bytes=len(data), lines=len(text.splitlines()),
                sha256=hashlib.sha256(data).hexdigest(), token_count=len(tokens),
                token_sha256=hashlib.sha256(normalized.encode()).hexdigest(),
                category=classify(relative), tokens=tokens))
    return records

def shingles(tokens, width=7):
    return {hashlib.blake2b('\x1f'.join(tokens[i:i+width]).encode(),digest_size=8).digest()
            for i in range(max(0,len(tokens)-width+1))}

def main():
    current, original = scan(ROOT), scan(ORIGIN)
    added_sources=[]
    if '--after' in sys.argv:
        before=json.loads((OUT/'audit_before.json').read_text(encoding='utf-8'))
        fixed_paths={item['path'] for item in before['files']}
        added_sources=[item['path'] for item in current if item['path'] not in fixed_paths]
        current=[item for item in current if item['path'] in fixed_paths]
        assert {item['path'] for item in current}==fixed_paths, 'Audit source set changed'
    by_hash = defaultdict(list)
    by_token = defaultdict(list)
    by_basename = defaultdict(list)
    by_relative = {x['path']:x for x in original}
    for item in original:
        by_hash[item['sha256']].append(item)
        by_token[item['token_sha256']].append(item)
        by_basename[Path(item['path']).name].append(item)
    shingle_cache = {}
    matches=[]
    for number,item in enumerate(current):
        record = {k:v for k,v in item.items() if k!='tokens'}
        exact=by_hash.get(item['sha256'],[])
        normalized=by_token.get(item['token_sha256'],[])
        record['exact_matches']=[x['path'] for x in exact]
        record['token_identical_matches']=[x['path'] for x in normalized]
        candidates = list(by_basename.get(Path(item['path']).name,[]))
        if item['path'] in by_relative and by_relative[item['path']] not in candidates:
            candidates.append(by_relative[item['path']])
        best=None
        own=shingles(item['tokens'])
        for other in candidates:
            if not own:
                continue
            if other['path'] not in shingle_cache:
                shingle_cache[other['path']]=shingles(other['tokens'])
            reference=shingle_cache[other['path']]
            intersection=len(own & reference)
            union=len(own | reference)
            score=dict(path=other['path'],jaccard=intersection/union if union else 0,
                       current_shingle_overlap=intersection/len(own),
                       origin_shingle_overlap=intersection/len(reference) if reference else 0)
            if best is None or score['current_shingle_overlap']>best['current_shingle_overlap']:
                best=score
        record['best_same_name_or_path_match']=best
        matches.append(record)
    exact=[x for x in matches if x['exact_matches']]
    normalized=[x for x in matches if x['token_identical_matches']]
    business=[x for x in matches if x['category'] in {'application_or_tool','FPGA_application_or_support'}]
    exact_business=[x for x in business if x['exact_matches']]
    summary=dict(current_source_files=len(current),origin_source_files=len(original),
        exact_file_matches=len(exact),exact_file_fraction=len(exact)/len(current),
        token_identical_file_matches=len(normalized),
        token_identical_file_fraction=len(normalized)/len(current),
        business_and_support_files=len(business),exact_business_file_matches=len(exact_business),
        exact_business_file_fraction=len(exact_business)/len(business),
        current_categories=dict(Counter(x['category'] for x in matches)),
        methods='SHA256 exact bytes; comments/whitespace-insensitive lexical tokens; same-name/path 7-token-shingle overlap',
        limits='Not an official plagiarism score or originality proof. Same-name/path partial matching may miss renamed/moved/split files. Standard APIs, IP and protocols naturally overlap.')
    result=dict(current_root=str(ROOT),origin_root=str(ORIGIN),summary=summary,files=matches,
                added_sources_not_used_to_dilute_comparison=added_sources)
    destination=OUT/('audit_after.json' if '--after' in sys.argv else 'audit_before.json')
    destination.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps(summary,ensure_ascii=False,indent=2))
    print('APPLICATION_MATCHES_OVER_200_TOKENS')
    ranking=sorted((x for x in matches if x['category']=='application_or_tool' and x['token_count']>=200 and x['best_same_name_or_path_match']),
                   key=lambda x:(-x['best_same_name_or_path_match']['current_shingle_overlap'],-x['token_count']))
    for item in ranking[:32]:
        best=item['best_same_name_or_path_match']
        print('overlap=%.3f tokens=%u exact=%s %s -> %s' % (
            best['current_shingle_overlap'],item['token_count'],bool(item['exact_matches']),item['path'],best['path']))

if __name__=='__main__':
    main()
