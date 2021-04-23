#pragma once

#include <atomic>

#include "common.h"

class Entity {
 public:
  Point pos;
  // Horizontal velocity.
  float vx;

  long long id;

  Entity();
  ~Entity();
};
