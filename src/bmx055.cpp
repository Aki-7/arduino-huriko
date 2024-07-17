//================================================================//
//  AE-BMX055             Arduino UNO                             //
//    VCC                    +5V                                  //
//    GND                    GND                                  //
//    SDA                    A4(SDA)                              //
//    SCL                    A5(SCL)                              //
//                                                                //
//   (JP4,JP5,JP6はショートした状態)                                //
//   http://akizukidenshi.com/catalog/g/gK-13010/                 //
//================================================================//

#include <Wire.h>
#include <Arduino.h>

#include "bmx055.h"

void bmx055Init(struct BMX055 *self) {
  if (self->addr_accl != 0) {
    Wire.beginTransmission(self->addr_accl);
    Wire.write(0x0F); // Select PMU_Range register
    Wire.write(0x03); // Range = +/- 2g
    Wire.endTransmission();
    delay(100);
    //------------------------------------------------------------//
    Wire.beginTransmission(self->addr_accl);
    Wire.write(0x10); // Select PMU_BW register
    Wire.write(0x08); // Bandwidth = 7.81 Hz
    Wire.endTransmission();
    delay(100);
    //------------------------------------------------------------//
    Wire.beginTransmission(self->addr_accl);
    Wire.write(0x11); // Select PMU_LPW register
    Wire.write(0x00); // Normal mode, Sleep duration = 0.5ms
    Wire.endTransmission();
    delay(100);
  }

  if (self->addr_gyro != 0) {
    Wire.beginTransmission(self->addr_gyro);
    Wire.write(0x0F); // Select Range register
    Wire.write(0x04); // Full scale = +/- 125 degree/s
    Wire.endTransmission();
    delay(100);
    //------------------------------------------------------------//
    Wire.beginTransmission(self->addr_gyro);
    Wire.write(0x10); // Select Bandwidth register
    Wire.write(0x07); // ODR = 100 Hz
    Wire.endTransmission();
    delay(100);
    //------------------------------------------------------------//
    Wire.beginTransmission(self->addr_gyro);
    Wire.write(0x11); // Select LPM1 register
    Wire.write(0x00); // Normal mode, Sleep duration = 2ms
    Wire.endTransmission();
    delay(100);
  }

  if (self->addr_mag != 0) {
    Wire.beginTransmission(self->addr_mag);
    Wire.write(0x4B); // Select Mag register
    Wire.write(0x83); // Soft reset
    Wire.endTransmission();
    delay(100);
    //------------------------------------------------------------//
    Wire.beginTransmission(self->addr_mag);
    Wire.write(0x4B); // Select Mag register
    Wire.write(0x01); // Soft reset
    Wire.endTransmission();
    delay(100);
    //------------------------------------------------------------//
    Wire.beginTransmission(self->addr_mag);
    Wire.write(0x4C); // Select Mag register
    Wire.write(0x00); // Normal Mode, ODR = 10 Hz
    Wire.endTransmission();
    //------------------------------------------------------------//
    Wire.beginTransmission(self->addr_mag);
    Wire.write(0x4E); // Select Mag register
    Wire.write(0x84); // X, Y, Z-Axis enabled
    Wire.endTransmission();
    //------------------------------------------------------------//
    Wire.beginTransmission(self->addr_mag);
    Wire.write(0x51); // Select Mag register
    Wire.write(0x04); // No. of Repetitions for X-Y Axis = 9
    Wire.endTransmission();
    //------------------------------------------------------------//
    Wire.beginTransmission(self->addr_mag);
    Wire.write(0x52); // Select Mag register
    Wire.write(16);   // No. of Repetitions for Z-Axis = 15
    Wire.endTransmission();
  }
}

void bmx055Accl(struct BMX055 *self) {
  if (self->addr_accl == 0) return;

  unsigned int data[6];
  for (int i = 0; i < 6; i++) {
    Wire.beginTransmission(self->addr_accl);
    Wire.write((2 + i)); // Select data register
    Wire.endTransmission();
    Wire.requestFrom(self->addr_accl, 1); // Request 1 byte of data
    // Read 6 bytes of data
    // xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
    if (Wire.available() == 1)
      data[i] = Wire.read();
  }
  // Convert the data to 12-bits
  float x = ((data[1] * 256) + (data[0] & 0xF0)) / 16;
  if (x > 2047)
    x -= 4096;
  float y = ((data[3] * 256) + (data[2] & 0xF0)) / 16;
  if (y > 2047)
    y -= 4096;
  float z = ((data[5] * 256) + (data[4] & 0xF0)) / 16;
  if (z > 2047)
    z -= 4096;
  x = x * 0.00098; // range = +/-2g
  y = y * 0.00098; // range = +/-2g
  z = z * 0.00098; // range = +/-2g

  self->accl.x = x;
  self->accl.y = y;
  self->accl.z = z;
}

void bmx055Gyro(struct BMX055 *self) {
  if (self->addr_gyro == 0) return;

  unsigned int data[6];
  for (int i = 0; i < 6; i++) {
    Wire.beginTransmission(self->addr_gyro);
    Wire.write((2 + i)); // Select data register
    Wire.endTransmission();
    Wire.requestFrom(self->addr_gyro, 1); // Request 1 byte of data
    // Read 6 bytes of data
    // xGyro lsb, xGyro msb, yGyro lsb, yGyro msb, zGyro lsb, zGyro msb
    if (Wire.available() == 1)
      data[i] = Wire.read();
  }
  // Convert the data
  float x = (data[1] * 256) + data[0];
  if (x > 32767)
    x -= 65536;
  float y = (data[3] * 256) + data[2];
  if (y > 32767)
    y -= 65536;
  float z = (data[5] * 256) + data[4];
  if (z > 32767)
    z -= 65536;

  x = x * 0.0038; //  Full scale = +/- 125 degree/s
  y = y * 0.0038; //  Full scale = +/- 125 degree/s
  z = z * 0.0038; //  Full scale = +/- 125 degree/s

  self->gyro.x = x;
  self->gyro.y = y;
  self->gyro.z = z;
}

void bmx055Mag(struct BMX055 *self) {
  if (self->addr_mag == 0) return;

  unsigned int data[8];
  for (int i = 0; i < 8; i++) {
    Wire.beginTransmission(self->addr_mag);
    Wire.write((0x42 + i)); // Select data register
    Wire.endTransmission();
    Wire.requestFrom(self->addr_mag, 1); // Request 1 byte of data
    // Read 6 bytes of data
    // xMag lsb, xMag msb, yMag lsb, yMag msb, zMag lsb, zMag msb
    if (Wire.available() == 1)
      data[i] = Wire.read();
  }
  // Convert the data
  int x = ((data[1] << 5) | (data[0] >> 3));
  if (x > 4095)
    x -= 8192;
  int y = ((data[3] << 5) | (data[2] >> 3));
  if (y > 4095)
    y -= 8192;
  int z = ((data[5] << 7) | (data[4] >> 1));
  if (z > 16383)
    z -= 32768;

  self->mag.x = x;
  self->mag.y = y;
  self->mag.z = z;
}
