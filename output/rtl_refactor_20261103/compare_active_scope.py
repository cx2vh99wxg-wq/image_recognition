"""Compare editable PDS RTL against the sibling repository."""
import argparse
import json
from pathlib import Path
import sys

from build_pds import FPGA, HERE, ROOT

sys.path.insert(0, str(HERE.parent / 'submission_similarity_20261102'))
import check_submission as audit


def measure(path, reference, index):
    raw = path.read_bytes()
    try:
        content = raw.decode('utf-8-sig')
    except UnicodeDecodeError:
        content = raw.decode('gb18030')
    words = audit.lex(content, 'verilog')
    ids = [audit.WORD_IDS.setdefault(word, len(audit.WORD_IDS) + 1) for word in words]
    covered, _ = audit.coverage({'ids': ids, 'language': 'verilog'}, reference, index)
    return len(ids), covered


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, default=HERE / 'active_scope_final.json')
    parser.add_argument('--before', type=Path, default=HERE / 'original')
    args = parser.parse_args()
    inventory = json.loads((HERE / 'expanded_source_inventory.json').read_text(encoding='utf-8'))
    audit.TOPS = ('fpga',)
    origin = ROOT.parent / 'image_recognition_origin'
    records, _, _, unreadable = audit.scan(origin)
    reference = audit.group([row for row in records if row['language'] == 'verilog' and audit.application(row)])
    index, _ = audit.build_index(reference)
    per_file = {}
    totals = {}
    for path, row in inventory['files'].items():
        if 'original_sha256' not in row:
            continue
        original = args.before / Path(path).relative_to('source')
        if not original.exists():
            original = FPGA / path
        per_file[path] = {
            'boards': row['boards'],
            'before': measure(original, reference, index),
            'after': measure(FPGA / path, reference, index),
        }
    for board in ('M', 'S', 'union'):
        scope = [file for file in per_file.values() if board == 'union' or board in file['boards']]
        totals[board] = {'files': len(scope)}
        for phase in ('before', 'after'):
            tokens = sum(file[phase][0] for file in scope)
            matched = sum(file[phase][1] for file in scope)
            totals[board][phase] = {
                'tokens': tokens, 'matched': matched, 'percent': round(100 * matched / tokens, 2),
            }
    result = {'minimum_match_tokens': audit.K, 'origin': str(origin),
              'unreadable': unreadable, 'totals': totals, 'per_file': per_file}
    args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(totals, indent=2))


if __name__ == '__main__':
    main()
