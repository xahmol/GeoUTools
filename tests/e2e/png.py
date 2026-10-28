"""Minimal palette PNG reader/writer for the end-to-end golden images.

Python 3 standard library only. Writes 8-bit palette PNGs (one byte per
pixel, colour type 3), which any image viewer shows in the program's own
colours, and reads back the palette PNGs it wrote (all five scanline
filters are supported, so a golden re-saved by another tool still loads
as long as it stays a palette image).
"""

import struct
import zlib

_SIGNATURE = b"\x89PNG\r\n\x1a\n"


def _chunk(kind, data):
    body = kind + data
    return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)


def write(path, rows, palette):
    """rows: list of bytes-like rows of palette indexes; palette: list of (r, g, b)."""
    height, width = len(rows), len(rows[0])
    raw = b"".join(b"\x00" + bytes(r) for r in rows)
    data = (_SIGNATURE
            + _chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 3, 0, 0, 0))
            + _chunk(b"PLTE", b"".join(bytes(c) for c in palette))
            + _chunk(b"IDAT", zlib.compress(raw, 9))
            + _chunk(b"IEND", b""))
    with open(path, "wb") as f:
        f.write(data)


def read(path):
    """Return (rows, palette) of a palette PNG with 8 bits per pixel."""
    with open(path, "rb") as f:
        data = f.read()
    if not data.startswith(_SIGNATURE):
        raise ValueError("%s: not a PNG" % path)
    pos, idat, palette = 8, b"", []
    while pos < len(data):
        length, kind = struct.unpack_from(">I4s", data, pos)
        body = data[pos + 8:pos + 8 + length]
        pos += 12 + length
        if kind == b"IHDR":
            width, height, depth, ctype = struct.unpack_from(">IIBB", body)
            if (depth, ctype) != (8, 3):
                raise ValueError("%s: not an 8-bit palette PNG" % path)
        elif kind == b"PLTE":
            palette = [tuple(body[i:i + 3]) for i in range(0, len(body), 3)]
        elif kind == b"IDAT":
            idat += body
    raw = zlib.decompress(idat)
    rows, prev = [], bytearray(width)
    for y in range(height):
        start = y * (width + 1)
        ftype, line = raw[start], bytearray(raw[start + 1:start + 1 + width])
        for x in range(width):
            a = line[x - 1] if x else 0
            b, c = prev[x], (prev[x - 1] if x else 0)
            if ftype == 1:
                line[x] = (line[x] + a) & 0xFF
            elif ftype == 2:
                line[x] = (line[x] + b) & 0xFF
            elif ftype == 3:
                line[x] = (line[x] + (a + b) // 2) & 0xFF
            elif ftype == 4:
                p = a + b - c
                pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                line[x] = (line[x] + (a if pa <= pb and pa <= pc else b if pb <= pc else c)) & 0xFF
        rows.append(bytes(line))
        prev = line
    return rows, palette
