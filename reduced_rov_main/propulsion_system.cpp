/*
Simple ROV - SEA TURTLE
Last Updated:             2025-03-16
Submit pull request to:   Bob - bobchim@protonmail.com
*/

#include "propulsion_system.h"

PropulsionSystem::PropulsionSystem(int n, Motor *thrusters,
                                   int *x_dir, int *y_dir, int *z_dir,
                                   int *roll_dir, int *pitch_dir, int *yaw_dir)
  : n_(n), thrusters_(thrusters),
    x_dir_(x_dir), y_dir_(y_dir), z_dir_(z_dir),
    roll_dir_(roll_dir), pitch_dir_(pitch_dir), yaw_dir_(yaw_dir) {}

void PropulsionSystem::Drive(int vx, int vy, int vz, int vroll, int vpitch, int vyaw) {
  for (int i = 0; i <= n_; i++) {
    int speed = vx * x_dir_[i] + vy * y_dir_[i] + vz * z_dir_[i] + vroll * roll_dir_[i] + vpitch * pitch_dir_[i] + vyaw * yaw_dir_[i];
    thrusters_[i].Spin(speed);
  }
}

void PropulsionSystem::Attach() {
  for (int i = 0; i <= n_; i++) {
    thrusters_[i].Attach();
  }
}

void PropulsionSystem::Reset() {
  for (int i = 0; i <= n_; i++) {
    thrusters_[i].Reset();
  }
}