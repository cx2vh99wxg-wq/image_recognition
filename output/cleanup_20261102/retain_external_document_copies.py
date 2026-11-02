"""Keep document copies whose only proven keeper lies outside this workspace."""
import hashlib
import json
from pathlib import Path

out = Path(__file__).resolve().parent
path = out / 'cleanup_plan.json'
plan = json.loads(path.read_text(encoding='utf-8'))
root = Path(plan['root']).resolve()
retained = []
selected = []
for item in plan['files']:
    original = item.get('duplicate_of')
    if original and not Path(original).resolve().is_relative_to(root):
        retained.append(dict(path=item['path'], size=item['size'], sha256=item['sha256']))
    else:
        selected.append(item)
plan['files'] = selected
plan['preserved_sha256'].extend(retained)
plan['delete_file_count'] = len(selected)
plan['delete_bytes'] = sum(x['size'] for x in selected)
plan['extra_preserved_external_document_copies'] = [x['path'] for x in retained]
path.write_text(json.dumps(plan, ensure_ascii=False, indent=2), encoding='utf-8')
print('PLAN_REVISED files=%u remove_GiB=%.3f extra_document_copies_preserved=%u' % (
    len(selected), plan['delete_bytes']/2**30, len(retained)))
print('PLAN_SHA256', hashlib.sha256(path.read_bytes()).hexdigest())
