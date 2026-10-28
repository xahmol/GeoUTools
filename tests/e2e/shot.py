#!/usr/bin/env python3
"""Save the current screen of an Ultimate 64 / C64 Ultimate as a PNG.

    tests/e2e/shot.py --device 192.168.1.148 build/e2e/screen.png

Captures one frame of the VIC video stream (multicast 239.0.1.64, see
ultimate.py). ultimate.py and png.py are copied from mandelbrot-upic's
tests/e2e (Xander Mol, same author).
"""

import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import png  # noqa: E402
from ultimate import Ultimate  # noqa: E402

# Colodore/VICE-like C64 palette (RGB)
PALETTE = [
    (0x00, 0x00, 0x00), (0xFF, 0xFF, 0xFF), (0x81, 0x33, 0x38), (0x75, 0xCE, 0xC8),
    (0x8E, 0x3C, 0x97), (0x56, 0xAC, 0x4D), (0x2E, 0x2C, 0x9B), (0xED, 0xF1, 0x71),
    (0x8E, 0x50, 0x29), (0x55, 0x38, 0x00), (0xC4, 0x6C, 0x71), (0x4A, 0x4A, 0x4A),
    (0x7B, 0x7B, 0x7B), (0xA9, 0xFF, 0x9F), (0x70, 0x6D, 0xEB), (0xB2, 0xB2, 0xB2),
]


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--device", required=True)
    ap.add_argument("--port", type=int, default=11000)
    ap.add_argument("out")
    a = ap.parse_args()
    u = Ultimate(a.device)
    with u.video_stream(a.port) as stream:
        frame = stream.frames(1)[0]
    os.makedirs(os.path.dirname(os.path.abspath(a.out)), exist_ok=True)
    png.write(a.out, frame, PALETTE)
    print(a.out)


if __name__ == "__main__":
    main()
