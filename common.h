#pragma once

#if defined(PLATFORM_ANDROID)

#define SCREEN_W 0
#define SCREEN_H 0

#else

#define SCREEN_W 1600
#define SCREEN_H 1200

#endif

typedef struct {
  float x;
  float y;
} Point;

bool has_chance(int chance);
