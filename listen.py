#!/usr/bin/env python

import sys
import os
import json
from time import sleep
from subprocess import call
import requests


API_URL = 'https://ino-state-server.herokuapp.com'


state = {}

while True:
    res = requests.get('%s/state' % API_URL)
    new_state = res.json()

    if state:
        if not state['deploying'] and new_state['deploying']:
            call(['./command.py', 'working'])
        elif state['deploying'] and not new_state['deploying']:
            call(['./command.py', 'finished'])

    state = new_state
    sleep(2)
