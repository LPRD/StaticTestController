#include "Servo.h"

class ServoArm {
  public:
    //TODO: Try to make this private
    Servo servo;
    
    ServoArm(int pin);
    
    void extend();

    void retract();
};