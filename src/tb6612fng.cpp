#include <Arduino.h>

#include "tb6612fng.h"
#include "common.h"

void tb6612fng_init(TB6612FNG *self) {
  pinMode(self->pin_stby, OUTPUT);
  if ((self->enabled_channel & TB6612FNGChannel::A) == TB6612FNGChannel::A) {
    if (!digitalPinHasPWM(self->pin_ap)) {
      if (self->debug)
        Serial.println("Error pin_ap is not PWM pin");
      self->error = true;
      return;
    }
    pinMode(self->pin_ap, OUTPUT);
    pinMode(self->pin_a1, OUTPUT);
    pinMode(self->pin_a2, OUTPUT);
  }

  if ((self->enabled_channel & TB6612FNGChannel::B) == TB6612FNGChannel::B) {
    if (!digitalPinHasPWM(self->pin_bp)) {
      if (self->debug)
        Serial.println("Error pin_bp is not PWM pin");
      self->error = true;
      return;
    }
    pinMode(self->pin_bp, OUTPUT);
    pinMode(self->pin_b1, OUTPUT);
    pinMode(self->pin_b2, OUTPUT);
  }

  digitalWrite(self->pin_stby, HIGH);
}

void tb6612fng_set(TB6612FNG *self, TB6612FNGChannel channel, int value) {
  if (!(channel & self->enabled_channel)) return;

  PinStatus in1 = value >= 0 ? HIGH : LOW;
  PinStatus in2 = value >= 0 ? LOW : HIGH;

  value = abs(value);
  value = min(value, self->max);
  value = min(value, 255);

  if (channel == TB6612FNGChannel::A) {
    digitalWrite(self->pin_a1, in1);
    digitalWrite(self->pin_a2, in2);
    analogWrite(self->pin_ap, value);
    if (self->debug)
      Serial.print("A: ");
  }

  if (channel == TB6612FNGChannel::B) {
    digitalWrite(self->pin_b1, in1);
    digitalWrite(self->pin_b2, in2);
    analogWrite(self->pin_bp, value);
    if (self->debug)
      Serial.print("B: ");
  }

  if (self->debug) {
    Serial.print(in1);
    Serial.print(", ");
    Serial.print(in2);
    Serial.print(", ");
    Serial.print(value);
    Serial.println("");
  }
}
