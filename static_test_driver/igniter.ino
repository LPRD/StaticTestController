#include "defs.h"

long ignition_time = 0;
bool ignitor_active = false;

void fire_igniter() {
  Serial.println("Firing igniter");
  digitalWrite(IGNITER_PIN, 1);
  ignition_time = millis();
  ignitor_active = true;
}

void reset_igniter() {
  Serial.println("Reset igniter");
  digitalWrite(IGNITER_PIN, 0);
  ignitor_active = false;
}

void handle_igniter() {
  if (ignitor_active && millis() > ignition_time + IGNITER_DURATION) {
    reset_igniter();
  }
}

//-------------------------------------------------------------------------------------------
//Ignition Arm Servo
//-------------------------------------------------------------------------------------------

//Sensor Device 5

void ServoArm::retract(){
    servo.write(90);              // move down. Part way to avoid servo weirdness
    servo.write(175);             // overshoot
    servo.write(165);             // final position
}

void ServoArm::extend(){
    servo.write(0);               // move servo arm up into rocket
}
