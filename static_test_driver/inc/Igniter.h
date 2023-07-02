class Igniter
{
 private:
    long ignition_time;
    bool ignitor_active;
    int pin;

 public:
    Igniter(int pin);

    void Igniter::handle_igniter ();

    void Igniter::reset_igniter (); 

    void Igniter::fire_igniter ();
};