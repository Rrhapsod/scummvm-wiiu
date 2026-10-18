#!/usr/bin/env python3
"""Compare the real patched SDL blitter/dispatch with the previous SDL source.

Usage: run.py SDL_SOURCE HOST_BUILD ORIGINAL_SDL_BLIT_N.c
The host SDL build must use SDL_STATIC=ON. Timings are not Wii U measurements.
"""
from pathlib import Path
import subprocess
import sys
import tempfile

source, build, baseline = [Path(p).resolve() for p in sys.argv[1:]]
test = Path(__file__).with_name('test.c')
includes = ['-I' + str(p) for p in [build / 'include', build / 'include/SDL2',
    build / 'include-config-release', build / 'include-config-release/SDL2', source / 'src/video']]
with tempfile.TemporaryDirectory(prefix='wiiu-blit-') as tmp:
    tmp = Path(tmp)
    # Keep the original relative internal includes valid without editing SDK files.
    original = tmp / 'baseline.c'
    text = baseline.read_text().replace('#include "../SDL_internal.h"',
        '#include "' + str(source / 'src/SDL_internal.h') + '"')
    original.write_text(text)
    obj = tmp / 'baseline.o'
    subprocess.run(['cc', '-O2', '-DUSING_GENERATED_CONFIG_H', '-DSDL_STATIC_LIB',
        '-DSDL_CalculateBlitN=Baseline_CalculateBlitN', *includes, '-c', str(original), '-o', str(obj)], check=True)
    for sanitize in [True, False]:
        exe = tmp / ('check' if sanitize else 'benchmark')
        flags = ['-fsanitize=address,undefined', '-no-pie'] if sanitize else ['-DBENCHMARK']
        patched = tmp / 'patched.o'
        subprocess.run(['cc', '-O2', '-g', '-DUSING_GENERATED_CONFIG_H', '-DSDL_STATIC_LIB',
            *flags, *includes, '-c', str(source / 'src/video/SDL_blit_N.c'), '-o', str(patched)], check=True)
        subprocess.run(['cc', '-O2', '-g', '-DUSING_GENERATED_CONFIG_H', '-DSDL_STATIC_LIB',
            *flags, *includes, str(test), str(obj), str(patched), str(build / 'libSDL2.a'),
            '-lm', '-ldl', '-lpthread', '-o', str(exe)], check=True)
        subprocess.run([str(exe)], check=True)
