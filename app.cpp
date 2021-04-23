#include "app.h"

#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <utility>

#include "common.h"
#include "raylib.h"
#include "screen_objects.h"

#define PLANE_MOVE_V 10

#define MAX_CLOUDS 5
// Chance in percentage [0..100).
#define CLOUD_CREATION_CHANCE 5
#define CLOUD_V_MIN 4.0f
#define CLOUD_V_MAX 20.0f

#define VEGETATION_SPEED 10
#define MOUNTAIN_SPEED 2

#define MAX_BERRY 4
#define BERRY_V 10.0f
// Chance in percentage [0..100).
#define BERRY_CREATION_CHANCE 3

#define MAX_POOPS 2
#define POOP_V 6.0f
#define POOP_CREATION_CHANCE 1

#define SCORE_BERRY 10
#define SCORE_BERRY_MISS 0
#define SCORE_POOP -30

#define IMG_VEGETATION "resources/images/vegetation.png"
#define IMG_MOUNTAINS "resources/images/mountains.png"
#define IMG_PLANE "resources/images/plane.png"
#define IMG_CLOUD "resources/images/cloud.png"
#define IMG_BERRY "resources/images/berry.png"
#define IMG_POOP "resources/images/poop.png"

#define CTRL_SIZE 0.06f
#define CTRL_MAX_SPEED 13.0f

App::App() : vegetation(VEGETATION_SPEED), mountains(MOUNTAIN_SPEED) {
  SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(SCREEN_W, SCREEN_H, "SECRET");

  InitAudioDevice();

  background_music = LoadMusicStream("resources/audio/track.xm");
  SetMusicVolume(background_music, 0.1f);
  PlayMusicStream(background_music);

  engine_sound = LoadMusicStream("resources/audio/engine.mp3");
  SetMusicVolume(engine_sound, 0.2f);
  PlayMusicStream(engine_sound);

  SetTargetFPS(60);

  textures.insert({"vegetation", LoadTexture(IMG_VEGETATION)});
  textures.insert({"mountains", LoadTexture(IMG_MOUNTAINS)});
  textures.insert({"plane", LoadTexture(IMG_PLANE)});
  textures.insert({"cloud", LoadTexture(IMG_CLOUD)});
  textures.insert({"berry", LoadTexture(IMG_BERRY)});
  textures.insert({"poop", LoadTexture(IMG_POOP)});

  sounds.insert({"plop", LoadSound("resources/audio/bogyo.mp3")});
  SetSoundVolume(sounds["plop"], 1.0f);
  sounds.insert({"ouch", LoadSound("resources/audio/oh.mp3")});
  SetSoundVolume(sounds["ouch"], 0.8f);

  srand(time(NULL));

  plane.init(&textures["plane"]);

  vegetation.init(&textures["vegetation"]);
  mountains.init(&textures["mountains"]);

  last_touch.reset();

  reset();
}

void App::reset() {
  score = 0;
  init_menu_state();
}

void App::run() {
  while (!WindowShouldClose()) {
    handle_state();

    HideCursor();

    BeginDrawing();

    {
      ClearBackground(RAYWHITE);

      draw();
    }

    EndDrawing();
  }
}

