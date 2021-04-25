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
#define CLOUD_CREATION_CHANCE 50
#define CLOUD_V_MIN 4.0f
#define CLOUD_V_MAX 20.0f

#define VEGETATION_SPEED 10
#define MOUNTAIN_SPEED 2

#define MAX_BERRY 4
#define BERRY_V 10.0f
// Chance in percentage [0..100).
#define BERRY_CREATION_CHANCE 30
#define BERRY_BONUS_CHANCE 50
#define BERRY_BURST 16
#define BERRY_BURST_CHANCE 300

#define CONSUMABLE_ITEM_FLAG_BERRY_BONUS 1

#define MAX_POOPS 2
#define POOP_V 6.0f
#define POOP_CREATION_CHANCE 30

#define SCORE_BERRY 10
#define SCORE_BERRY_MISS 0
#define SCORE_POOP -30

#define BOSS_FIGHT_SCORE_INIT 500
#define BOSS_FIGHT_SCORE_JUMP 500
#define BOSS_TREAT_V 20

#define CARROT_V 8
#define CARROT_CREATION_CHANCE 1

#define IMG_VEGETATION "resources/images/vegetation.png"
#define IMG_MOUNTAINS "resources/images/mountains.png"
#define IMG_PLANE "resources/images/plane.png"
#define IMG_CLOUD "resources/images/cloud.png"
#define IMG_BERRY "resources/images/berry.png"
#define IMG_POOP "resources/images/poop.png"
#define IMG_LIFE "resources/images/life.png"
#define IMG_RASPBERRY "resources/images/raspberry.png"
#define IMG_CARROT "resources/images/carrot.png"
#define IMG_BUBBLE "resources/images/bubble.png"
#define IMG_CHICKEN "resources/images/chicken.png"
#define IMG_BOSS "resources/images/boss.png"

#define CTRL_SIZE 0.06f
#define CTRL_MAX_SPEED 13.0f

