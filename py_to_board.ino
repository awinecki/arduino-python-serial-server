#include "pitches.h"

#define n2 0.5
#define n4 0.25
#define n8 0.125
#define n16 0.0625
#define n32 0.03125

#define smallRed 5
/* #define red 10 */
/* #define green 7 */
/* #define blue 4 */
#define buzz 2
#define DELAY 1

// Define leds
int rgbOne[3] = {12, 11, 10};
int rgbTwo[3] = {9, 8, 7};

// Colors
int rgbOneColor[3] = {0, 0, 0};
int rgbTwoColor[3] = {0, 0, 0};

// State
boolean rgbOnePulsing = false;
int rgbOnePulse = 0;
int rgbOnePulseStep = 3;
int rgbOnePulseDirection = HIGH;

// Music variables
float tempo = 120; // 120 quarter notes per minute!
float time_signature = 4/4;
int note_freq = 10;
boolean colorToggle = LOW;

// Commands
String inputString = "";

void initRgbLed(int ledPins[3]) {
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);
}

void setup() {
  initRgbLed(rgbOne);
  pinMode(smallRed, OUTPUT);
  pinMode(buzz, OUTPUT);
  Serial.begin(9600);
  inputString.reserve(200);
}

void resetRgbColor(int colorArray[3]) {
  for (int i = 0; i < 3; i += 1) {
    colorArray[i] = 0;
  }
}

// setRgb
void writeRgbColor(int ledPins[3], int r, int g, int b) {
  analogWrite(ledPins[0], r);
  analogWrite(ledPins[1], g);
  analogWrite(ledPins[2], b);
}

// tColor
void setRgbColor(int colorArray[3], int r, int g, int b) {
  colorArray[0] = r;
  colorArray[1] = g;
  colorArray[2] = b;
}

// writecolor
void writeRgbFromColor(int ledPins[3], int colorArray[3]) {
  analogWrite(ledPins[0], colorArray[0]);
  analogWrite(ledPins[1], colorArray[1]);
  analogWrite(ledPins[2], colorArray[2]);
}

void alternateColor() {
  if (colorToggle == HIGH) {
    writeRgbFromColor(rgbOne, rgbOneColor);
    writeRgbFromColor(rgbTwo, rgbTwoColor);
  } else {
    writeRgbFromColor(rgbOne, rgbOneColor);
    writeRgbFromColor(rgbTwo, rgbTwoColor);
    /* writeRgbColor(rgbOne, 0, 0, 0); */
    /* writeRgbColor(rgbTwo, 0, 0, 0); */
  }
  colorToggle = !colorToggle;
}

void debug() {
  Serial.print(rgbOneColor[0]);
  Serial.print(" ");
  Serial.print(rgbOneColor[1]);
  Serial.print(" ");
  Serial.print(rgbOneColor[2]);
  Serial.println(" ");
}

void handleCommand(String command) {
  int colon1 = command.indexOf(':');
  String resource =  command.substring(0, colon1);
  int colon2 = command.indexOf(':', colon1 + 1);
  String instruction = command.substring(colon1 + 1, colon2);
  String param = command.substring(colon2 + 1, command.length());

  if (instruction == "setcolor") {
    if (param == "green") {
      if (resource == "one") {
        setRgbColor(rgbOneColor, 0, 200, 0);
      } else if (resource == "two") {
        setRgbColor(rgbTwoColor, 0, 200, 0);
      }
    } else if (param == "blue") {
      if (resource == "one") {
        setRgbColor(rgbOneColor, 0, 0, 200);
      } else if (resource == "two") {
        setRgbColor(rgbTwoColor, 0, 0, 200);
      }
    } else if (param == "red") {
      if (resource == "one") {
        setRgbColor(rgbOneColor, 200, 0, 0);
      } else if (resource == "two") {
        setRgbColor(rgbTwoColor, 200, 0, 0);
      }
    } else if (param == "reset") {
      if (resource == "one") {
        setRgbColor(rgbOneColor, 0, 0, 0);
      } else if (resource == "two") {
        setRgbColor(rgbTwoColor, 0, 0, 0);
      }
      rgbOnePulsing = false;
    } else if (param == "pulse") {
      rgbOnePulsing = true;
    }
  } else if (instruction == "rgb") {
    int comma1 = param.indexOf(',');
    int r = param.substring(0, comma1).toInt();
    int comma2 = param.indexOf(',', comma1 + 1);
    int g = param.substring(comma1 + 1, comma2).toInt();
    int b = param.substring(comma2 + 1, param.length()).toInt();
    if (resource == "one") {
      writeRgbColor(rgbOne, r, g, b);
    } else if (resource == "two") {
      writeRgbColor(rgbTwo, r, g, b);
    }
  } else if (instruction == "pitch") {
    analogWrite(buzz, param.toInt());
  } else if (instruction == "brightness") {
    analogWrite(smallRed, param.toInt());
  }
}

void stepPulse() {
  if (!rgbOnePulsing) {
    return;
  }

  if (rgbOnePulseDirection == HIGH) {
    rgbOnePulse += rgbOnePulseStep;
  } else {
    rgbOnePulse -= rgbOnePulseStep;
  }
  if (rgbOnePulse > 254) {
    rgbOnePulse = 255;
  } else if (rgbOnePulse < 1) {
    rgbOnePulse = 0;
  }
  if (rgbOnePulse == 255) {
    rgbOnePulseDirection = LOW;
  } else if (rgbOnePulse == 0) {
    rgbOnePulseDirection = HIGH;
  }
  writeRgbColor(rgbOne, 0, rgbOnePulse, rgbOnePulse);
  writeRgbColor(rgbTwo, rgbOnePulse, rgbOnePulse, 0);
}

void do_step() {
  /* alternateColor(); */
  /* stepPulse(); */
}

void loop() {
  if (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    inputString += inChar;

    if (inChar == '\n') {
      handleCommand(inputString.substring(0, inputString.length() - 1));
      /* Serial.println(inputString.substring(0, inputString.length() - 1 )); */
      inputString = "";
      Serial.flush();
    }
  }

  do_step();
  /* debug(); */
  delay(DELAY);
}
