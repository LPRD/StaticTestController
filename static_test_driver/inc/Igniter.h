class Igniter
{
 private:
    long ignition_time;
    struct gpiod_chip *chip;
	struct gpiod_line *line;

 public:
    Igniter(int pin);

    ~Igniter();

    void handle_igniter ();

    void reset_igniter (); 

    void fire_igniter ();
};