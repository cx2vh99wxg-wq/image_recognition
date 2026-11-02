"""Read-only, reproducible overlap metrics for the four submission directories.

No source upload, board access, application execution or code modification.
These metrics do not establish originality or an official contest score.
"""
from collections import Counter,defaultdict
import hashlib
import io
import json
import os
from pathlib import Path
import re
import sys
import tokenize

sys.stdout.reconfigure(encoding='utf-8')
OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
ORIGIN=ROOT.parent/'image_recognition_origin'
TOPS=('udp_rk_rk_yolo','fpga','HMI','common')
SKIP={'.git','.vscode','.claude','tmp','output','pds_work','build','__pycache__','logs','log','logbackup',
      'compile','synthesize','device_map','place_route','report_timing','report_power','constraint_check','generate_bitstream','generate_netlist'}
C_EXT={'.c','.cc','.cpp','.cxx','.h','.hh','.hpp','.inc','.inl','.tcc'}
V_EXT={'.v','.vh','.sv','.svh'}
SCRIPT_EXT={'.sh','.bash','.ps1','.tcl'}
SOURCE=C_EXT|V_EXT|SCRIPT_EXT|{'.py','.vp'}
K=40
MASK=(1<<64)-1
BASE=1000003
WORD_IDS={}
STRING=r'(?:u8|u|U|L)?"(?:\\[\s\S]|[^"\\])*"'
CHAR=r"(?:u8|u|U|L)?'(?:\\[^\n]|[^'\\\n])*'"
RAW=r'(?:u8|u|U|L)?R"(?P<delimiter>[^ ()\\\t\r\n]{0,16})\([\s\S]*?\)(?P=delimiter)"'
VNUMBER=r"(?:\d[\d_]*)?'[sS]?[bBoOdDhH][0-9a-fA-F_xXzZ?]+|'[01xXzZ]"
COMMON=r'[A-Za-z_$][A-Za-z0-9_$]*|(?:0[xX][0-9a-fA-F_]+|\d[\w.]*)|>>=|<<=|===|!==|::|->\*|->|\.\.\.|\+\+|--|&&|\|\||==|!=|<=|>=|<<|>>|\+=|-=|\*=|/=|%=|&=|\|=|\^=|\*\*|[^\s]'
C_LEX=re.compile(r'//[^\n]*|/\*[\s\S]*?\*/|'+RAW+'|'+STRING+'|'+CHAR+'|'+COMMON)
V_LEX=re.compile(r'//[^\n]*|/\*[\s\S]*?\*/|'+STRING+'|'+VNUMBER+'|'+COMMON)
S_LEX=re.compile(r'"(?:\\[\s\S]|[^"\\])*"|\'(?:[^\']|\'\')*\'|[A-Za-z_$][A-Za-z0-9_$]*|\d+|[^\s]')

def sha(data):return hashlib.sha256(data).hexdigest()
def role(path,text):
    lower=path.lower(); name=Path(path).name.lower()
    if '/ipcore/' in lower or '/source/pcie/' in lower or any(x in lower for x in ['/third_party/','/3rdparty/','/rknn_api','/rknn_matmul','/stb_','/librga/']):
        return 'vendor_IP_SDK_or_PCIe_support'
    if re.search(r'copyright[^\n]{0,150}(?:rockchip|pango|xilinx|altera|arm limited)|confidential[^\n]{0,100}rockchip',text[:8000],re.I):
        return 'vendor_IP_SDK_or_PCIe_support'
    if '/tests/' in lower or '/test/' in lower or name.startswith('test_'):
        return 'tests'
    if Path(path).suffix.lower()=='.inc' and any(x in name for x in ['font','asset','image']):
        return 'static_resource_source'
    return 'FPGA_application_and_board_support' if path.startswith('fpga/') else 'software_application_and_tools'

