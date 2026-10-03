#!/usr/bin/env python3
"""Compile real SDL lifecycle functions with host mocks (not GPU validation)."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

source = Path(sys.argv[1]).read_text()
baseline = len(sys.argv) > 2 and sys.argv[2] == "--baseline"

def function(name):
    start = source.index("void " + name + "(") if "void " + name + "(" in source else source.index("int " + name + "(")
    opening = source.index("{", start)
    depth = 1
    end = opening + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[start:end]

names = ["WIIU_SDL_DestroyWindowTex", "WIIU_SDL_DestroyRenderer"]
if not baseline:
    names.insert(0, "WIIU_SDL_CreateWindowTex")
    create = function("WIIU_SDL_CreateRenderer")
    assert "if (!data->ctx)" in create
    assert "if (WIIU_SDL_CreateWindowTex(renderer, window) < 0)" in create
    assert "WIIU_SDL_DestroyRenderer(renderer);" in create

with tempfile.TemporaryDirectory(prefix="wiiu-renderer-test-") as tmp:
    tmp = Path(tmp)
    (tmp / "extracted.c").write_text("\n".join(function(n) for n in names))
    flags = ["-DTEST_BASELINE"] if baseline else []
    subprocess.run([os.environ.get("CC", "cc"), "-std=c11", "-g", "-Wall", "-Wextra",
                    "-Wno-unused-parameter", "-Wno-unused-function", "-Werror",
                    "-fsanitize=address,undefined", "-fno-omit-frame-pointer", *flags,
                    "-I" + str(tmp), str(Path(__file__).with_name("test.c")),
                    "-o", str(tmp / "test")], check=True)
    subprocess.run([str(tmp / "test")], check=True)
