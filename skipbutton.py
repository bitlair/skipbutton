#!/usr/bin/env python3

from mpd import MPDClient
from serial import Serial
import sys


serial_dev = sys.argv[1]


MSG_LEN     = 3
MSG_START   = 0x55
MSG_END     = 0xCC
MSG_SKIP    = 0x01
MSG_VOLDOWN = 0x02
MSG_VOLUP   = 0x03


def serial_bytes(ser):
    while (buf := ser.read(1)):
        yield buf[0]

def unflip(it):
    a = next(it)
    while True:
        b = next(it)
        if a == ~b & 0xff:
            yield a
        a = b

def messages(it):
    frame = [0, 0, 0]
    for b in it:
        frame.append(b)
        frame.pop(0)
        if frame[0] != MSG_START or frame[2] != MSG_END:
            continue
        msg = frame[1]

        if msg == MSG_SKIP:
            yield 'skip'
        elif msg == MSG_VOLDOWN:
            yield 'vol-'
        elif msg == MSG_VOLUP:
            yield 'vol+'


def main():
    mpdc = MPDClient()
    mpdc.connect('localhost', 6600)

    with Serial(serial_dev, 600, timeout=1) as ser:
        for m in messages(unflip(serial_bytes(ser))):
            if m == 'skip':
                mpdc.next()
            elif m == 'vol+':
                mpdc.volume(5)
            elif m == 'vol-':
                mpdc.volume(-5)


if __name__ == '__main__':
    main()
