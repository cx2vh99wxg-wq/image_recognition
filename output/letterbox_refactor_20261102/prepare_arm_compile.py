"""Prepare isolated ARM compilation and read-only object-code comparison."""
from pathlib import Path

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
previous=ROOT/'output/application_refactor_20261102'
packager=(previous/'package_candidate_sources.py').read_text(encoding='utf-8')
(OUT/'package_candidate_sources.py').write_text(packager,encoding='utf-8')
builder=(previous/'compile_board_candidate.py').read_text(encoding='utf-8')
builder=builder.replace('refactor_candidate_20261102','refactor_letterbox_candidate_20261102')
builder=builder.replace('/tmp/application_refactor_sources_20261102.tgz','/tmp/letterbox_refactor_sources_20261102.tgz')
marker="(root/'compile_result.json').write_text(json.dumps(result,indent=2))"
assert builder.count(marker)==1
inspection='''# Read the ELF64 .text sections as data; never execute either object.
import struct
def section_digest(path, wanted):
    data=path.read_bytes()
    assert data[:6]==b'\\x7fELF\\x02\\x01'
    section_offset=struct.unpack_from('<Q',data,40)[0]
    entry_size,entry_count,names_index=struct.unpack_from('<HHH',data,58)
    def header(index):return struct.unpack_from('<IIQQQQIIQQ',data,section_offset+index*entry_size)
    names=header(names_index);strings=data[names[4]:names[4]+names[5]]
    for index in range(entry_count):
        item=header(index);end=strings.find(b'\\0',item[0])
        if strings[item[0]:end].decode()==wanted:
            assert item[5]>0, 'The compared machine-code section must not be empty'
            return hashlib.sha256(data[item[4]:item[4]+item[5]]).hexdigest()
    raise ValueError('ELF section not found: '+wanted)
if result['compiled']:
    old=Path('/home/linaro/refactor_candidate_20261102')/args.board/'pcie/build/preprocess.o'
    new=board/'pcie/build/preprocess.o'
    before_text=section_digest(old,'.text');after_text=section_digest(new,'.text')
    result['preprocess_object_text_before_sha256']=before_text
    result['preprocess_object_text_after_sha256']=after_text
    result['preprocess_object_text_identical']=before_text==after_text
    if before_text!=after_text:result['needs_investigation']=True
'''
builder=builder.replace(marker,inspection+'\n'+marker)
builder=builder.replace("raise SystemExit(0 if result['compiled'] else 1)","raise SystemExit(0 if result['compiled'] and result.get('preprocess_object_text_identical') else 1)")
(OUT/'compile_board_candidate.py').write_text(builder,encoding='utf-8')
print('ISOLATED_ARM_COMPILER_PREPARED no_application_execution no_production_overwrite')
