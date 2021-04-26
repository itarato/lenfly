#pragma once

#include <cmath>
#include <cstdint>

#include "raylib.h"

#define GRAVITY_FALLBACK_THRESHOLD 1
#define GRAVITY_ACC 1.4f
#define GRAVITY_ACC_MIN -3.0f
#define GRAVITY_ACC_MAX 3.0f

const double pi = std::acos(-1);

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

struct Wiggle {
  bool enabled;
  float size;
  uint32_t counter_sin;
  uint32_t counter_cos;
  uint32_t length;

  Wiggle();

  float update1();
  float update2();
  void init(float size, uint32_t length);
};
