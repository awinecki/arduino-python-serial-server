#!/usr/bin/env python
import sys
from commands import Commander

arg = sys.argv[1]
Commander('localhost', 6000, authkey=b'qweqwe').order(arg)
