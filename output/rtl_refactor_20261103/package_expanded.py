"""Create the repository-relative RTL replacement archive and verify each member."""
from hashlib import sha256
import json
from zipfile import ZIP_DEFLATED, ZipFile

from build_pds import FPGA, HERE, ROOT
from inventory_expanded import main as inventory


def main():
    inventory()
    data = json.loads((HERE / 'expanded_source_inventory.json').read_text(encoding='utf-8'))
    entries = {
        (FPGA / path).relative_to(ROOT).as_posix(): item['current_sha256']
        for path, item in data['files'].items()
        if 'original_sha256' in item and item['original_sha256'] != item['current_sha256']
    }
    package = HERE / 'replacement_expanded.zip'
    with ZipFile(package, 'w', ZIP_DEFLATED) as archive:
        for path in entries:
            archive.write(ROOT / path, arcname=path)
    with ZipFile(package) as archive:
        assert set(archive.namelist()) == set(entries)
        for path, expected in entries.items():
            assert sha256(archive.read(path)).hexdigest() == expected, path
    print(f'Verified {len(entries)} RTL files in {package.name}')


if __name__ == '__main__':
    main()
