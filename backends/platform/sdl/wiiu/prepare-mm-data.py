#!/usr/bin/env python3
"""Refresh Xeen constants from this checkout; preserve other bundled mm.dat data."""
import os
from pathlib import Path
import re
import shlex
import subprocess
import sys
import tempfile
import zipfile

root = Path(__file__).resolve().parents[4]
tool = root / "devtools/create_mm/create_xeen"


def prepare(output):
    engine = (root / "engines/mm/xeen/files.cpp").read_text()
    required = re.search(r'load_engine_data\("mm.dat", "xeen", (\d+), (\d+),', engine)
    if not required:
        raise RuntimeError("Could not determine Xeen engine-data version")
    version = (required[1] + "." + required[2] + "\n").encode()
    with tempfile.TemporaryDirectory(prefix="scummvm-mm-data-") as tmp:
        work = Path(tmp)
        # Native generator only: never include the PowerPC build's config.h.
        (work / "config.h").write_text(
            "#define SCUMM_" + sys.byteorder.upper() + "_ENDIAN\n"
            "#define SCUMM_NEED_ALIGNMENT\n")
        (work / "generator").mkdir()
        (work / "files/xeen").mkdir(parents=True)
        sources = ["create_xeen", "cc", "clouds", "constants", "hashmap", "map",
                   "memorypool", "str", "swords"]
        compiler = shlex.split(os.environ.get("WIIU_HOST_CXX", "c++"))
        subprocess.run([*compiler, "-std=c++11", "-O2", "-DHAVE_CONFIG_H",
                        "-I" + str(work), "-I" + str(root),
                        *[str(tool / (s + ".cpp")) for s in sources],
                        "-o", str(work / "create_xeen")], check=True)
        subprocess.run([str(work / "create_xeen"), "--constants-only"],
                       cwd=work / "generator", check=True)
        generated = {"xeen/" + p.name: p.read_bytes()
                     for p in (work / "files/xeen").iterdir() if p.is_file()}
        assert generated["xeen/version.txt"] == version
        expected = {"xeen/version.txt"} | {
            "xeen/" + prefix + str(lang)
            for prefix in ("CONSTANTS_", "CONSTKEYS_") for lang in (4, 7, 8, 26)}
        assert generated.keys() == expected
        with zipfile.ZipFile(root / "dists/engine-data/mm.dat") as original:
            assert original.testzip() is None
            contents = {n: original.read(n) for n in original.namelist() if not n.endswith("/")}
        contents.update(generated)
        output.parent.mkdir(parents=True, exist_ok=True)
        with zipfile.ZipFile(output, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as dest:
            for name, data in sorted(contents.items()):
                entry = zipfile.ZipInfo(name, date_time=(1980, 1, 1, 0, 0, 0))
                entry.compress_type = zipfile.ZIP_DEFLATED
                entry.external_attr = 0o100644 << 16
                dest.writestr(entry, data)
        with zipfile.ZipFile(output) as result:
            assert result.testzip() is None
            assert result.read("xeen/version.txt") == version
            for name, data in contents.items():
                assert result.read(name) == data
    print("Prepared mm.dat with source-generated Xeen " + version.decode().strip() + " constants: " + str(output))


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("Usage: prepare-mm-data.py OUTPUT/mm.dat (requires a native C++ compiler)")
    prepare(Path(sys.argv[1]).resolve())
