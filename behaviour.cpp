#include "behaviour.h"

#include <algorithm>
#include <cmath>

#include "raylib.h"

Gravity::Gravity() : v(GRAVITY_FALLBACK_THRESHOLD), acc(GRAVITY_ACC) {}

float Gravity::update() {
  if (fabs(v) < GRAVITY_FALLBACK_THRESHOLD) {
    v = GRAVITY_FALLBACK_THRESHOLD;
  } else if (v < 0.0) {
    v /= acc;
  } else {
    v = std::min(GRAVITY_ACC_MAX, v * acc);
  }

  return v;
}

void Gravity::boost(float val) { v = std::max(GRAVITY_ACC_MIN, v - val); }

PenaltyColor::PenaltyColor() : counter(0) {}

void PenaltyColor::activate() { counter = 200; }

void PenaltyColor::update() {
  if (counter <= 0) return;

  counter--;
}

Color PenaltyColor::tint() {
  if (counter <= 0) return WHITE;

  unsigned char redness =
      (unsigned char)(sin((double)counter / 2.0) * 127 + 127);

  unsigned char alpha = (unsigned char)(sin((double)counter / 2.0) * 64 + 191);

  return Color{255, redness, redness, alpha};
}

void PenaltyColor::reset() { counter = 0; }

Wiggle::Wiggle()
    : enabled(false), size(0.0f), counter_sin(0), counter_cos(0), length(0) {}

void Wiggle::init(float _size, uint32_t _length) {
  enabled = true;
  size = _size;
  length = _length;
}

float Wiggle::update1() {
  static const double step = (pi * 2) / length;

  float wiggle = sin(step * counter_sin);
  counter_sin = (counter_sin + 1) % length;

  return wiggle * size;
}

float Wiggle::update2() {
  static const double step = (pi * 2) / length;

  float wiggle = cos(step * counter_cos);
  counter_cos = (counter_cos + 1) % length;

  return wiggle * size;
}
