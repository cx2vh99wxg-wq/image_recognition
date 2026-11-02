"""Static compiler comparison only; does not run a project or test program."""
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import subprocess

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
BUILD=OUT/'compiler_inspection';BUILD.mkdir(exist_ok=True)
compiler=Path('D:/zheliVScode/mingw64/bin/gcc.exe')
spec=importlib.util.spec_from_file_location('source_audit_lexer',ROOT/'output/submission_similarity_20261102/check_submission.py')
lexer=importlib.util.module_from_spec(spec);spec.loader.exec_module(lexer)
results=[]
environment=dict(os.environ,TMP=str(BUILD),TEMP=str(BUILD),TMPDIR=str(BUILD))
for board,standard in [('M','c99'),('S','c11')]:
    relative='udp_rk_rk_yolo/'+board+'/pcie/csrc/preprocess.c'
    streams=[]
    for version,path in [('before',OUT/'before'/relative),('after',ROOT/relative)]:
        output=BUILD/(board+'_'+version+'.i')
        command=[str(compiler),'-std='+standard,'-E','-P','-I'+str(ROOT/'udp_rk_rk_yolo'/board/'pcie/include'),str(path),'-o',str(output)]
        process=subprocess.run(command,cwd=ROOT,capture_output=True,text=True,env=environment)
        assert process.returncode==0,(board,version,process.stderr)
        streams.append(lexer.lex(output.read_text(encoding='utf-8',errors='strict'),'c_family'))
    assert streams[0]==streams[1],board+' preprocessed implementation differs'
    results.append(dict(board=board,compiler_preprocessed_tokens_equal=True,
                        token_count=len(streams[0]),token_sha256=hashlib.sha256(json.dumps(streams[0],ensure_ascii=False).encode()).hexdigest()))
relative='udp_rk_rk_yolo/S/fspi/csrc/main.c'
old=lexer.lex((OUT/'before'/relative).read_text(encoding='utf-8'),'c_family')
new=lexer.lex((ROOT/relative).read_text(encoding='utf-8'),'c_family')
assert old==new,'Control source changed beyond comments'
protected=[]
for item in json.loads((OUT/'before.json').read_text(encoding='utf-8')):
    if item.get('protected'):
        assert hashlib.sha256((ROOT/item['path']).read_bytes()).hexdigest()==item['sha256'],item['path']
        protected.append(item['path'])
record=dict(preprocessed_source_inspections=results,control_noncomment_tokens_unchanged=True,
            protected_files_unchanged=protected,runtime_tests_run=False,actuator_actions=False)
(OUT/'static_inspection.json').write_text(json.dumps(record,ensure_ascii=False,indent=2),encoding='utf-8')
print('STATIC_COMPILER_INSPECTION_OK M_and_S_implementations_identical public_headers_and_control_parameters_unchanged')
