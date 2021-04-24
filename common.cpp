#include "common.h"

#include "stdlib.h"

/**
 * Chance of a rand(0...1023) being less than equal to (chance - 1).
 */
bool has_chance(int chance) { return (rand() & 0x3ff) <= (chance - 1); }
