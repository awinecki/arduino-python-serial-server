# Arduino project: communicating over Serial USB using Python

This is a project I've started while learning hardware, in this case Arduino.

I've designed it to control 1 Red LED, 2 RGB LEDs, and a simple buzzer over a simple string-based communication protocol directly from Python. This allows using more high level Python structures to implement specific LED/buzzer behaviour, like pulsing colours, or cycling a color on an RGB LED (with a lot cleaner code than using Arduino language).

## Breadboard

![Breadboard](/breadboard.png?raw=true)

### Parts

- Arduino board. I'm using Mega 2560 rev3 but it's an overkill, we only need 8 PWM pins
- 2 x RGB LED
- 1 x RED LED
- Buzzer
- 8 resistors (adjust OHM yourself. 500-1k ohm should be alright)
- Some male-male cables

## How to run

### Prerequisites

- **Python 3.4.0+** (I'm using `asyncio` core lib for asynchronous communication of multiple peripherals). If you don't want to mess up your py versions, I recommend using [pyenv](https://github.com/yyuu/pyenv).

### Install & setup

- After ensuring you're running new-ish python, run `pip install -r requirements.txt`
- Open `command_listener.py` and look for line:
  ```
  ino = serial.Serial('/dev/cu.usbmodem14221', 9600)
  ```
  Replace `/dev/cu.usbmodem14221` to the port your Arduino is running on
- In `seq.py`, I'm using a herokuapp (URL hard-coded) to keep track of simple JSON state object. Replace that mechanism with anything you like. What the hell, you can even use my app but please namespace your state object keys.

### Run

Connect your Arduino.

```
./command_listener.py
2016-09-11 18:21:14 vimbook INO Command listener[48461] INFO Started. Ready to accept commands
```

Now, you should be able to send commands to the listener process, which then sends instructions to arduino.

The simplest way to do that is by `./send.py` utility.

```
./send.py one:rgb:0,255,0  # set RGB LED no.1 color to green
./send.py two:rgb:0,0,150  # set RGB LED no.2 color to blue
./send.py buzzer:pitch:50  # set buzzer to 50 analog current rate
...
./send.py buzzer:pitch:reset
./send.py one:rgb:0,0,0
./send.py two:rgb:0,0,0
```

Now the fun part. the `seq.py` file opens up a process that fetches the heroku app JSON state object every so often (1sec by default), and manages the LEDs state based on that. Note that because we're using `asyncio`, we can initiate things upon changes. For example, when deployment was started on a server, we can initiate pulsing RGB LED no. 1, then when it ends, flash it green 5 times to show that it was a successful deployment.

#### Have fun & happy hacking!

## How it works

`./command_listener.py` initiates Serial connection with Arduino and keeps it alive, for minimising connection setup overhead later on. It also creates a multiprocessing connection listener so that we're able to send commands from separate python processes.

Every instruction to Arduino follows the `resource_name:instruction:comma_separated_params` format. So for example:

```
one:rgb:20,30,40
```

Means: take RGB LED no. 1, set RGB values, use values 20 for red, 30 for green, 40 for blue.

The resources and instructions are hardcoded in the Arduino sketch code currently, so only a limited actions are supported. However, being able to send arbitrary RGB value to a LED with a relatively low latency connection means that we can define more complex behaviour in python instead.

To see that in action, look into `resources.py` file. It defines arbitrary pulsing, cycling, etc. behaviour, in a friendly class based API.