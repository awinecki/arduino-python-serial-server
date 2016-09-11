#!/usr/bin/env python
import sys
import datetime
import aiohttp
import signal
import itertools
import requests
import math
import asyncio
from time import sleep
from commands import Commander
from resources import SimpleLed, RGBLed, Buzzer


# Config & setup
API_URL = 'https://ino-state-server.herokuapp.com'
commander = Commander('localhost', 6000, authkey=b'qweqwe')
frequency = .1
loop = asyncio.get_event_loop()

# Resources
led1 = RGBLed('one', commander, loop)
led2 = RGBLed('two', commander, loop)
simple_led = SimpleLed('smallRed', commander, loop)
bz = Buzzer('buzz', commander)

state = {}

def handle_state(state, prev_state):
    print(state)


async def fetch_state(loop):
    async with aiohttp.ClientSession(loop=loop) as session:
        async with session.get('%s/state' % API_URL) as response:
            return await response.json()


async def check_state(loop):
    global state
    try:
        while True:
            new_state = await fetch_state(loop)
            if not state:
                state = new_state

            print(state)

            if new_state.get('deploying_prod') and \
                    not state.get('deploying_prod'):
                print('startdeploy')
                led1.stop()
                led1.pulse(240, 0, 'pulse')
            if not new_state.get('deploying_prod') and \
                    state.get('deploying_prod'):
                print('enddeploy')
                led1.stop()
                led1.blink(0, 200, 0, 5, .20)

            if new_state.get('deploying_stage') and \
                    not state.get('deploying_stage'):
                print('startdeploy')
                led2.stop()
                led2.pulse(0, 0, 'pulse')
            if not new_state.get('deploying_stage') and \
                    state.get('deploying_stage'):
                print('enddeploy')
                led2.stop()
                led2.blink(0, 200, 0, 5, .20)

            state = new_state
            await asyncio.sleep(1)
    except asyncio.CancelledError:
        print('Fetch state cancelled.')


state_task = asyncio.ensure_future(check_state(loop))


def exit():
    led1.stop()
    # simple_led.stop()
    loop.stop()
    led1.reset()
    led2.reset()
    simple_led.reset()
    print('Exiting..')
    sys.exit(0)

loop.add_signal_handler(signal.SIGINT, exit)
loop.run_forever()
