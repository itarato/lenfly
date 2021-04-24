#pragma once

#include <atomic>

#include "common.h"
#include "raylib.h"

class Entity {
 public:
  Point pos;
  Vector2 v;

  long long id;

  Entity();
  ~Entity();
};
