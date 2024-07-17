#pragma once

enum TB6612FNGChannel {
  A = 1 << 0,
  B = 1 << 1,
};

struct TB6612FNG {
  int pin_stby;
  int pin_ap; 
  int pin_a1; 
  int pin_a2; 
  int pin_bp; 
  int pin_b1; 
  int pin_b2; 

  int enabled_channel;
  bool error = false;
  bool debug = false;
};

void tb6612fng_init(TB6612FNG *self);

/// @brief
/// @param self 
/// @param channel 
/// @param value -255 ~ 255
void tb6612fng_set(TB6612FNG *self, TB6612FNGChannel channel, int value);
