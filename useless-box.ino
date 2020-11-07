#include <Servo.h>

#include "config.h"
#include "proximity-sensor.h"

Servo switchServo;
Servo lidServo;
ProximitySensor sensor;
int switchButton = D5;

int buttonState = 0;
int currentState = 0;
int sensorInRange = 0;

int currentMode = 0;
int MODES = 4;

int senses = 0;

void setup() {
  pinMode(switchButton, INPUT);
  sensor.attach(PIN_SENSOR_SDA, PIN_SENSOR_SCL, SENSOR_TRIGGER_THRESHOLD);
  switchServo.attach(D4);
  lidServo.attach(D8);
  switchServo.write(0);
  lidServo.write(90);
  Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(switchButton);

  if (buttonState != currentState) {
    //state changed
    Serial.print("State changed to ");
    Serial.println(buttonState);
    if (buttonState == 1) {
      openLid();
      switch (currentMode) {
        case 0:
          goSlow();
          break;
        case 1:
          goSlowThenFast();
          break;
        case 2:
          goFast();
          break;
        case 3:
          goFastThenSlow();
          break;
        default:
          break;
      }
      closeLid();
      // goFastThenSlow();
      currentMode++;
      if (currentMode == MODES) {
        currentMode = 0;
      }
    }
    currentState = buttonState;
  }
  if (sensor.isInRange() && sensorInRange == 0) {
    Serial.println("Sensor is now in range");
    sensorInRange = 1;
    if(currentMode == 2) {
      lidServo.writeMicroseconds(LID_OPEN);
    }
  }
  if (!sensor.isInRange() && sensorInRange == 1 && currentMode == 2) {
    Serial.println("Sensor is now out of range");
    sensorInRange = 0;
    senses++;
    if (senses == 3) {
      senses = 0;
      for (int n = 0; n < 4; n++) {
        lidServo.writeMicroseconds(LID_OPEN);
        delay(100);
        lidServo.writeMicroseconds(LID_CLOSE);
        delay(100);
      }
    }
    lidServo.writeMicroseconds(LID_CLOSE);
  }
}

void openLid() {
  lidServo.writeMicroseconds(LID_OPEN);
  delay(400);
}

void closeLid() {
  delay(400);
  lidServo.writeMicroseconds(LID_CLOSE);
}

void goSlow() {
  int currentPos;
  for (currentPos = ARM_START_POS; currentPos <= ARM_END_POS; currentPos += 8) {
    switchServo.writeMicroseconds(currentPos);
    delay(15);  // fast 2
  }
  if (currentPos < ARM_END_POS) {
    switchServo.writeMicroseconds(ARM_END_POS);
  }
  delay(200);
  switchServo.writeMicroseconds(ARM_START_POS);
}

void goFast() {
  switchServo.writeMicroseconds(ARM_END_POS);
  delay(500);
  switchServo.writeMicroseconds(ARM_START_POS);
}

void goSlowThenFast() {
  int currentPos;
  for (currentPos = ARM_START_POS; currentPos <= ARM_ALMOST_END_POS; currentPos += 8) {
    switchServo.writeMicroseconds(currentPos);
    delay(15);  // fast 2
  }
  if (currentPos < ARM_ALMOST_END_POS) {
    switchServo.writeMicroseconds(ARM_ALMOST_END_POS);
  }
  delay(1000);
  switchServo.writeMicroseconds(ARM_ALMOST_END_POS - 300);
  delay(300);
  switchServo.writeMicroseconds(ARM_END_POS);

  delay(200);
  switchServo.writeMicroseconds(ARM_START_POS);
}
void goFastThenSlow() {
  switchServo.writeMicroseconds(ARM_ALMOST_END_POS);
  delay(1000);
  int currentPos;
  for (currentPos = ARM_ALMOST_END_POS; currentPos <= ARM_END_POS; currentPos += 8) {
    switchServo.writeMicroseconds(currentPos);
    delay(8);  // fast 2
  }
  if (currentPos < ARM_END_POS) {
    switchServo.writeMicroseconds(ARM_ALMOST_END_POS);
  }

  delay(200);
  switchServo.writeMicroseconds(ARM_START_POS);
}
