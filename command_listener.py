#!/usr/bin/env python
import sys
import serial
import signal
import logging
import coloredlogs
from multiprocessing.connection import Listener

log = logging.getLogger('INO Command listener')
coloredlogs.install(level='DEBUG')

ino = serial.Serial('/dev/cu.usbmodem14221', 9600)
address = ('localhost', 6000)
listener = Listener(address, authkey=b'qweqwe')
log.info('Started. Ready to accept commands')
connection = None

def exit_process():
    global connection
    ino.close()
    if connection:
        connection.close()
    listener.close()
    log.info('Exiting..')
    sys.exit(0)


def signal_handler(signal, frame):
    exit_process()

signal.signal(signal.SIGINT, signal_handler)

def listen_for_msg():
    global connection
    connection = listener.accept()
    msg = connection.recv_bytes().decode()
    log.info('Command received: %s' % msg)
    if msg == 'close':
        exit_process()
    ino.write('{}\n'.format(msg).encode())
    connection.close()

while True:
    listen_for_msg()

listener.close()
