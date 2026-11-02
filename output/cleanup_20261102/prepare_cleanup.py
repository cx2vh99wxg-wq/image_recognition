"""Prepare an explicit derived-file cleanup plan and hash every retained file."""
from collections import Counter, defaultdict
import hashlib
import json
from pathlib import Path
import re
import subprocess

OUT = Path(__file__).resolve().parent
ROOT = OUT.parents[1]
inventory = json.loads((OUT / 'inventory_before.json').read_text(encoding='utf-8'))
assert Path(inventory['root']).resolve() == ROOT.resolve()
assert not inventory['links'] and not inventory['errors']
files = inventory['files']
by_path = {x['path']: x for x in files}
plan = {}

def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()

def add(item, reason, duplicate_of=None):
    relative = item['path']
    assert not relative.startswith(('.git/', 'output/', 'HMI/', 'udp_rk_rk_yolo/', 'common/', 'fspi/'))
    assert '/source/' not in relative and '/ipcore/' not in relative
    assert not relative.startswith('fpga/fpga_pcie_ov5640/project/generate_bitstream/')
    value = dict(item, reason=reason, absolute=str((ROOT / relative).resolve()),
                 mtime_ticks=item['mtime_ns']//100 + 621355968000000000)
    if duplicate_of:
        value['duplicate_of'] = str(duplicate_of)
        value['sha256'] = digest(ROOT / relative)
    plan[relative] = value

# Verilator output directories are recognized by their generated makefiles,
# rather than assuming any directory named build contains disposable files.
sim_roots = set()
for item in files:
    path = Path(item['path'])
    if item['path'].startswith('tmp/') and path.name.startswith('V') and path.suffix == '.mk':
        sim_roots.add(path.parent.as_posix())

derived_suffixes = {'.gch', '.o', '.whl', '.adf', '.netlist', '.vm', '.db', '.sdf', '.smsk', '.rtr'}
generated_document_roots = ['tmp/paper_section/', 'tmp/docx_indent_20261030/',
    'tmp/docx_integration_20261030/', 'tmp/paper_template_render/', 'tmp/template_reference/']
project_roots = ['fpga/fpga_pcie_ov5640/project/',
    'fpga/fpga_pcie_ov5640/project_m_imu_nack_20261101/']
generated_stages = {'compile','synthesize','place_route','report_timing','report_power',
                    'generate_netlist','log','logbackup'}
constraint_suffixes = {'.pcf', '.fdc', '.sdc', '.xdc', '.pdc', '.lpf'}

for item in files:
    relative = item['path']
    path = Path(relative)
    suffix = path.suffix.lower()
    if relative.startswith('tmp/'):
        if any(relative.startswith(prefix + '/') for prefix in sim_roots):
            add(item, 'Verilator generated build cache')
        elif suffix in derived_suffixes:
            add(item, 'Temporary compiler/PDS cache or downloaded wheel')
        elif any(relative.startswith(prefix) for prefix in generated_document_roots) and suffix in {'.png','.jpg','.jpeg','.ppm','.pdf'}:
            add(item, 'Temporary document render or preview')
        elif '__pycache__' in path.parts or suffix in {'.pyc','.pyo'}:
            add(item, 'Python bytecode cache')
    elif relative.startswith('pds_work/') and suffix not in constraint_suffixes:
        if any(part in generated_stages | {'device_map','generate_bitstream'} for part in path.parts[2:]) and suffix not in {'.sbit','.sfc','.pds','.tcl','.txt','.csv','.json'}:
            add(item, 'Historical PDS experiment build cache')
        elif suffix == '.log':
            add(item, 'Historical PDS experiment log')
    elif any(relative.startswith(prefix) for prefix in project_roots) and '/ipcore/' not in relative:
        prefix = next(prefix for prefix in project_roots if relative.startswith(prefix))
        rest = relative[len(prefix):]
        stage = rest.split('/')[0]
        if stage in generated_stages and suffix not in constraint_suffixes:
            if suffix not in {'.tcl','.pds','.json','.csv'}:
                add(item, 'Regenerable PDS build output or log')
        elif stage == 'device_map' and suffix in derived_suffixes | {'.log','.dmr','.prt','.pvf'}:
            add(item, 'Regenerable device mapping database; placement constraints retained')
        elif '/' not in rest and path.name in {'flow.log','pds.log','run.log','stderr.log','stdout.log'}:
            add(item, 'PDS top-level log')
    elif path.parts[0] in {'log','logbackup'} and suffix == '.log':
        add(item, 'Root PDS log')
    elif len(path.parts) == 1 and suffix == '.log':
        add(item, 'Root PDS log')

# Remove only byte-identical temporary document/firmware copies; unique source,
# model, document and hardware artifacts are not inferred disposable by name.
keepers = defaultdict(list)
for item in files:
    if item['path'] in plan or item['path'].startswith(('.git/','tmp/')):
        continue
    if Path(item['path']).suffix.lower() in {'.docx','.hmi','.tft','.sbit','.sfc'}:
        keepers[item['size']].append(ROOT / item['path'])
hash_cache = {}
for item in files:
    if item['path'] in plan or not item['path'].startswith('tmp/'):
        continue
    if Path(item['path']).suffix.lower() not in {'.docx','.hmi','.tft','.sbit','.sfc'}:
        continue
    choices = keepers.get(item['size'], [])
    if not choices:
        continue
    source_hash = digest(ROOT / item['path'])
    for keeper in choices:
        if keeper not in hash_cache:
            hash_cache[keeper] = digest(keeper)
        if hash_cache[keeper] == source_hash:
            add(item, 'Byte-identical temporary copy; authoritative file retained', keeper)
            break

# Capture only project setup inputs, not each PDS task's regenerated outputs.
dependencies = []
for name in ['project/project.pds','project_m_imu/project_m_imu.pds',
             'project_m_imu_nack_20261101/project_m_imu.pds']:
    project = ROOT / 'fpga/fpga_pcie_ov5640' / name
    setup = project.read_text(encoding='utf-8', errors='replace').split('(_task tsk_compile')[0]
    for ref in re.findall(r'\(_file\s+"([^"]+)"', setup):
        candidate = (project.parent / ref).resolve()
        assert candidate.is_relative_to(ROOT.resolve()), (name, ref)
        relative = candidate.relative_to(ROOT).as_posix()
        assert relative not in plan, ('Attempt to delete PDS input', relative)
        dependencies.append(dict(project=name, reference=ref, path=relative, exists=candidate.is_file()))
    # Explicit seed constraint used by prepare_project.ps1 is a required input.
for item in files:
    if Path(item['path']).suffix.lower() in constraint_suffixes:
        assert item['path'] not in plan

retained = []
for item in files:
    relative = item['path']
    if relative in plan or relative.startswith(('.git/', 'output/cleanup_20261102/')):
        continue
    retained.append(dict(path=relative, size=item['size'], sha256=digest(ROOT / relative)))
branch = subprocess.check_output(['git','branch','--show-current'], cwd=ROOT, text=True).strip()
head = subprocess.check_output(['git','rev-parse','HEAD'], cwd=ROOT, text=True).strip()
result = dict(root=str(ROOT), branch=branch, head=head, boards_modified=False,
              delete_file_count=len(plan), delete_bytes=sum(x['size'] for x in plan.values()),
              files=sorted(plan.values(), key=lambda x:x['path']),
              preserved_sha256=retained, PDS_setup_dependencies=dependencies,
              missing_PDS_inputs_before=[x for x in dependencies if not x['exists']])
(OUT/'cleanup_plan.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
summary = Counter()
for item in plan.values():
    summary[item['reason']] += item['size']
print('PLAN_ONLY current_branch=%s files=%u remove_GiB=%.3f retained_hashes=%u PDS_inputs=%u preexisting_missing_inputs=%u' % (
    branch, len(plan), result['delete_bytes']/2**30, len(retained), len(dependencies), len(result['missing_PDS_inputs_before'])))
for reason, size in summary.most_common():
    print('%.2f MiB %s' % (size/2**20, reason))
print('PLAN_SHA256', digest(OUT/'cleanup_plan.json'))
