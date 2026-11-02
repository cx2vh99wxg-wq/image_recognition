"""Move unchanged remote-control modules out of the camera/PCIe top file."""
import hashlib
import json
from pathlib import Path
import re
import shutil

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
FPGA=ROOT/'fpga/fpga_pcie_ov5640'
source=FPGA/'source/image_pcie_capture.v'
raw=source.read_bytes()
baseline=(OUT/'baseline/fpga/fpga_pcie_ov5640/source/image_pcie_capture.v').read_bytes()
assert raw==baseline
text=raw.decode('utf-8').replace('\r\n','\n')
newline='\r\n' if b'\r\n' in raw else '\n'
first_end=re.search(r'(?m)^endmodule\s*$',text)
assert first_end is not None
top=text[:first_end.end()].rstrip()+'\n'
blocks=text[first_end.end():].lstrip('\n')
names=re.findall(r'(?m)^module\s+(\w+)',blocks)
assert names==['hmi_gear_feedback','remote_speed_controller','remote_direction_arbiter',
    'lane_permission_guard','auto_brake_enable_gate','remote_stop_controller','hmi_control_synchronizer'],names
destination=FPGA/'source/remote_vehicle_control.v'
assert not destination.exists()
destination.write_bytes(('// Shared vehicle-control blocks. Ports and module bodies are preserved.\n'+blocks).replace('\n',newline).encode('utf-8'))
source.write_bytes(top.replace('\n',newline).encode('utf-8'))
projects=['project/project.pds','project_m_imu/project_m_imu.pds',
          'project_m_imu_nack_20261101/project_m_imu.pds']
for name in projects:
    project=FPGA/name
    data=project.read_text(encoding='utf-8')
    expression=r'(?m)^([ \t]*)\(_file "\.\./source/image_pcie_capture\.v"[^\n]*\n\1[ \t]+\(_format verilog\)\n(?:\1[ \t]+\(_timespec "[^"]+"\)\n)?\1\)'
    match=re.search(expression,data)
    assert match is not None,name
    indent=match[1]
    addition='\n'+indent+'(_file "../source/remote_vehicle_control.v"\n'+indent+'    (_format verilog)\n'+indent+'    (_timespec "2026-11-02T00:00:00")\n'+indent+')'
    project.write_text(data[:match.end()]+addition+data[match.end():],encoding='utf-8')

# The main .pds files remain normal user entry points. Independent candidates
# isolate the new build outputs from the previously delivered bitstreams.
candidate_projects=[]
for board,original,project_file,top_module in [
    ('S','project','project.pds','image_pcie_capture'),
    ('M','project_m_imu_nack_20261101','project_m_imu.pds','image_pcie_capture_m_imu')]:
    old=FPGA/original
    new=FPGA/('project_'+board.lower()+'_application_refactor_20261102')
    assert not new.exists(),new
    new.mkdir()
    candidate=new/('project_'+board.lower()+'_refactor.pds')
    shutil.copy2(old/project_file,candidate)
    for path in old.glob('*.fdc'):shutil.copy2(path,new/path.name)
    for path in old.glob('*.pcf'):shutil.copy2(path,new/path.name)
    (new/'device_map').mkdir()
    for path in (old/'device_map').glob('*.pcf'):shutil.copy2(path,new/'device_map'/path.name)
    shutil.copytree(old/'ipcore',new/'ipcore')
    candidate_projects.append(dict(board=board,project=str(candidate),directory=str(new),top_module=top_module))
(OUT/'rtl_migration.json').write_text(json.dumps(dict(modules=names,original_module_source=str(source),shared_module_source=str(destination),bodies_preserved=True,original_sha256=hashlib.sha256(raw).hexdigest(),candidate_projects=candidate_projects),ensure_ascii=False,indent=2),encoding='utf-8')
print('RTL_ORGANIZED seven unchanged modules; three project inputs updated; independent S/M build projects prepared')
