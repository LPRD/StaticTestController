class Igniter
{
 private:
    long ignition_time;
    struct gpiod_chip *chip;
	struct gpiod_line *line;

 public:
    Igniter(int pin);

    void Igniter::handle_igniter ();

    void Igniter::reset_igniter (); 

    void Igniter::fire_igniter ();
};