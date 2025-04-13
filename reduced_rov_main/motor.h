/*
Simple ROV - SEA TURTLE
Last Updated:             2025-03-16
Submit pull request to:   Bob - bobchim@protonmail.com
*/

#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>
#include <Servo.h>

class Motor {
private:
  int pin_;
  int input_low_ = -128;
  int input_high_ = 127;
  int output_low_ = 1000;
  int output_high_ = 2000;
  bool soft_ = 1;
  Servo m;

public:
  Motor(int pin, int input_low, int input_high, int output_low, int output_high, bool soft);
  void Spin(int speed);
  void Attach();
  void Reset();
  int getMicroseconds();
  int getSpeed();
  int current_speed;
};
#endif  // MOTOR_H