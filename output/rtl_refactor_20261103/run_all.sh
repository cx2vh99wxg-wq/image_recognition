#!/usr/bin/env bash
set -eu
runner="$(dirname "$0")/run_sim.sh"
status=0
bash "$runner" baseline || status=1
bash "$runner" revised &
first=$!
bash "$runner" compare &
second=$!
wait "$first" || status=1
wait "$second" || status=1
exit "$status"
