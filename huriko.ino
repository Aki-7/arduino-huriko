#include <Wire.h>

#include "bmx055.h"
#include "tb6612fng.h"

#define ADDR_ACCL 0x19 // (JP1,JP2,JP3 = Open)
#define ADDR_GYRRO 0x69 // (JP1,JP2,JP3 = Open)
#define ADDR_MAG 0x13 // (JP1,JP2,JP3 = Open)

struct BMX055::BMX055 bmx;
struct TB6612FNG motor;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  bmx.addr_accl = ADDR_ACCL;
  bmx.addr_gyro = ADDR_GYRRO;
  bmx.addr_mag = ADDR_MAG;
  bmx055Init(&bmx);

  motor.pin_stby = 4;
  motor.pin_ap = 5;
  motor.pin_a2 = 6;
  motor.pin_a1 = 7;
  motor.pin_b1 = 8;
  motor.pin_b2 = 9;
  motor.pin_bp = 10;
  motor.enabled_channel = TB6612FNGChannel::A | TB6612FNGChannel::B;
  tb6612fng_init(&motor);

  pinMode(13, OUTPUT);
}

void loop() {
  bmx055Gyro(&bmx);
  tb6612fng_set(&motor, TB6612FNGChannel::B, bmx.gyro.x);
  tb6612fng_set(&motor, TB6612FNGChannel::A, -bmx.gyro.x);
}
