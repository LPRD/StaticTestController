/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int val = 0;    // variable to read the value from the analog pin

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(val);
  delay(10000);
}

void loop() {

  myservo.write(val);                  // sets the servo position according to the scaled value
  delay(1);                           // waits for the servo to get there
  val = val + 2.5;
  if(val > 170 && val < 175)
  {
    val = 170;
  }

}
