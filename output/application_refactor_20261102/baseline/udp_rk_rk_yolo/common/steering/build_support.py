"""Shared compiler/test helpers. No M or S implementation is imported here."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess

PROJECT_ROOT = Path(__file__).resolve().parents[2]
EXE = ".exe" if os.name == "nt" else ""
CC = os.environ.get("CC", "gcc")
BASE = ["-std=c11", "-O2", "-Wall", "-Wextra", "-Werror", "-D_POSIX_C_SOURCE=200809L"]


def compile_program(output: Path, sources: list[Path], includes: list[Path],
                    extra: list[str] | None = None) -> Path:
    if not shutil.which(CC):
        raise RuntimeError(f"Compiler not found: {CC}. Install GCC or set CC to its executable path.")
    output = output.with_name(output.name + EXE)
    output.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run([CC, *BASE, *(f"-I{p}" for p in includes), *(extra or []),
                    *(str(p) for p in sources), "-lm", "-o", str(output)], check=True)
    print(f"BUILT {output.relative_to(PROJECT_ROOT.parent)}", flush=True)
    return output


def run(program: Path, args: list[str], expected: int = 0) -> subprocess.CompletedProcess[str]:
    result = subprocess.run([str(program), *args], text=True, capture_output=True, timeout=15)
    if result.returncode != expected:
        raise AssertionError(f"{program.name} {args}: expected {expected}, got {result.returncode}\n"
                             f"{result.stdout}\n{result.stderr}")
    return result
