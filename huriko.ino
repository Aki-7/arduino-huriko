#include <Wire.h>

#include "bmx055.h"
#include "tb6612fng.h"
#include "common.h"

#define ADDR_ACCL 0x19 // (JP1,JP2,JP3 = Open)
#define ADDR_GYRRO 0x69 // (JP1,JP2,JP3 = Open)
#define ADDR_MAG 0x13 // (JP1,JP2,JP3 = Open)

struct BMX055::BMX055 bmx;
struct TB6612FNG motor;

float V_IN = 5.00;
float V_MAX = 3.00;

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
  motor.max = 255 * V_MAX / V_IN;
  motor.enabled_channel = TB6612FNGChannel::A | TB6612FNGChannel::B;
  tb6612fng_init(&motor);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(3000);
  digitalWrite(13, LOW);

  bmx055Gyro(&bmx);
}

float theta = 0;
float v;
float x;

                       // static / swing
float k_theta = 3.80;  // 4.08   / 3.80
float k_gyro  = 50.0;  // 37.5   / 50.0
float k_v     = 470;   // 500    / 470
float k_x     = 50;   
float k_power = 0.99; 

float prev_power;
bool running = true;
int overflow_count = 0;

void loop() {
  if (!running) return;

  bmx055Gyro(&bmx);
  float gyro = bmx.gyro.y;
  theta += gyro;

  float power = theta * k_theta + gyro * k_gyro + v * k_v + x * k_x;
  power *= V_MAX / V_IN / 100;
  power = clamp(power, -motor.max, motor.max);
  v += power / 100;
  x += v / 1000;

  float out = power * k_power;
  if (prev_power * power < 0) {
    if (power < 0) out = -motor.max;
    else out = motor.max;
  }

  if (abs(power) >= motor.max) {
    overflow_count ++;
    if (overflow_count > 50) {
      running = false;
      out = 0;
    }
  } else {
    overflow_count = 0;
  }

  tb6612fng_set(&motor, TB6612FNGChannel::A, out);
  tb6612fng_set(&motor, TB6612FNGChannel::B, -out);

  prev_power = power;
}
