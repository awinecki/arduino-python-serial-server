import asyncio
import itertools
import math

frequency = .1


class RGBLed(object):

    def __init__(self, name, commander, loop):
        asyncio.set_event_loop(loop)
        self.name = name
        self.commander = commander
        self.tasks = []

    def set_rgb(self, r, g, b):
        self.commander.order('%s:rgb:%s,%s,%s' % (self.name, r, g, b))

    def reset(self):
        self.commander.order('%s:rgb:0,0,0' % self.name)

    def stop(self):
        for task in self.tasks:
            task.cancel()

    def pulse(self, r, g, b):
        self.tasks.append(asyncio.ensure_future(self.async_pulse(r, g, b)))

    def cycle(self):
        self.tasks.append(asyncio.ensure_future(self.async_cycle()))

    def blink(self, r, g, b, count, delay):
        self.tasks.append(asyncio.ensure_future(
            self.async_blink(r, g, b, count, delay)))

    async def async_pulse(self, r=0, g=0, b=0):
        try:
            for i in itertools.cycle(range(round(2 * math.pi / frequency))):
                color_intensity = math.sin(frequency * i) * 127 + 128
                red, green, blue = r, g, b
                if r == 'pulse':
                    red = color_intensity
                elif g == 'pulse':
                    green = color_intensity
                elif b == 'pulse':
                    blue = color_intensity
                self.set_rgb(red, green, blue)
                await asyncio.sleep(.1)
        except asyncio.CancelledError:
            print('Cycle cancelled.')

    async def async_cycle(self):
        try:
            for i in itertools.cycle(range(round(2 * math.pi / frequency))):
                red = math.sin(frequency * i + 0) * 127 + 128
                green = math.sin(frequency * i + math.pi * 2 / 3) * 127 + 128
                blue = math.sin(frequency * i + math.pi * 4 / 3) * 127 + 128
                self.set_rgb(red, green, blue)
                await asyncio.sleep(.1)
        except asyncio.CancelledError:
            print('Cycle cancelled.')

    async def async_blink(self, r, g, b, count, delay):
        try:
            for i in range(count):
                await asyncio.sleep(delay)
                self.set_rgb(r, g, b)
                await asyncio.sleep(delay)
                self.set_rgb(0, 0, 0)
                await asyncio.sleep(delay)
        except asyncio.CancelledError:
            print('Blink cancelled.')



class SimpleLed(object):

    def __init__(self, name, commander, loop):
        self.name = name
        self.commander = commander
        asyncio.set_event_loop(loop)

    def set(self, level):
        self.commander.order('%s:brightness:%s' % (self.name, level))

    def reset(self):
        self.commander.order('%s:brightness:0' % self.name)

    def start_pulse(self):
        self.pulse_task = asyncio.ensure_future(self.pulse())

    def stop(self):
        self.pulse_task.cancel()

    async def pulse(self):
        try:
            for i in itertools.cycle(range(round(2 * math.pi / frequency))):
                self.set(math.sin(frequency * i + 0) * 15 + 15.5)
                await asyncio.sleep(.1)
        except asyncio.CancelledError:
            print('Pulse cancelled.')



class Buzzer(object):

# for i in range(3):
#     sleep(.2)
#     bz.pitch(2)
#     sleep(.2)
#     bz.pitch(0)

    def __init__(self, name, commander):
        self.name = name
        self.commander = commander

    def pitch(self, level):
        self.commander.order('%s:pitch:%s' % (self.name, level))

    def reset(self):
        self.commander.order('%s:pitch:0' % self.name)

