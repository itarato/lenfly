#pragma once

#include "behaviour.h"
#include "entity.h"
#include "raylib.h"

struct Cloud {
 public:
  Entity entity;
  Texture2D* texture;
  float fade;

  Cloud(float vx, Texture2D* texture);
  ~Cloud();

  void update();
  bool should_die();
};

struct Plane {
 private:
  int shield;

 public:
  Texture2D* texture;
  Entity entity;
  Gravity gravity;
  PenaltyColor penalty_color;
  bool gravity_enabled;

  Plane();
  ~Plane();
  void reset();
  void init(Texture2D* texture);
  Rectangle rect();
  bool shielded();
  void update();
  Color shield_color();
  void shield_enable();
};

struct Background {
 public:
  Entity entity;
  Texture2D* texture;

  Background(float vx);
  ~Background();

  void init(Texture2D* texture);
  void draw_and_move(int pos_y);
};

struct ConsumableItem {
 private:
  Color color;

 public:
  Entity entity;
  Texture2D* texture;
  bool consumed;
  int flags;

  ConsumableItem(Vector2 v, Texture2D* texture);
  ~ConsumableItem();

  void update();
  bool should_die() const;
  bool out_of_screen() const;
  void consume();
  Color get_color();
  void set_color(Color);
  Rectangle rect();
};
