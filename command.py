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

ino = serial.Serial('/dev/cu.usbmodem1421', 9600)
# ino.flushInput()

# sleep(1)
# while True and not ino.is_open:
#     sleep(.05)
arg = sys.argv[1]
ino.write(arg + '\n')
ino.close()
