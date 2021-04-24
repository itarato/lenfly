#pragma once

#include <cmath>

#include "raylib.h"

#define GRAVITY_FALLBACK_THRESHOLD 1
#define GRAVITY_ACC 1.4f
#define GRAVITY_ACC_MIN -3.0f
#define GRAVITY_ACC_MAX 3.0f

struct Gravity {
  float v;
  float acc;

  float update();
  void boost(float);
  Gravity();
};

struct PenaltyColor {
  int counter;
  void activate();
  PenaltyColor();
  void update();
  Color tint();
  void reset();
};
