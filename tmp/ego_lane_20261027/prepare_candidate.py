import pathlib,shutil,sys,subprocess,json,hashlib
role=sys.argv[1];assert role in ('M','S')
root=pathlib.Path('/home/linaro')/role
work=root/'ego_lane_20261027';incoming=work/'incoming';candidate=work/'candidate'
assert not candidate.exists(),'Candidate already exists'
candidate.mkdir()
folders=['include','csrc','pcie/csrc','pcie/include','pcie/tests','pcie/tools','udp/csrc','udp/include','udp/tests'] if role=='M' else ['include','csrc','udp/csrc','udp/include','udp/tests','x11/csrc']
for name in folders:
    src=root/name
    if src.exists():shutil.copytree(str(src),str(candidate/name),symlinks=True,ignore=shutil.ignore_patterns('build','__pycache__'))
for name in (['Makefile','pcie/Makefile','udp/Makefile'] if role=='M' else ['Makefile','udp/Makefile']):
    dest=candidate/name;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(str(root/name),str(dest))
for name in ['lib','pcie/lib','pcie/model']:
    src=root/name;dest=candidate/name
    if src.exists() and not dest.exists():dest.parent.mkdir(parents=True,exist_ok=True);dest.symlink_to(src,target_is_directory=True)
files={}
for p in incoming.rglob('*'):
    if p.is_file():
        rel=p.relative_to(incoming);dest=candidate/rel;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(str(p),str(dest));files[str(rel)]=hashlib.sha256(dest.read_bytes()).hexdigest()
(work/'source_manifest.json').write_text(json.dumps(files,indent=2))
commands=[['make','-C','pcie','-j2','all','test-line-style','test-ego-lane','line-style-replay','line-style-model-probe'],['make','-C','udp','-j2']] if role=='M' else [['make','-C','udp','-j2'],['make','-C','x11/csrc','-j2']]
with (work/'build.log').open('w') as f:
    for command in commands:
        result=subprocess.run(command,cwd=str(candidate),stdout=f,stderr=subprocess.STDOUT)
        if result.returncode:print('BUILD_FAILED',role,result.returncode,flush=True);sys.exit(result.returncode)
print('CANDIDATE_BUILD_PASS',role,flush=True)