def lex(text,language):
    if language=='python':
        ignored={tokenize.COMMENT,tokenize.NL,tokenize.ENCODING,tokenize.ENDMARKER}
        values=[]
        try:
            for token in tokenize.generate_tokens(io.StringIO(text).readline):
                if token.type in ignored:continue
                if token.type in {tokenize.INDENT,tokenize.DEDENT,tokenize.NEWLINE}:
                    values.append('<'+tokenize.tok_name[token.type]+'>')
                else:values.append(token.string)
            return values
        except (tokenize.TokenError,IndentationError):
            return ['<PYTHON_LEX_FALLBACK>']+[x.group() for x in S_LEX.finditer(text)]
    text=re.sub(r'\\\r?\n','',text)
    if language=='script':
        # Leading hash comments are unambiguous; retain inline hashes because
        # shell parameter expansion and Make/Tcl quoting can give them meaning.
        text=re.sub(r'(?m)^[ \t]*#[^\n]*','',text)
        return [x.group() for x in S_LEX.finditer(text)]
    lexer=V_LEX if language=='verilog' else C_LEX
    return [x.group() for x in lexer.finditer(text) if not x.group().startswith(('//','/*'))]

def scan(root):
    records=[]; opaque=[]; resources=[]; cache={}; unreadable=[]
    for top in TOPS:
        base=root/top
        if not base.is_dir():continue
        for folder,dirs,names in os.walk(base,followlinks=False):
            dirs[:]=[name for name in dirs if name not in SKIP and not (Path(folder)/name).is_symlink()]
            for name in sorted(names):
                path=Path(folder)/name
                if path.is_symlink():continue
                extension=path.suffix.lower(); relative=path.relative_to(root).as_posix()
                if top=='HMI' and extension in {'.hmi','.tft'}:
                    raw=path.read_bytes();opaque.append(dict(path=relative,bytes=len(raw),sha256=sha(raw),kind='HMI_binary'));continue
                if extension not in SOURCE and name not in {'Makefile','makefile'}:continue
                raw=path.read_bytes();raw_hash=sha(raw)
                if extension=='.vp':
                    opaque.append(dict(path=relative,bytes=len(raw),sha256=raw_hash,kind='protected_vendor_IP'));continue
                try:text=raw.decode('utf-8-sig')
                except UnicodeDecodeError:
                    try:text=raw.decode('gb18030')
                    except UnicodeDecodeError:
                        unreadable.append(relative);continue
                category=role(relative,text)
                if category=='static_resource_source':
                    resources.append(dict(path=relative,bytes=len(raw),sha256=raw_hash));continue
                language='verilog' if extension in V_EXT else 'python' if extension=='.py' else 'script' if extension in SCRIPT_EXT or name.lower()=='makefile' else 'c_family'
                key=(language,raw_hash)
                if key not in cache:
                    words=lex(text,language)
                    normalized=sha(json.dumps([language,words],ensure_ascii=False,separators=(',',':')).encode())
                    ids=[]
                    for word in words:
                        if word not in WORD_IDS:WORD_IDS[word]=len(WORD_IDS)+1
                        ids.append(WORD_IDS[word])
                    cache[key]=(normalized,ids)
                normalized,ids=cache[key]
                records.append(dict(path=relative,bytes=len(raw),lines=len(text.splitlines()),sha256=raw_hash,
                    language=language,category=category,token_sha256=normalized,tokens=len(ids),ids=ids))
    return records,opaque,resources,unreadable

def group(records):
    mapping={}; groups=[]
    for record in records:
        key=record['token_sha256']
        if key not in mapping:
            mapping[key]=len(groups);groups.append(dict(ids=record['ids'],language=record['language'],members=[]))
        record['group']=mapping[key];groups[record['group']]['members'].append(record)
    return groups

def rolling(ids):
    if len(ids)<K:return
    power=pow(BASE,K-1,1<<64);value=0
    for item in ids[:K]:value=((value*BASE)+item)&MASK
    yield 0,value
    for position in range(1,len(ids)-K+1):
        value=(((value-ids[position-1]*power)&MASK)*BASE+ids[position+K-1])&MASK
        yield position,value

APP_ROLES={'software_application_and_tools','FPGA_application_and_board_support'}
def application(record):return record['category'] in APP_ROLES
def build_index(groups,app_only=False,dual=False):
    index=defaultdict(dict);second=defaultdict(dict)
    for number,entry in enumerate(groups):
        if app_only and not any(application(x) for x in entry['members']):continue
        table=index[entry['language']];alternate=second[entry['language']]
        for pos,key in rolling(entry['ids']):
            existing=table.get(key)
            if existing is None:table[key]=(number<<32)|pos
            elif dual and existing>>32!=number and key not in alternate:alternate[key]=(number<<32)|pos
    return index,second

