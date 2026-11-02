"""Compile independent S/M firmware candidates; never connects/programs JTAG."""
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import time

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
PDS=Path('D:/PDS/Installation/PDS_2022.2-SP6.4/bin/pds_shell.exe')
CDT=PDS.with_name('cdt_cfg_shell.exe')
PUBLIC=ROOT/'fpga/fpga_pcie_ov5640/project/generate_bitstream'
projects=json.loads((OUT/'rtl_migration.json').read_text(encoding='utf-8'))['candidate_projects']
running=[];results=[]
for candidate in projects:
    board=candidate['board'];top=candidate['top_module'];project=Path(candidate['project'])
    tcl=OUT/('build_'+board+'.tcl')
    tcl.write_text('''open_project "{project}"
compile -force_to_run -top_module {top}
synthesize -ads -force_to_run -selected_syn_tool_opt 2
dev_map -force_to_run
pnr -force_to_run -gplace_seed 3 -groute_seed 3 -mode normal -optimize_multi_corner_timing -fix_hold_violation -slack_weight 90
report_timing -force_to_run
gen_bit_stream -force_to_run -compress_bitstream true -master_configuration_clock_frequency {{40M}}
'''.format(project=project.as_posix(),top=top),encoding='utf-8')
    log=(OUT/('build_'+board+'.log')).open('wb')
    error=(OUT/('build_'+board+'.err')).open('wb')
    process=subprocess.Popen([str(PDS),'-file',str(tcl)],cwd=project.parent,
        stdout=log,stderr=error,creationflags=getattr(subprocess,'CREATE_NO_WINDOW',0))
    running.append((candidate,process,log,error,time.time()))
    print('FPGA_CANDIDATE_BUILD_STARTED',board,process.pid,flush=True)
while running:
    pending=[]
    for candidate,process,log,error,started in running:
        if process.poll() is None:
            pending.append((candidate,process,log,error,started));continue
        log.close();error.close()
        board=candidate['board'];directory=Path(candidate['directory'])
        generated=directory/'generate_bitstream'/(candidate['top_module']+'.sbit')
        item=dict(candidate,returncode=process.returncode,elapsed_seconds=time.time()-started,
                  compiled=False,programmed=False,hardware_tested=False)
        if process.returncode==0 and generated.is_file() and generated.stat().st_size>1000000:
            destination=PUBLIC/('image_pcie_capture_'+board.lower()+'_application_refactor_20261102.sbit')
            assert not destination.exists(),'Never overwrite a delivered image'
            shutil.copy2(generated,destination)
            sfc=destination.with_suffix('.sfc')
            conversion=OUT/('convert_'+board+'.tcl')
            conversion.write_text('cfg_gen_sfc -sbit "'+destination.as_posix()+'" -sfc "'+sfc.as_posix()+'" -opcode 59\n',encoding='utf-8')
            with (OUT/('convert_'+board+'.log')).open('wb') as conversion_log:
                converted=subprocess.run([str(CDT),'-file',str(conversion)],cwd=directory,
                    stdout=conversion_log,stderr=subprocess.STDOUT,creationflags=getattr(subprocess,'CREATE_NO_WINDOW',0))
            item.update(compiled=True,sbit=str(destination),sbit_sha256=hashlib.sha256(destination.read_bytes()).hexdigest(),
                        conversion_returncode=converted.returncode)
            if converted.returncode==0 and sfc.is_file() and sfc.stat().st_size==destination.stat().st_size:
                item.update(sfc=str(sfc),sfc_sha256=hashlib.sha256(sfc.read_bytes()).hexdigest())
        results.append(item)
        (OUT/'firmware_candidates.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
        print('FPGA_CANDIDATE_BUILD_FINISHED',board,json.dumps(item,ensure_ascii=False),flush=True)
    running=pending
    if running:time.sleep(2)
if not all(item['compiled'] and item.get('sfc') for item in results):raise SystemExit(1)
print('S_M_CANDIDATE_FILES_CREATED_NO_PROGRAMMING_NO_HARDWARE_TEST',flush=True)
