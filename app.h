#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "behaviour.h"
#include "entity.h"
#include "raylib.h"
#include "screen_objects.h"

typedef enum {
  STATE_MENU = 0,
  STATE_GAME = 1,
} GameState;

class App {
 private:
  Music background_music;
  Music engine_sound;
  Plane plane;
  Background vegetation;
  Background mountains;
  bool mouse_enabled;
  int score;
  std::optional<Vector2> last_touch;
  GameState state;
  int life_count;
  int next_life_score;
  int berry_burst;

  std::map<std::string, Texture2D> textures;

  std::vector<Cloud> clouds;
  std::vector<ConsumableItem> berries;
  std::vector<ConsumableItem> poops;
  std::optional<ConsumableItem> life;
  std::optional<ConsumableItem> carrot;

  std::map<std::string, Sound> sounds;

  void draw();
  void handle_state();
  int max_poop();
  void init_game_state();
  void init_menu_state();

 public:
  App();
  ~App();

  void run();
  void reset();
};