def coverage(entry,reference,index,second=None,self_group=None):
    ids=entry['ids'];table=index.get(entry['language'],{});alternates=(second or {}).get(entry['language'],{})
    covered=0;end=-1;contributors=Counter()
    for pos,key in rolling(ids):
        encoded=table.get(key)
        if encoded is None:continue
        if self_group is not None and encoded>>32==self_group:
            encoded=alternates.get(key)
            if encoded is None:continue
        owner=encoded>>32;offset=encoded&0xffffffff
        # A hash hit is never accepted without the actual token sequence.
        if ids[pos:pos+K]!=reference[owner]['ids'][offset:offset+K]:continue
        added=K if end<pos else max(0,pos+K-end)
        covered+=added;end=max(end,pos+K)
        if added:contributors[owner]+=added
    return covered,[(reference[number]['members'][0]['path'],count) for number,count in contributors.most_common(5)]

def exact_stats(records,reference):
    ref_raw={x['sha256'] for x in reference};ref_norm={x['token_sha256'] for x in reference}
    raw=Counter(x['sha256'] for x in records);normal=Counter(x['token_sha256'] for x in records)
    total_tokens=sum(x['tokens'] for x in records)
    first={x['token_sha256']:x['tokens'] for x in records}
    exact=sum(x['sha256'] in ref_raw for x in records)
    norm=sum(x['token_sha256'] in ref_norm for x in records)
    return dict(files=len(records),tokens=total_tokens,bytes=sum(x['bytes'] for x in records),
        exact_external_files=exact,exact_external_file_percent=100*exact/len(records) if records else 0,
        normalized_external_files=norm,normalized_external_file_percent=100*norm/len(records) if records else 0,
        internal_exact_extra_copies=sum(n-1 for n in raw.values()),
        internal_normalized_extra_copies=sum(n-1 for n in normal.values()),
        internal_normalized_extra_token_percent=100*sum((n-1)*first[key] for key,n in normal.items())/total_tokens if total_tokens else 0,
        unique_normalized_contents=len(normal))

