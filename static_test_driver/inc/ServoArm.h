class ServoArm
{
 private:
    Servo servo;
    
 public:
    ServoArm(int pin):servo() {}
    
    void extend();

    void retract();
};