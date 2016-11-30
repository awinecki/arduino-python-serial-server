#!/usr/bin/env python

"""
Available commands:

    - alert
    - blink
    - pulse
    - melody

"""

import sys
import os
import serial
from time import sleep
from subprocess import call

SERIAL_PORT = '/dev/cu.usbmodem14131'

ino = serial.Serial(SERIAL_PORT, 9600)
# ino.flushInput()

# sleep(1)
# while True and not ino.is_open:
#     sleep(.05)
arg = sys.argv[1]
ino.write(arg + '\n')
ino.close()
