#pragma once

#include <atomic>

#include "common.h"
#include "raylib.h"

class Entity {
 public:
  Vector2 pos;
  Vector2 v;

  long long id;

  Entity();
  ~Entity();
};