def main():
    print('READ_ONLY_SCAN_STARTED',flush=True)
    current,opaque,resources,unreadable=scan(ROOT)
    original,origin_opaque,origin_resources,origin_unreadable=scan(ORIGIN)
    cg=group(current);og=group(original)
    print('SCANNED current_files=%u origin_files=%u unique_contents=%u/%u'%(len(current),len(original),len(cg),len(og)),flush=True)
    external,_=build_index(og);app_external,_=build_index(og,app_only=True)
    print('REFERENCE_FRAGMENT_INDEX_BUILT',flush=True)
    internal,internal_second=build_index(cg,dual=True)
    app_internal,app_internal_second=build_index(cg,app_only=True,dual=True)
    print('INTERNAL_FRAGMENT_INDEX_BUILT',flush=True)
    for number,entry in enumerate(cg):
        entry['external_covered'],entry['external_contributors']=coverage(entry,og,external)
        entry['internal_covered']=len(entry['ids']) if len(entry['members'])>1 else coverage(entry,cg,internal,internal_second,number)[0]
        app_members=[x for x in entry['members'] if application(x)]
        if app_members:
            entry['app_external_covered'],entry['app_external_contributors']=coverage(entry,og,app_external)
            entry['app_internal_covered']=len(entry['ids']) if len(app_members)>1 else coverage(entry,cg,app_internal,app_internal_second,number)[0]
        if (number+1)%100==0:print('FRAGMENT_COVERAGE %u/%u'%(number+1,len(cg)),flush=True)
    subsets={
        'all_text_source':current,
        'application_excluding_vendor_and_tests':[x for x in current if application(x)],
        'software_application_all':[x for x in current if x['category']=='software_application_and_tools'],
        'M_software_application':[x for x in current if application(x) and x['path'].startswith('udp_rk_rk_yolo/M/')],
        'S_software_application':[x for x in current if application(x) and x['path'].startswith('udp_rk_rk_yolo/S/')],
        'shared_application':[x for x in current if application(x) and x['path'].startswith('udp_rk_rk_yolo/common/')],
        'FPGA_application_and_board_support':[x for x in current if x['category']=='FPGA_application_and_board_support'],
        'vendor_IP_SDK_or_PCIe_support':[x for x in current if x['category']=='vendor_IP_SDK_or_PCIe_support'],
        'tests':[x for x in current if x['category']=='tests']}
    metrics={}
    for name,records in subsets.items():
        app=name not in {'all_text_source','vendor_IP_SDK_or_PCIe_support','tests'}
        refs=[x for x in original if application(x)] if app else original
        item=exact_stats(records,refs)
        prefix='app_' if app else ''
        total=item['tokens']
        external_tokens=sum(cg[x['group']][prefix+'external_covered'] for x in records)
        internal_tokens=sum(cg[x['group']][prefix+'internal_covered'] for x in records)
        item.update(external_fragment_covered_tokens=external_tokens,
                    external_fragment_token_percent=100*external_tokens/total if total else 0,
                    internal_cross_file_fragment_token_percent=100*internal_tokens/total if total else 0)
        unique={x['group'] for x in records}
        unique_tokens=sum(len(cg[g]['ids']) for g in unique)
        item['content_deduplicated_external_fragment_percent']=100*sum(cg[g][prefix+'external_covered'] for g in unique)/unique_tokens if unique_tokens else 0
        metrics[name]=item
    original_raw=defaultdict(list);original_normal=defaultdict(list)
    for item in original:
        original_raw[item['sha256']].append(item['path']);original_normal[item['token_sha256']].append(item['path'])
    details=[]
    for item in current:
        entry=cg[item['group']];app=application(item);prefix='app_' if app else ''
        row={key:value for key,value in item.items() if key not in {'ids','group'}}
        count=entry[prefix+'external_covered']
        row.update(exact_matches=original_raw[item['sha256']],normalized_matches=original_normal[item['token_sha256']],
                   fragment_covered_tokens=count,fragment_percent=100*count/item['tokens'] if item['tokens'] else 0,
                   representative_origin_contributors=entry[prefix+'external_contributors'])
        details.append(row)
    opaque_hashes=defaultdict(list)
    for item in origin_opaque:opaque_hashes[item['sha256']].append(item['path'])
    for item in opaque:item['exact_origin_matches']=opaque_hashes[item['sha256']]
    groups=[dict(copies=len(entry['members']),tokens=len(entry['ids']),files=[x['path'] for x in entry['members']]) for entry in cg if len(entry['members'])>1]
    result=dict(date='2026-11-02',current_root=str(ROOT),origin_root=str(ORIGIN),scope=list(TOPS),
        method=dict(fragment_minimum_tokens=K,whole_file_normalization='Language-aware lexical tokens; whitespace/comments ignored, identifiers and literals retained; Python indentation structure retained',
                    fragment_matching='All-file same-language rolling-hash lookup with exact token verification; merged union coverage; moved/split files included',
                    coverage_direction='Fraction of CURRENT tokens covered by exact >=40-token fragments anywhere in the stated reference corpus',
                    internal_definition='Cross-file repetition, including legitimate reuse; not a judgement of copying'),
        metrics=metrics,files=details,internal_duplicate_groups=groups,opaque_artifacts=opaque,static_resources=resources,
        unreadable_sources=unreadable,origin_unreadable_sources=origin_unreadable,
        limits=['Not an official contest score or internet/database check','No identifier-renaming/AST/semantic equivalence detection','Path-based vendor classification requires manual provenance review',
                'Protected IP and HMI binaries are hash-compared only; HMI event scripts not extracted','Primary full-source counts include multiple physical IP copies in candidate projects','Static image/font resource data excluded from executable-code metrics'],
        code_modified=False,boards_accessed=False,uploaded_to_third_party=False)
    (OUT/'report.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    print('FINAL_METRICS '+json.dumps(metrics,ensure_ascii=False),flush=True)
    print('OPAQUE_ARTIFACTS %u STATIC_RESOURCES %u UNREADABLE %u'%(len(opaque),len(resources),len(unreadable)),flush=True)

if __name__=='__main__':main()
