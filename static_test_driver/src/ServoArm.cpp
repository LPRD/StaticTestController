#include "Servo.h"

ServoArm::ServoArm(int pin){
    servo.attach(pin);
}

void ServoArm::retract(){
    servo.write(90);              // move down. Part way to avoid servo weirdness
    servo.write(175);             // overshoot
    servo.write(165);             // final position
}

void ServoArm::extend(){
    servo.write(0);               // move servo arm up into rocket
}