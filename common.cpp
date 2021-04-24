#include "common.h"

#include "stdlib.h"

bool has_chance(int chance) { return rand() % 100 <= (chance - 1); }
