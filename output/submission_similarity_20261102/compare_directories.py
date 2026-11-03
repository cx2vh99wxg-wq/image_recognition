"""Compare matching subtrees and locate long, verified token matches."""
from collections import defaultdict
import json
from pathlib import Path
import re

import check_submission as audit

MIN_BLOCK = 200
STRIDE = 100


def locations(root, record, start):
    if record['language'] not in {'c_family', 'verilog'}:
        return None
    data = (root / record['path']).read_bytes()
    try:
        source = data.decode('utf-8-sig')
    except UnicodeDecodeError:
        source = data.decode('gb18030')
    if re.search(r'\\\r?\n', source):
        return None
    lexer = audit.V_LEX if record['language'] == 'verilog' else audit.C_LEX
    for index, match in enumerate(m for m in lexer.finditer(source)
                                  if not m.group().startswith(('//', '/*'))):
        if index == start:
            return source.count('\n', 0, match.start()) + 1
    return None


def long_matches(groups, reference):
    exact = {(g['language'], g['members'][0]['token_sha256']): i
             for i, g in enumerate(reference)}
    anchors = {}
    needed = defaultdict(set)
    direct = {}
    for i, group in enumerate(groups):
        key = group['language'], group['members'][0]['token_sha256']
        if key in exact:
            direct[i] = exact[key]
        elif len(group['ids']) >= MIN_BLOCK:
            anchors[i] = [(p, h) for p, h in audit.rolling(group['ids']) if p % STRIDE == 0]
            needed[group['language']].update(h for _, h in anchors[i])
    occurrences = defaultdict(list)
    for j, group in enumerate(reference):
        language = group['language']
        if language in needed:
            for q, h in audit.rolling(group['ids']):
                if h in needed[language]:
                    occurrences[language, h].append((j, q))

    for i, group in enumerate(groups):
        left = group['ids']
        if i in direct and len(left) >= MIN_BLOCK:
            yield i, [(0, len(left), (len(left), direct[i], 0, 0))]
            continue
        spans = []
        checked = {}
        for p, h in anchors.get(i, []):
            for j, q in occurrences[group['language'], h]:
                diagonal = j, q - p
                if checked.get(diagonal, -1) >= p + audit.K:
                    continue
                right = reference[j]['ids']
                if left[p:p+audit.K] != right[q:q+audit.K]:
                    continue
                a, b = p, q
                while a and b and left[a-1] == right[b-1]:
                    a -= 1
                    b -= 1
                end, other_end = p + audit.K, q + audit.K
                while end < len(left) and other_end < len(right) and left[end] == right[other_end]:
                    end += 1
                    other_end += 1
                checked[diagonal] = end
                if end-a >= MIN_BLOCK:
                    spans.append((a, end, (end-a, j, a, b)))
        regions = []
        for start, end, witness in sorted(spans):
            if regions and start < regions[-1][1]:
                lo, hi, best = regions[-1]
                regions[-1] = (lo, max(hi, end), max(best, witness))
            else:
                regions.append((start, end, witness))
        yield i, regions


def compare(current, origin):
    groups, reference = audit.group(current), audit.group(origin)
    index, _ = audit.build_index(reference)
    counts = audit.exact_stats(current, origin)
    covered = [audit.coverage(g, reference, index)[0] for g in groups]
    counts['covered_tokens'] = sum(n*len(g['members']) for n, g in zip(covered, groups))
    counts['coverage_percent'] = 100 * counts['covered_tokens'] / counts['tokens']
    counts['deduplicated_coverage_percent'] = 100 * sum(covered) / sum(len(g['ids']) for g in groups)
    del index
    regions = list(long_matches(groups, reference))
    counts['large_regions'] = sum(len(spans)*len(groups[i]['members']) for i, spans in regions)
    counts['large_regions_deduplicated'] = sum(len(spans) for _, spans in regions)
    counts['regions_with_500_token_match'] = sum(
        sum(witness[0] >= 500 for _, _, witness in spans)*len(groups[i]['members'])
        for i, spans in regions)
    counts['large_region_files'] = sum(len(groups[i]['members']) for i, spans in regions if spans)
    counts['large_region_tokens'] = sum(sum(end-start for start, end, _ in spans)*len(groups[i]['members'])
                                        for i, spans in regions)
    details = []
    for i, spans in regions:
        g = groups[i]
        record = g['members'][0]
        for start, end, (length, j, a, b) in spans:
            candidates = reference[j]['members']
            other = next((r for r in candidates if r['path'] == record['path']), candidates[0])
            details.append(dict(current=record['path'], origin=other['path'],
                                region_tokens=end-start, longest_match_tokens=length,
                                current_start_token=a, origin_start_token=b,
                                current_copies=[r['path'] for r in g['members']]))
    details.sort(key=lambda row: -row['longest_match_tokens'])
    for row in details[:15]:
        for side, root, records in [('current', audit.ROOT, current), ('origin', audit.ORIGIN, origin)]:
            record = next(r for r in records if r['path'] == row[side])
            row[side+'_line'] = locations(root, record, row[side+'_start_token'])
    return counts, details


def main():
    audit.TOPS = ('udp_rk_rk_yolo', 'fpga')
    for root in (audit.ROOT, audit.ORIGIN):
        for name in audit.TOPS:
            assert (root/name).is_dir(), root/name
    current, opaque, _, unreadable = audit.scan(audit.ROOT)
    origin, origin_opaque, _, origin_unreadable = audit.scan(audit.ORIGIN)
    result = {'minimum_match_tokens': audit.K, 'large_block_minimum_tokens': MIN_BLOCK,
              'large_region_counting': 'Merge overlapping long matches within each current file',
              'unreadable': unreadable + origin_unreadable, 'scopes': {}}
    for name, prefix, application_only in [('software', 'udp_rk_rk_yolo/', False),
                                            ('software_application', 'udp_rk_rk_yolo/', True),
                                            ('fpga', 'fpga/', False),
                                            ('fpga_application', 'fpga/', True)]:
        def select(records):
            return [r for r in records if r['path'].startswith(prefix)
                    and (not application_only or audit.application(r))]
        print('Comparing', name, flush=True)
        metrics, blocks = compare(select(current), select(origin))
        result['scopes'][name] = dict(metrics=metrics, blocks=blocks)
        print(json.dumps(dict(scope=name, metrics=metrics, top=blocks[:5]), ensure_ascii=False), flush=True)
    opaque_origin = {r['sha256'] for r in origin_opaque}
    result['protected_ip'] = {'files': len(opaque),
                              'exact_files': sum(r['sha256'] in opaque_origin for r in opaque)}
    (audit.OUT/'directory_comparison_20261103.json').write_text(
        json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')


if __name__ == '__main__':
    main()
