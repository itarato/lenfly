#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "behaviour.h"
#include "entity.h"
#include "raylib.h"
#include "screen_objects.h"

class App {
 private:
  Music background_music;
  Music engine_sound;
  Plane plane;
  Background vegetation;
  Background mountains;
  bool gravity_enabled;
  bool mouse_enabled;
  bool debug_window_enabled;
  int score;
  std::optional<Vector2> last_touch;

  std::map<std::string, Texture2D> textures;

  std::vector<Cloud> clouds;
  std::vector<ConsumableItem> berries;
  std::vector<ConsumableItem> poops;

  std::map<std::string, Sound> sounds;

  void draw();
  void handle_state();

 public:
  App();
  ~App();

  void run();
  void reset();
};
