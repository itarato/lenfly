#include "entity.h"

std::atomic<long long> counter(0);

Entity::Entity() { id = counter.fetch_add(1); }

Entity::~Entity() {}
