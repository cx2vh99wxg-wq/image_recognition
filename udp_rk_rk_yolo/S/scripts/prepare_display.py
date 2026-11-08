#!/usr/bin/env python3
"""Compatibility entry point for make prepare-display."""
from pathlib import Path
import runpy

def prepare():
    module=Path(__file__).resolve().parents[2]/'common/display/prepare_display.py'
    runpy.run_path(str(module),run_name='vision_display')['prepare']()

if __name__ == '__main__':
    prepare()