void App::handle_state() {
  int touch_count = GetTouchPointsCount();

  if (state == STATE_MENU) {
    if (IsKeyPressed(KEY_ENTER)) {
      init_game_state();
    }

    if (touch_count > 0) {
      Vector2 current_touch = GetTouchPosition(0);

      int play_title_width = MeasureText("PLAY", 64);
      Rectangle play_button_rect{
          (float)(GetScreenWidth() >> 1) - (play_title_width >> 1),
          (float)(GetScreenHeight() >> 1) - 32, (float)play_title_width, 64.0f};

      if (CheckCollisionPointRec(current_touch, play_button_rect)) {
        init_game_state();
      }
    }
  } else {
    UpdateMusicStream(background_music);
    UpdateMusicStream(engine_sound);

    if (IsKeyPressed(KEY_R)) reset();
    if (IsKeyDown(KEY_UP)) plane.entity.pos.y -= PLANE_MOVE_V;
    if (IsKeyDown(KEY_DOWN)) plane.entity.pos.y += PLANE_MOVE_V;
    if (IsKeyDown(KEY_LEFT)) plane.entity.pos.x -= PLANE_MOVE_V;
    if (IsKeyDown(KEY_RIGHT)) plane.entity.pos.x += PLANE_MOVE_V;
    if (IsKeyPressed(KEY_G)) gravity_enabled = !gravity_enabled;
    if (IsKeyPressed(KEY_M)) mouse_enabled = !mouse_enabled;

    if (gravity_enabled) {
      if (IsKeyDown(KEY_SPACE)) plane.gravity.boost(10.0f);
      plane.entity.pos.y += plane.gravity.update();
    }

    if (mouse_enabled) {
      if (touch_count == 0) {
        last_touch.reset();
      } else {
        Vector2 current_touch = GetTouchPosition(0);

        if (!last_touch.has_value()) {
          last_touch = current_touch;
        } else {
          plane.entity.pos.x += std::max(
              -CTRL_MAX_SPEED,
              std::min(CTRL_MAX_SPEED,
                       (current_touch.x - last_touch.value().x) / 4.0f));
          plane.entity.pos.y += std::max(
              -CTRL_MAX_SPEED,
              std::min(CTRL_MAX_SPEED,
                       (current_touch.y - last_touch.value().y) / 4.0f));
        }

        plane.entity.pos.x = std::max(0, (int)plane.entity.pos.x);
        plane.entity.pos.y = std::max(0, (int)plane.entity.pos.y);
        plane.entity.pos.x = std::min(GetScreenWidth() - plane.texture->width,
                                      (int)plane.entity.pos.x);
        plane.entity.pos.y = std::min(GetScreenHeight() - plane.texture->height,
                                      (int)plane.entity.pos.y);
      }
    }

    if (clouds.size() < MAX_CLOUDS) {
      if (rand() % 100 < CLOUD_CREATION_CHANCE) {
        Cloud cloud(
            -(float)((rand() % (int)(CLOUD_V_MAX - CLOUD_V_MIN)) + CLOUD_V_MIN),
            &textures["cloud"]);
        clouds.push_back(std::move(cloud));
      }
    }

    clouds.erase(std::remove_if(clouds.begin(), clouds.end(),
                                [](auto cloud) { return cloud.should_die(); }),
                 clouds.end());

    if (berries.size() < MAX_BERRY) {
      if (rand() % 100 < BERRY_CREATION_CHANCE) {
        ConsumableItem new_berry(-BERRY_V, &textures["berry"]);
        berries.push_back(std::move(new_berry));
      }
    }

    if (poops.size() < max_poop()) {
      if (rand() % 100 < POOP_CREATION_CHANCE) {
        ConsumableItem new_poop(-POOP_V, &textures["poop"]);
        poops.push_back(std::move(new_poop));
      }
    }

    Rectangle plane_rect{plane.entity.pos.x, plane.entity.pos.y,
                         (float)plane.texture->width,
                         (float)plane.texture->height};

    for (auto& berry : berries) {
      Rectangle berry_rect{berry.entity.pos.x, berry.entity.pos.y,
                           (float)berry.texture->width,
                           (float)berry.texture->height};

      if (CheckCollisionRecs(plane_rect, berry_rect)) {
        score += SCORE_BERRY;
        berry.consume();
        StopSound(sounds["plop"]);
        PlaySound(sounds["plop"]);
      }

      if (berry.out_of_screen()) {
        score += SCORE_BERRY_MISS;
      }
    }

    for (auto& poop : poops) {
      Rectangle poop_rect{poop.entity.pos.x, poop.entity.pos.y,
                          (float)poop.texture->width,
                          (float)poop.texture->height};

      if (CheckCollisionRecs(plane_rect, poop_rect)) {
        score += SCORE_POOP;
        poop.consume();
        plane.penalty_color.activate();
        StopSound(sounds["ouch"]);
        PlaySound(sounds["ouch"]);
        life_count--;
      }
    }

    plane.penalty_color.update();

    berries.erase(
        std::remove_if(berries.begin(), berries.end(),
                       [](const auto& berry) { return berry.should_die(); }),
        berries.end());
    poops.erase(
        std::remove_if(poops.begin(), poops.end(),
                       [](const auto& poop) { return poop.should_die(); }),
        poops.end());

    if (life_count < 0) {
      init_menu_state();
    }
  }
}

void App::draw() {
  if (state == STATE_MENU) {
    int play_title_width = MeasureText("PLAY", 64);
    DrawText("PLAY", (GetScreenWidth() >> 1) - (play_title_width >> 1),
             (GetScreenHeight() >> 1) - 32, 64, DARKGRAY);

    if (score != 0) {
      const char* score_title = TextFormat("Score: %d", score);
      int score_title_width = MeasureText(score_title, 64);
      DrawText(score_title, (GetScreenWidth() >> 1) - (score_title_width >> 1),
               (GetScreenHeight() >> 1) + 128, 64, DARKGRAY);
    }
  } else if (state == STATE_GAME) {
    mountains.draw_and_move(GetScreenHeight() - (mountains.texture->height));
    vegetation.draw_and_move(GetScreenHeight() -
                             (vegetation.texture->height / 4 * 3));

    for (Cloud& cloud : clouds) {
      DrawTexture(*cloud.texture, cloud.entity.pos.x, cloud.entity.pos.y,
                  Fade(WHITE, cloud.fade));
      cloud.update();
    }

    DrawTexture(*plane.texture, plane.entity.pos.x, plane.entity.pos.y,
                plane.penalty_color.tint());

    for (auto& berry : berries) {
      DrawTexture(*berry.texture, berry.entity.pos.x, berry.entity.pos.y,
                  WHITE);
      berry.update();
    }
    for (auto& poop : poops) {
      DrawTexture(*poop.texture, poop.entity.pos.x, poop.entity.pos.y, WHITE);
      poop.update();
    }

    DrawFPS(GetScreenWidth() - 128, 8);

    DrawText(TextFormat("Score: %d | Life: %d", score, life_count), 8, 8, 64,
             DARKGRAY);
  }
}

App::~App() {
  for (auto& [key, texture] : textures) {
    UnloadTexture(texture);
  }

  for (auto& [key, sound] : sounds) {
    UnloadSound(sound);
  }

  UnloadMusicStream(background_music);
  UnloadMusicStream(engine_sound);

  CloseAudioDevice();
  CloseWindow();
}

int App::max_poop() { return score / 100 + MAX_POOPS; }

void App::init_game_state() {
  plane.reset();

  gravity_enabled = false;
#if defined(PLATFORM_ANDROID)
  mouse_enabled = true;
#else
  mouse_enabled = false;
#endif

  life_count = 3;
  score = 0;

  state = STATE_GAME;
}

void App::init_menu_state() { state = STATE_MENU; }
