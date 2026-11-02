"""Internal source transport only; models, calibration and binaries stay put."""
import hashlib
import json
import os
from pathlib import Path
import tarfile

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
APP=ROOT/'udp_rk_rk_yolo'
skip={'build','logs','calibration','model','lib','__pycache__','tests','test'}
suffixes={'.c','.cpp','.h','.hpp','.py','.sh','.bash','.inc','.inl','.tcc','.S','.txt'}
manifest=[]
archive=OUT/'candidate_sources.tgz'
with tarfile.open(archive,'w:gz') as tar:
    for start in [APP/'M',APP/'S',APP/'common']:
        for folder,dirs,names in os.walk(start):
            dirs[:]=[name for name in dirs if name not in skip]
            for name in names:
                path=Path(folder)/name
                if path.suffix not in suffixes and name!='Makefile':continue
                relative=path.relative_to(APP).as_posix()
                tar.add(path,arcname=relative,recursive=False)
                manifest.append(dict(path=relative,sha256=hashlib.sha256(path.read_bytes()).hexdigest()))
(OUT/'candidate_source_manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2),encoding='utf-8')
print('SOURCE_TRANSPORT_READY files=%u bytes=%u sha256=%s'%(len(manifest),archive.stat().st_size,hashlib.sha256(archive.read_bytes()).hexdigest()))
