#pragma once

#include "vector.h"

struct BMX055 {
  int addr_accl = 0;
  int addr_gyro = 0;
  int addr_mag = 0;

  struct Vector3f accl;
  struct Vector3f gyro;
  struct Vector3i mag;
};

void bmx055Init(struct BMX055 *self);

void bmx055Accl(struct BMX055 *self);

void bmx055Gyro(struct BMX055 *self);

void bmx055Mag(struct BMX055 *self);