#define LIFE_SCORE_JUMP 300;
#define LIFE_V 12

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
  textures.insert({"life", LoadTexture(IMG_LIFE)});
  textures.insert({"raspberry", LoadTexture(IMG_RASPBERRY)});
  textures.insert({"carrot", LoadTexture(IMG_CARROT)});
  textures.insert({"bubble", LoadTexture(IMG_BUBBLE)});
  textures.insert({"chicken", LoadTexture(IMG_CHICKEN)});
  textures.insert({"boss", LoadTexture(IMG_BOSS)});

  sounds.insert({"plop", LoadSound("resources/audio/bogyo.mp3")});
  SetSoundVolume(sounds["plop"], 1.0f);
  sounds.insert({"ouch", LoadSound("resources/audio/oh.mp3")});
  SetSoundVolume(sounds["ouch"], 0.8f);
  sounds.insert({"roar", LoadSound("resources/audio/roar.mp3")});
  SetSoundVolume(sounds["roar"], 0.8f);

  srand(time(NULL));

  plane.init(&textures["plane"]);

  vegetation.init(&textures["vegetation"]);
  mountains.init(&textures["mountains"]);

  last_touch.reset();
  life.reset();
  carrot.reset();

  boss.init(&textures["boss"]);
  boss.reset();

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
  }
  
  if (state == STATE_GAME) {
    UpdateMusicStream(background_music);
    UpdateMusicStream(engine_sound);

    if (IsKeyPressed(KEY_R)) reset();

    if (clouds.size() < MAX_CLOUDS) {
      if (has_chance(CLOUD_CREATION_CHANCE)) {
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
      if (has_chance(BERRY_CREATION_CHANCE)) {
        Texture2D* berry_texture = nullptr;
        if (has_chance(BERRY_BONUS_CHANCE)) {
          berry_texture = &textures["raspberry"];
        } else {
          berry_texture = &textures["berry"];
        }
        ConsumableItem new_berry({-BERRY_V, 0.0f}, berry_texture);
        if (berry_texture == &textures["raspberry"]) {
          new_berry.flags |= CONSUMABLE_ITEM_FLAG_BERRY_BONUS;
        }
        new_berry.set_color({(uint8_t)(255 - GetRandomValue(0, 100)),
                             (uint8_t)(255 - GetRandomValue(0, 100)),
                             (uint8_t)(255 - GetRandomValue(0, 100)), 255});
        berries.push_back(std::move(new_berry));
      }
    }

    if (berry_burst > 0 && has_chance(BERRY_BURST_CHANCE)) {
      ConsumableItem new_berry({-BERRY_V, 0.0f}, &textures["berry"]);
      new_berry.set_color(RED);
      berries.push_back(std::move(new_berry));
      berry_burst--;
    }

    if ((int)poops.size() < max_poop()) {
      if (has_chance(POOP_CREATION_CHANCE)) {
        ConsumableItem new_poop({-POOP_V, 0.0f}, &textures["poop"]);
        poops.push_back(std::move(new_poop));
      }
    }

    Rectangle plane_rect = plane.rect();

    for (auto& berry : berries) {
      if (CheckCollisionRecs(plane_rect, berry.rect())) {
        score += SCORE_BERRY;
        berry.consume();
        StopSound(sounds["plop"]);
        PlaySound(sounds["plop"]);

        if (berry.flags & CONSUMABLE_ITEM_FLAG_BERRY_BONUS) {
          berry_burst += BERRY_BURST;
        }
      }

      if (berry.out_of_screen()) {
        score += SCORE_BERRY_MISS;
      }
    }

    for (auto& poop : poops) {
      if (CheckCollisionRecs(plane_rect, poop.rect())) {
        poop.consume();

        if (!plane.shielded()) {
            score += SCORE_POOP;
            plane.penalty_color.activate();
            StopSound(sounds["ouch"]);
            PlaySound(sounds["ouch"]);
            life_count--;
        }
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

    {  // Life.
      if (score >= next_life_score) {
        next_life_score += LIFE_SCORE_JUMP;
        life = ConsumableItem({-LIFE_V, 0.0f}, &textures["life"]);
      }
      if (life.has_value()) {
        if (CheckCollisionRecs(plane_rect, life.value().rect())) {
          life.value().consume();
          life_count += 1;
          StopSound(sounds["roar"]);
          PlaySound(sounds["roar"]);
        }
        if (life.value().should_die()) {
          life.reset();
        }
      }
    }

    if (life_count < 0) {
      init_menu_state();
    }

    {  // Carrot.
      if (!carrot.has_value()) {
        if (has_chance(CARROT_CREATION_CHANCE)) {
          carrot = ConsumableItem({-CARROT_V, 0.0f}, &textures["carrot"]);
        }
      } else {
        if (CheckCollisionRecs(plane_rect, carrot.value().rect())) {
          plane.shield_enable();
          carrot.value().consume();
        }

        if (carrot.value().should_die()) {
          carrot.reset();
        }
      }
    }

    {  // Boss fight.
      if (score >= boss_fight_score) {
        boss_fight_score += BOSS_FIGHT_SCORE_JUMP;
        init_boss_state();
      }
    }
  }
  
  if (state == STATE_BOSS) {
    int gesture = GetGestureDetected();
    if (gesture == GESTURE_TAP || IsKeyPressed(KEY_SPACE)) {
      ConsumableItem boss_treat{{0.0f, BOSS_TREAT_V}, &textures["chicken"]};
      boss_treat.entity.pos.x = plane.entity.pos.x + plane.texture->width / 2;
      boss_treat.entity.pos.y = plane.entity.pos.y + plane.texture->height - 30;
      boss_treats.push_back(boss_treat);
    }

    boss_treats.erase(std::remove_if(boss_treats.begin(), boss_treats.end(),
                                     [](const auto& boss_treat) {
                                       return boss_treat.should_die();
                                     }),
                      boss_treats.end());

    for (auto& boss_treat : boss_treats) {
      if (CheckCollisionRecs(boss_treat.rect(), boss.rect())) {
        boss_treat.consume();
        boss.feed();
      }
    }

    if (boss.is_full()) {
      return_game_state();
    }
  }

  if (state == STATE_GAME || state == STATE_BOSS) {
    if (IsKeyDown(KEY_UP)) plane.entity.pos.y -= PLANE_MOVE_V;
    if (IsKeyDown(KEY_DOWN)) plane.entity.pos.y += PLANE_MOVE_V;
    if (IsKeyDown(KEY_LEFT)) plane.entity.pos.x -= PLANE_MOVE_V;
    if (IsKeyDown(KEY_RIGHT)) plane.entity.pos.x += PLANE_MOVE_V;
    if (IsKeyPressed(KEY_G)) plane.gravity_enabled = !plane.gravity_enabled;
    if (IsKeyPressed(KEY_M)) mouse_enabled = !mouse_enabled;
    
    if (plane.gravity_enabled) {
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
      }
    }

    {  // Keep plane inside the screen.
      plane.entity.pos.x = std::max(0, (int)plane.entity.pos.x);
      plane.entity.pos.y = std::max(0, (int)plane.entity.pos.y);
      plane.entity.pos.x = std::min(GetScreenWidth() - plane.texture->width,
                                    (int)plane.entity.pos.x);
      plane.entity.pos.y = std::min(GetScreenHeight() - plane.texture->height,
                                    (int)plane.entity.pos.y);
    }
  }

  if (state == STATE_BOSS) {
    plane.entity.pos.y = 128;
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
  }

  if (state == STATE_GAME || state == STATE_BOSS) {
    mountains.draw_and_move(GetScreenHeight() - (mountains.texture->height));
    vegetation.draw_and_move(GetScreenHeight() -
                             (vegetation.texture->height / 4 * 3));

    for (Cloud& cloud : clouds) {
      DrawTexture(*cloud.texture, cloud.entity.pos.x, cloud.entity.pos.y,
                  Fade(WHITE, cloud.fade));
      cloud.update();
    }
  }

  if (state == STATE_BOSS) {
    for (auto& boss_treat : boss_treats) {
      DrawTexture(*boss_treat.texture, boss_treat.entity.pos.x,
                  boss_treat.entity.pos.y, WHITE);
      boss_treat.update();
    }

    DrawTexture(*boss.texture, boss.entity.pos.x, boss.entity.pos.y, WHITE);
    boss.update();
  }

  if (state == STATE_GAME || state == STATE_BOSS) {
    {  // Plane.
      DrawTexture(*plane.texture, plane.entity.pos.x, plane.entity.pos.y,
                  plane.penalty_color.tint());
      if (plane.shielded()) {
        DrawTexture(textures["bubble"], plane.entity.pos.x - 15,
                    plane.entity.pos.y - 15, plane.shield_color());
      }
      plane.update();
    }
  }

  if (state == STATE_BOSS) {
    DrawRectangleLinesEx({32.0f, 32.0f, GetScreenWidth() - 64.0f, 48.0f}, 4,
                         DARKGRAY);
    DrawRectangle(40, 40, (GetScreenWidth() - 80) * boss.health(), 32, RED);
  }

  if (state == STATE_GAME) {
    for (auto& berry : berries) {
      DrawTexture(*berry.texture, berry.entity.pos.x, berry.entity.pos.y,
                  berry.get_color());
      berry.update();
    }
    for (auto& poop : poops) {
      DrawTexture(*poop.texture, poop.entity.pos.x, poop.entity.pos.y, WHITE);
      poop.update();
    }

    if (life.has_value()) {
      DrawTexture(*life.value().texture, life.value().entity.pos.x,
                  life.value().entity.pos.y, WHITE);
      life.value().update();
    }

    if (carrot.has_value()) {
      DrawTexture(*carrot.value().texture, carrot.value().entity.pos.x,
                  carrot.value().entity.pos.y, WHITE);
      carrot.value().update();
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
  return_game_state();
  score = 0;
  life_count = 3;
  next_life_score = LIFE_SCORE_JUMP;
  boss_fight_score = BOSS_FIGHT_SCORE_INIT;

#if defined(PLATFORM_ANDROID)
  mouse_enabled = true;
#else
  mouse_enabled = false;
#endif
}

void App::return_game_state() {
  plane.reset();

  state = STATE_GAME;
  life.reset();
  carrot.reset();
  berry_burst = 0;

  berries.clear();
  poops.clear();
}

void App::init_menu_state() { state = STATE_MENU; }

void App::init_boss_state() {
  state = STATE_BOSS;

  berries.clear();
  poops.clear();
  clouds.clear();

  life.reset();
  carrot.reset();
  plane.reset();

  boss.reset();
}
