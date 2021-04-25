#pragma once

#include "behaviour.h"
#include "entity.h"
#include "raylib.h"

struct BaseScreenObject {
  Entity entity;
  Texture2D* texture;

  BaseScreenObject();

  bool out_of_screen() const;
};

struct Cloud : public BaseScreenObject {
 public:
  float fade;

  Cloud(float vx, Texture2D* texture);
  ~Cloud();

  void update();
  bool should_die();
};

struct Plane : public BaseScreenObject {
 private:
  int shield;

 public:
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

struct Boss : public BaseScreenObject {
 private:
  int next_location;

 public:
  Boss();
  ~Boss();
  void init(Texture2D*);
  void update();
  int life;
  void reset();
  Rectangle rect();
  float health();
  void feed();
  bool is_full();
};

struct Background : public BaseScreenObject {
 public:
  Background(float vx);
  ~Background();

  void init(Texture2D* texture);
  void draw_and_move(int pos_y);
};

struct ConsumableItem : public BaseScreenObject {
 private:
  Color color;

 public:
  bool consumed;
  int flags;

  ConsumableItem(Vector2 v, Texture2D* texture);
  ~ConsumableItem();

  void update();
  bool should_die() const;
  void consume();
  Color get_color();
  void set_color(Color);
  Rectangle rect();
};
