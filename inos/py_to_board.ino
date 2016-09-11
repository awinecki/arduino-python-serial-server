#include "pitches.h"

#define n2 0.5
#define n4 0.25
#define n8 0.125
#define n16 0.0625
#define n32 0.03125

#define smallRed 11
#define red 10
#define green 7
#define blue 4
#define buzz 2
int color[3];
int delay_ms = 4;
int pulse_step = 5;
int pulse_level = 0;
int pulse_cycles = 0;
int pulse_direction = HIGH;
int currentPulsing = 5;

float tempo = 120; // 120 quarter notes per minute!
float time_signature = 4/4;
int note_freq = 10;

String inputString = "";
boolean stringComplete = false;

void setup() {
  pinMode(smallRed, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(buzz, OUTPUT);

  Serial.begin(9600);
  inputString.reserve(200);
}

void setRgb(int r, int g, int b) {
  analogWrite(red, r);
  analogWrite(green, g);
  analogWrite(blue, b);
}

void setColor() {
  analogWrite(red, color[0]);
  analogWrite(green, color[1]);
  analogWrite(blue, color[2]);
}

void resetColor() {
  for (int i = 0; i < 3; i += 1) {
    color[i] = 0;
  }
}

void startPulsing(int color_idx, int cycles=0) {
  resetColor();
  currentPulsing = color_idx;
  pulse_cycles = cycles;
}

void stopPulsing() {
  currentPulsing = 5;
  pulse_level = 0;
  pulse_direction = HIGH;
  pulse_cycles = 0;
}

void handlePulseStep() {
  if (currentPulsing > 3) {
    return false;
  }

  if (pulse_direction == HIGH) {
    pulse_level += 1;
  } else {
    pulse_level -= 1;
  }

  if (pulse_level > 255) {
    pulse_level = 255;
    pulse_direction = !pulse_direction;
    pulse_cycles -= 1;
  }

  if (pulse_level < 0) {
    pulse_level = 0;
    pulse_direction = !pulse_direction;
    pulse_cycles -= 1;
  }

  if (pulse_cycles == 1) {
    stopPulsing();
  }

  resetColor();
  color[currentPulsing] = pulse_level;
  setColor();
}

void pulseColor(int color_idx, boolean rise_intensity, int delayOffset=0,
    int pulseOffset=0) {
  for (int i = 0; i < 255; i += pulse_step + pulseOffset) {
    resetColor();
    if (rise_intensity) {
      color[color_idx] = i;
    } else {
      color[color_idx] = 255 - i;
    }
    if (color[color_idx] > 255) {
      color[color_idx] = 255;
    }
    if (color[color_idx] < 1) {
      color[color_idx] = 0;
    }
    setColor();
    delay(delay_ms + delayOffset);
  }
}

void note(int freq, float note_length, float delay_after=0) {
  float delay_calculated = 60.0 * 1000.0 / tempo * 4.0 * note_length;
  float delay_after_calculated = 60.0 * 1000.0 / tempo * 4.0 * delay_after;
  delay_after_calculated += delay_calculated;
  tone(buzz, NOTE_B0, delay_calculated);
  delay(delay_after_calculated);
}


void melodyA() {
  note(NOTE_G7, n16);
  delay(30);
  note(NOTE_G7, n16, n16);
  delay(30);
  note(NOTE_D8, n16);
}

void led_blink() {
  for (int i = 0; i < 10; i += 1) {
    setRgb(50, 0, 0);
    delay(100);
    setRgb(0, 50, 0);
    delay(100);
    setRgb(0, 0, 50);
    delay(100);
  }
  setRgb(0, 0, 0);
}

void alert() {
  for (int i = 0; i < 11; i += 1) {
    setRgb(200, 0, 0);
    delay(100);
    setRgb(0, 0, 0);
    note(NOTE_E3, n8);
    delay(10);
  }
}

void error() {
  for (int i = 0; i < 3; i += 1) {
    analogWrite(buzz, 30);
    delay(20);
    analogWrite(buzz, 0);
    /* setRgb(50, 0, 0); */
    digitalWrite(smallRed, HIGH);
    delay(200);
    /* setRgb(0, 0, 0); */
    digitalWrite(smallRed, LOW);
    delay(200);
  }
}

void loop() {

  if (stringComplete) {
    if (inputString == "melody\n") {
      melodyA();
    } else if (inputString == "blink\n") {
      led_blink();
    } else if (inputString == "pulse\n") {
      for (int i = 0; i < 4; i += 1 ) {
        pulseColor(2, true, 12);
        pulseColor(2, false, 12);
      }
      pulseColor(1, true, 12);
      pulseColor(1, false, 12);
      setRgb(0, 0, 0);
    } else if (inputString == "alert\n") {
      alert();
    } else if (inputString == "error\n") {
      error();
    } else if (inputString == "goodpulse\n") {
      pulseColor(2, true, 5, -4);
      pulseColor(2, false, 5, -4);
      setRgb(0, 0, 0);
    } else if (inputString == "working\n") {
      startPulsing(2);
    } else if (inputString == "finished\n") {
      /* stopPulsing(); */
      startPulsing(1, 7);
    }
    inputString = "";
    stringComplete = false;
  }

  handlePulseStep();
  delay(delay_ms);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
