/*
Simple ROV - SEA TURTLE
Last Updated:             2025-03-16
Submit pull request to:   Bob - bobchim@protonmail.com
*/

#include "motor.h"

Motor::Motor(int pin, int input_low, int input_high, int output_low, int output_high, bool soft)
  : pin_(pin), input_low_(input_low), input_high_(input_high), output_low_(output_low), output_high_(output_high), soft_(soft) {}

void Motor::Attach() {
  m.attach(pin_);
}

void Motor::Spin(int speed) {
  speed = constrain(speed, input_low_, input_high_);
  if (soft_ == 1) {
    current_speed += speed / 2;
    current_speed = current_speed / 3 * 2;
  } else {
    current_speed = speed;
  }
  int mapped_speed = map(current_speed, input_low_, input_high_, output_low_, output_high_);
  m.writeMicroseconds(mapped_speed);
}

void Motor::Reset() {
  Spin(0);
}

int Motor::getMicroseconds() {
  return m.readMicroseconds();
}

int Motor::getSpeed() {
  return current_speed;
}