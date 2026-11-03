"""Measure matching token coverage for selected RTL and the active PDS application."""
import json
import sys

from build_pds import FPGA, HERE, ROOT

sys.path.insert(0, str(HERE.parent / 'submission_similarity_20261102'))
import check_submission as audit

K = audit.K


def tokens(path):
    data = path.read_bytes()
    try:
        source = data.decode('utf-8-sig')
    except UnicodeDecodeError:
        source = data.decode('gb18030')
    return audit.lex(source, 'verilog')


def windows(values):
    for start in range(len(values) - K + 1):
        yield tuple(values[start:start + K])


def coverage(path, reference):
    values = tokens(path)
    covered = bytearray(len(values))
    for start, window in enumerate(windows(values)):
        if window in reference:
            covered[start:start + K] = b'\x01' * K
    return {'tokens': len(values), 'matched_tokens': sum(covered),
            'coverage_percent': round(100 * sum(covered) / len(values), 2)}


def active_application_coverage(origin_root, target_files):
    audit.TOPS = ('fpga',)
    current, _, _, current_unreadable = audit.scan(ROOT)
    origin, _, _, origin_unreadable = audit.scan(origin_root)
    manifest = json.loads((HERE / 'pds_inputs.json').read_text(encoding='utf-8'))
    prefix = FPGA.relative_to(ROOT)
    targets = {(prefix / 'source' / name).as_posix() for name in target_files}
    reference = audit.group([row for row in origin if audit.application(row)])
    index, _ = audit.build_index(reference)
    result = {}
    for board, entries in manifest.items():
        paths = {(prefix / row['path']).as_posix() for row in entries}
        app = [row for row in current if row['path'] in paths and audit.application(row)]
        total = sum(row['tokens'] for row in app)
        matches = {row['path']: audit.coverage(row, reference, index)[0] for row in app}
        matched = sum(matches.values())
        outside_targets = sum(count for path, count in matches.items() if path not in targets)
        result[board] = {
            'files': len(app), 'tokens': total, 'matched_tokens': matched,
            'coverage_percent': round(100 * matched / total, 2),
            'matched_tokens_outside_four_target_files': outside_targets,
            'outside_targets_share_of_all_tokens_percent': round(100 * outside_targets / total, 2),
        }
    return {'boards': result, 'unreadable': current_unreadable + origin_unreadable}


def main():
    origin_root = ROOT.parent / 'image_recognition_origin'
    origin = origin_root / FPGA.relative_to(ROOT)
    if not origin.is_dir():
        raise SystemExit(f'Missing comparison directory: {origin}')
    paths = json.loads((HERE / 'replacement_hashes.json').read_text(encoding='utf-8'))
    rows = {}
    for relative in paths:
        peer = origin / 'source' / relative
        if not peer.is_file():
            rows[relative] = {'origin_counterpart': None}
            continue
        reference = set(windows(tokens(peer)))
        rows[relative] = {
            'origin_counterpart': str(peer.relative_to(origin)),
            'before': coverage(HERE / 'original' / relative, reference),
            'after': coverage(FPGA / 'source' / relative, reference),
        }
    report = {'origin': str(origin), 'minimum_match_tokens': K, 'files': rows,
              'active_pds_application': active_application_coverage(origin_root, paths)}
    output = json.dumps(report, indent=2, ensure_ascii=False)
    (HERE / 'origin_target_files.json').write_text(output + '\n', encoding='utf-8')
    print(output)


if __name__ == '__main__':
    main()
