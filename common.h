#pragma once

#define ANDROID_BUTTON_TTL 8

#if defined(PLATFORM_ANDROID)

#define SCREEN_W 0
#define SCREEN_H 0

#elif defined(FULLSCREEN)

#define SCREEN_W 0
#define SCREEN_H 0

#else

#define SCREEN_W 1600
#define SCREEN_H 1200

#endif

bool has_chance(int chance);
