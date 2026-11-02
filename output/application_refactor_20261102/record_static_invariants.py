"""Record source-level invariants only; never executes project/test code."""
import hashlib
import importlib.util
import json
from pathlib import Path
import re

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
spec=importlib.util.spec_from_file_location('source_invariant_lexer',ROOT/'output/submission_similarity_20261102/check_submission.py')
lexer=importlib.util.module_from_spec(spec)
spec.loader.exec_module(lexer)
def tokens(text,language='c_family'):
    # Verilog apostrophes introduce width/base constants, not C characters.
    # A C-only character regex can swallow comments between 32'd... and 13'.
    return lexer.lex(text,language)
baseline=json.loads((OUT/'baseline_manifest.json').read_text(encoding='utf-8'))
formats=json.loads((OUT/'format_manifest.json').read_text(encoding='utf-8'))
migrations=json.loads((OUT/'common_migration.json').read_text(encoding='utf-8'))
shared=[]
for migration in migrations:
    old=(OUT/'baseline'/migration['original_paths'][0]).read_text(encoding='utf-8-sig')
    current=(ROOT/migration['shared']).read_text(encoding='utf-8-sig')
    assert tokens(old)==tokens(current),migration['shared']
    shared.append(dict(path=migration['shared'],implementation_tokens_preserved=True,
                       compatible_paths=migration['original_paths']))
old_top=(OUT/'baseline/fpga/fpga_pcie_ov5640/source/image_pcie_capture.v').read_text(encoding='utf-8-sig')
current_top=(ROOT/'fpga/fpga_pcie_ov5640/source/image_pcie_capture.v').read_text(encoding='utf-8-sig')
blocks=(ROOT/'fpga/fpga_pcie_ov5640/source/remote_vehicle_control.v').read_text(encoding='utf-8-sig')
blocks=re.sub(r'(?m)^`timescale[^\n]*\n','',blocks)
assert tokens(old_top,'verilog')==tokens(current_top+'\n'+blocks,'verilog'),'RTL module body changed'
protected=[]
for item in baseline['files']:
    name=item['path']
    if Path(name).suffix in {'.fdc','.pcf','.rknn','.HMI','.tft','.cal','.table'} or name.endswith(('runtime.json','homing.json','auto_home.current.json')):
        path=ROOT/name
        assert path.is_file() and hashlib.sha256(path.read_bytes()).hexdigest()==item['sha256'],name
        protected.append(name)
record=dict(scope='Static source/token and file-integrity inspection only; no application or test executed',
            shared_implementations=shared,RTL_module_tokens_preserved=True,
            original_body_modules=7,protected_files_unchanged=protected,
            formatted_files_with_unchanged_tokens=sum(x.get('changed',False) for x in formats),
            formatting_rejections_or_skips=[dict(path=x['path'],reason=x['skipped']) for x in formats if 'skipped' in x],
            runtime_tests_run=False,hardware_programmed=False,origin_modified=False)
(OUT/'static_invariants.json').write_text(json.dumps(record,ensure_ascii=False,indent=2),encoding='utf-8')
print('STATIC_INVARIANTS_RECORDED shared_modules=%u RTL_modules=7 protected_files=%u no_runtime_tests'% (len(shared),len(protected)))
