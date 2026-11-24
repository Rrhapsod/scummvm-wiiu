#!/usr/bin/env python3
"""Keep RPX loader metadata in the forward read order used by Cafe OS."""
import argparse
import struct
from pathlib import Path

SECTION = struct.Struct(">10I")
IMPORTS = 0x80000002
EXPORTS = 0x80000001


def order_metadata(source):
    if source[:7] != b"\x7fELF\x01\x02\x01" or source[16:20] != b"\xfe\x01\x00\x14":
        raise ValueError("expected a big-endian PowerPC RPX")
    table, = struct.unpack_from(">I", source, 32)
    stride, count = struct.unpack_from(">HH", source, 46)
    if stride != SECTION.size or table + count * stride > len(source):
        raise ValueError("invalid section table")

    # Cafe processes allocated data first, then loader sections in section
    # index order, then text. elf2rpl 1.3.4 moves imports behind the string
    # tables on disk, which can force a backward read across a 4 MiB window.
    group = []
    for index in range(count):
        _, kind, flags, _, offset, size, *_ = SECTION.unpack_from(source, table + index * stride)
        if size and kind != 8 and flags & 2 and not flags & 1:
            if kind in (IMPORTS, EXPORTS) or not flags & 4:
                group.append((offset, size, index))
    if not group:
        raise ValueError("missing RPX loader metadata")

    physical = sorted(group)
    start = physical[0][0]
    end = start
    for offset, size, _ in physical:
        if offset != end or offset + size > len(source):
            raise ValueError("unexpected elf2rpl metadata layout")
        end += size
    if start < table + count * stride:
        raise ValueError("metadata overlaps section headers")

    result = bytearray(source)
    cursor = start
    for offset, size, index in group:
        result[cursor:cursor + size] = source[offset:offset + size]
        struct.pack_into(">I", result, table + index * stride + 16, cursor)
        cursor += size
    return result


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    args.output.write_bytes(order_metadata(args.source.read_bytes()))
