set -eu
export LC_ALL=C
stage=/home/linaro/adas_hmi_autopilot_20261113
mkdir -p "$stage/source/S"
cp -a /home/linaro/S/fspi /home/linaro/S/include /home/linaro/S/csrc /home/linaro/S/steering "$stage/source/S/"
if [ ! -e "$stage/source/common" ]; then
  ln -s /home/linaro/common "$stage/source/common"
fi
test -r "$stage/source/common/steering/contracts/steering_types.h"
python3 -c 'import json, pathlib, sys, zlib; d=pathlib.Path(sys.argv[1]); files=json.loads(zlib.decompress((d/"serial_source_stage.zlib").read_bytes())); root=d/"source/S"; [(root/p).write_text(data,encoding="utf-8") for p,data in files.items()]' "$stage"
cd "$stage/source/S/fspi"
if ! make -j2 BUILDDIR=hmi_build all test-control-status test-speed >"$stage/compile.log" 2>&1; then tail -30 "$stage/compile.log"; exit 2; fi
grep PASS "$stage/compile.log"
file hmi_build/vehicle_control
sha256sum hmi_build/vehicle_control
ps -C vehicle_control -o args=