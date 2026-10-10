#!/usr/bin/env python3
"""Offline manifest/pin consistency. Optional pyslang elaboration excludes vendor IP.
This does NOT replace PDS synthesis, resource usage, timing or electrical checks.
"""
from pathlib import Path
import argparse,re,collections,sys
ROOT=Path(__file__).resolve().parents[1];B=ROOT/'fpga/fpga_pcie_ov5640';D=B/'drive_6ch'
p=argparse.ArgumentParser();p.add_argument('--pyslang-path');a=p.parse_args()
tcl=(D/'add_sources.tcl').read_text(encoding='utf-8')
rel=re.findall(r'^\s+\{([^{}]+\.(?:v|idf))\}',tcl,re.M)
for r in rel:assert (D/r).is_file(),r
fdc=(D/'drive_6ch.fdc').read_text(encoding='utf-8')
pins=re.findall(r'define_attribute \{p:([^}]+)\} \{PAP_IO_LOC\} \{([^}]+)\}',fdc)
balls=collections.defaultdict(list)
for port,ball in pins:balls[ball].append(port)
assert all(len(p)==1 for p in balls.values()),'duplicate FPGA ball'
required={'free_clk','board_rst_n','RXD','spi_clk','spi_cs','go','back','left','right','stop'}
required.update(f'spi_data[{n}]' for n in range(4))
for cam in (2,5,6):
    required.update(f'cmos{cam}_{s}' for s in ('scl','sda','reset','pclk','href','vsync'))
    required.update(f'cmos{cam}_data[{n}]' for n in range(8))
assert required<=dict(pins).keys(),required-dict(pins).keys()
top=(D/'image_pcie_capture.v').read_text(encoding='utf-8')
assert '.wframe_data0(isp2_data)' in top and '.wframe_data1(isp5_data)' in top and '.wframe_data2(isp6_data)' in top
assert top.count('night_isp isp')==3 and 'safe_fspi bus' in top
print(f'Manifest PASS: {len(rel)} design files; {len(pins)} unique ball assignments; three enhanced camera inputs.')
if a.pyslang_path:
    sys.path.insert(0,a.pyslang_path)
    import pyslang
    from pyslang.syntax import SyntaxTree
    from pyslang.ast import Compilation
    sources=[(D/r).resolve() for r in rel if r.endswith('.v')]
    c=Compilation()
    for f in sources:
        text=f.read_text(encoding='utf-8',errors='replace')
        # fromText does not infer an include search directory. Expand the one
        # same-directory vendor parameter include without editing its source.
        text=re.sub(r'`include\s+"([^\"]+)"',lambda m:(f.parent/m.group(1)).read_text(encoding='utf-8'),text)
        c.addSyntaxTree(SyntaxTree.fromText('`timescale 1ns/1ps\n'+text,str(f)))
    report=pyslang.DiagnosticEngine.reportAll(c.sourceManager,c.getAllDiagnostics())
    out=ROOT/'tmp/adas_sim';out.mkdir(parents=True,exist_ok=True);(out/'full-top-lint.log').write_text(report,encoding='utf-8')
    allowed={'clk_1080p_gen','ddr3','pcie_test','W_FIFO_16i_128o','R_FIFO_128i_128o',
             'hsst_rst_cross_sync_v1_0','hsst_rst_sync_v1_0','GTP_DRM18K','GTP_DRM18K_E1','GTP_DRM9K','GTP_RAM32X1D'}
    errors=[line for line in report.splitlines() if 'error:' in line]
    unexpected=[]
    for line in errors:
        m=re.search(r"unknown module '([^']+)'",line)
        if not m or m.group(1) not in allowed:unexpected.append(line)
    if unexpected:print('\n'.join(unexpected));raise SystemExit(1)
    print('User RTL elaboration PASS; vendor IDF/primitives intentionally unresolved. See full-top-lint.log.')
