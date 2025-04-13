/*
Simple ROV - SEA TURTLE
Last Updated:             2025-03-16
Submit pull request to:   Bob - bobchim@protonmail.com
*/

#ifndef PROPULSION_SYSTEM_H
#define PROPULSION_SYSTEM_H
#include <Arduino.h>
#include "motor.h"

class PropulsionSystem {
private:
  int n_;
  Motor *thrusters_;
  // boolean arrays define direction of motion based on motor config
  // x is forward, y is rightward, z is upward, roll pitch yaw is clockwise rotation around x y z
  int *x_dir_, *y_dir_, *z_dir_, *roll_dir_, *pitch_dir_, *yaw_dir_;

public:
  PropulsionSystem(int n, Motor *thrusters,
                   int *x_dir, int *y_dir, int *z_dir,
                   int *roll_dir, int *pitch_dir, int *yaw_dir);
  void Drive(int vx, int vy, int vz, int vroll, int vpitch, int vyaw);
  void Attach();
  void Reset();
};
#endif  // PROPULSION_SYSTEM_H