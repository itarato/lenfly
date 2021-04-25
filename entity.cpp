#include "entity.h"

std::atomic<long long> counter(0);

Entity::Entity() {
  id = counter.fetch_add(1);

  pos.x = 0.0f;
  pos.y = 0.0f;

  v.x = 0.0f;
  v.y = 0.0f;
}

Entity::~Entity() {}
