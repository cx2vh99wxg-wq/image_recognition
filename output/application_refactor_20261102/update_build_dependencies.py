"""Track transitive shared headers without changing compile optimization/options."""
import json
from pathlib import Path

OUT=Path(__file__).resolve().parent
ROOT=OUT.parents[1]
specs=[('udp_rk_rk_yolo/M/Makefile','$(SHARED_MEMORY_OBJ)'),
       ('udp_rk_rk_yolo/S/Makefile','$(SHARED_OBJECTS)'),
       ('udp_rk_rk_yolo/S/pcie/Makefile','$(OBJECTS)'),
       ('udp_rk_rk_yolo/S/fspi/Makefile','$(ALL_OBJECTS)')]
changed=[]
for name,objects in specs:
    path=ROOT/name
    text=path.read_text(encoding='utf-8')
    lines=[]
    for line in text.splitlines():
        if (line.startswith('CFLAGS = ') or line.startswith('CXXFLAGS = ')) and '-MMD' not in line:
            line+=' -MMD -MP'
        lines.append(line)
    dependency='-include '+objects.replace(')',':.o=.d)')
    assert dependency.count(':.o=.d')==1
    if dependency not in text:
        lines.extend(['','# Shared headers are authoritative; rebuild their consumers when they change.',dependency])
    path.write_text('\n'.join(lines)+'\n',encoding='utf-8')
    changed.append(name)
(OUT/'dependency_changes.json').write_text(json.dumps(changed,indent=2),encoding='utf-8')
print('TRANSITIVE_HEADER_DEPENDENCIES_ENABLED',len(changed))
