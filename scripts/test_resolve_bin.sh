#!/bin/bash
# Isolated launcher regression tests; fixture files stay under ignored tmp/.
set -eu
TASK_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
for script in start_m.sh start_s.sh stop_all.sh resolve_bin.sh test_resolve_bin.sh; do
    bash -n "$TASK_ROOT/scripts/$script"
done
bash "$TASK_ROOT/scripts/start_s.sh" --help >/dev/null
bash "$TASK_ROOT/scripts/start_m.sh" --help >/dev/null
mkdir -p "$TASK_ROOT/tmp"
FIXTURE="$(mktemp -d "$TASK_ROOT/tmp/resolve-bin.XXXXXX")"
REPO_ROOT="$FIXTURE"
BIN_DIR="$FIXTURE/bin"
NO_BUILD=1
mkdir -p "$BIN_DIR" "$REPO_ROOT/planning/csrc" "$REPO_ROOT/planning/include"
source "$TASK_ROOT/scripts/resolve_bin.sh"
make_fake() {
    printf '#!/bin/bash\necho "planning_main ADAS-6CH-v2 layout=960x480 M=top S=bottom"\n' > "$1"
    chmod +x "$1"
}
make_fake "$REPO_ROOT/planning/planning_main"
make_fake "$BIN_DIR/planning_main"
test "$(resolve_bin planning_main planning)" = "$REPO_ROOT/planning/planning_main"
# An older executable ignores --build-info: it must never be executed.
printf '#!/bin/bash\ntouch "%s/old-was-run"\nexit 0\n' "$FIXTURE" > "$REPO_ROOT/planning/planning_main"
test "$(resolve_bin planning_main planning)" = "$BIN_DIR/planning_main"
test ! -f "$FIXTURE/old-was-run"
cp "$REPO_ROOT/planning/planning_main" "$BIN_DIR/planning_main"
if resolve_bin planning_main planning; then echo "FAIL: incompatible binary accepted"; exit 1; fi
test ! -f "$FIXTURE/old-was-run"
# A source Makefile newer than the binary must force a rebuild.
make_fake "$REPO_ROOT/planning/planning_main"
touch -t 202001010000 "$REPO_ROOT/planning/planning_main"
touch "$REPO_ROOT/planning/Makefile"
if bin_fresh "$REPO_ROOT/planning/planning_main" planning; then echo "FAIL: ignored Makefile"; exit 1; fi
NO_BUILD=0
make() { return 1; }
if resolve_bin planning_main planning; then echo "FAIL: fallback after failed build"; exit 1; fi
echo "test_resolve_bin: PASS (module priority, old binary rejected without execution, Makefile dependency, fail closed)"
