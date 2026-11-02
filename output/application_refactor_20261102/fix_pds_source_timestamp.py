from pathlib import Path

OUT=Path(__file__).resolve().parent
FPGA=OUT.parents[1]/'fpga/fpga_pcie_ov5640'
for name in ['project/project.pds','project_m_imu/project_m_imu.pds',
    'project_m_imu_nack_20261101/project_m_imu.pds',
    'project_s_application_refactor_20261102/project_s_refactor.pds',
    'project_m_application_refactor_20261102/project_m_refactor.pds']:
    path=FPGA/name
    text=path.read_text(encoding='utf-8')
    old='(_file "../source/remote_vehicle_control.v"\n                    (_format verilog)\n                )'
    new='(_file "../source/remote_vehicle_control.v"\n                    (_format verilog)\n                    (_timespec "2026-11-02T00:00:00")\n                )'
    assert text.count(old)==1,name
    path.write_text(text.replace(old,new),encoding='utf-8')
print('PDS_SOURCE_TIMESTAMP_ADDED five project files')
