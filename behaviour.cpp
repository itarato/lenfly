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