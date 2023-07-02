#include "Igniter.h"

Igniter::Igniter(int pint){
  this.pin = pin;
}

void Igniter::fire_igniter() {
  Serial.println("Firing igniter");
  digitalWrite(IGNITER_PIN, 1);
  ignition_time = millis();
  ignitor_active = true;
}

void Igniter::reset_igniter() {
  Serial.println("Reset igniter");
  digitalWrite(IGNITER_PIN, 0);
  ignitor_active = false;
}

void Igniter::handle_igniter() {
  if (ignitor_active && millis() > ignition_time + IGNITER_DURATION) {
    reset_igniter();
  }
}
