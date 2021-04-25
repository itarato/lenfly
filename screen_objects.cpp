#include "screen_objects.h"

#include <stdlib.h>

#include <algorithm>
#include <iostream>

#include "common.h"
#include "raylib.h"

#define SHIELD_LIFESPAN 400
#define SHIELD_END_WARNING 150

#define BOSS_LIFE 20
#define BOSS_V 8

BaseScreenObject::BaseScreenObject() { texture = nullptr; }

/// CLOUD /////////////////////////////////////////////////////////////////////

Cloud::Cloud(float vx, Texture2D* _texture) {
  texture = _texture;
  fade = (float)((rand() % 10) + 1) / 10.0f;
  entity.v = Vector2{vx, 0.0f};
  entity.pos.x = GetScreenWidth();
  entity.pos.y = (rand() % GetScreenHeight()) - (texture->height / 2);
}

Cloud::~Cloud() {}

void Cloud::update() { entity.pos.x += entity.v.x; }

bool Cloud::should_die() { return entity.pos.x < -texture->width; }

/// PLANE /////////////////////////////////////////////////////////////////////

Plane::Plane() {}

void Plane::init(Texture2D* _texture) {
  texture = _texture;
  reset();
}

void Plane::reset() {
  entity.pos.x = GetScreenWidth() / 3;
  entity.pos.y = GetScreenWidth() / 2 - texture->height;
  gravity.v = 0.0;
  penalty_color.reset();
  gravity_enabled = false;
  shield = 0;
}

Plane::~Plane() {}

Rectangle Plane::rect() {
  return Rectangle{entity.pos.x, entity.pos.y, (float)texture->width,
                   (float)texture->height};
}

void Plane::update() {
  if (shield > 0) shield--;
}

Color Plane::shield_color() {
  uint8_t alpha = 180;

  if (shield < SHIELD_END_WARNING && (shield / 10) % 2 == 0) {
    alpha = 100;
  }

  return Color{255, 255, 255, alpha};
}

bool Plane::shielded() { return shield > 0; }

void Plane::shield_enable() { shield = SHIELD_LIFESPAN; }

/// BOSS //////////////////////////////////////////////////////////////////////

Boss::Boss() {}

void Boss::init(Texture2D* _texture) {
  texture = _texture;
  reset();
}

Boss::~Boss() {}

void Boss::update() {
  if (abs(entity.pos.x - next_location) < BOSS_V) {
    next_location = GetRandomValue(0, GetScreenWidth() - texture->width);
  } else {
    entity.pos.x += (BOSS_V * ((entity.pos.x - next_location <= 0) ? 1 : -1));
  }
}

void Boss::reset() {
  life = BOSS_LIFE;
  entity.pos.x = GetScreenWidth() / 2;
  entity.pos.y = GetScreenHeight() - (texture->height / 5 * 4);
  next_location = (int)entity.pos.x;
}

Rectangle Boss::rect() {
  return Rectangle{entity.pos.x, entity.pos.y, (float)texture->width,
                   (float)texture->height};
}

void Boss::feed() {
  if (life > 0) life--;
}

bool Boss::is_full() { return life <= 0; }

float Boss::health() { return (float)life / BOSS_LIFE; }

/// MOVABLE IMAGE /////////////////////////////////////////////////////////////

void Background::init(Texture2D* _texture) { texture = _texture; }

Background::Background(float vx) {
  entity.pos.x = 0;
  entity.v = Vector2{vx, 0.0f};
}

Background::~Background() {}

void Background::draw_and_move(int pos_y) {
  if (entity.pos.x <= -texture->width) entity.pos.x = 0;

  DrawTexture(*texture, entity.pos.x, pos_y, WHITE);
  DrawTexture(*texture, entity.pos.x + texture->width, pos_y, WHITE);

  entity.pos.x -= entity.v.x;
}

/// CONSUMABLE ITEM ///////////////////////////////////////////////////////////

ConsumableItem::ConsumableItem(Vector2 v, Texture2D* _texture) : color(WHITE) {
  texture = _texture;
  entity.v = v;

  consumed = false;
  flags = 0;
  entity.pos.x = GetScreenWidth();
  entity.pos.y = (rand() % GetScreenHeight()) - (texture->height / 2);
}

ConsumableItem::~ConsumableItem() {}

void ConsumableItem::update() {
  entity.pos.x += entity.v.x;
  entity.pos.y += entity.v.y;
}

bool ConsumableItem::should_die() const { return consumed || out_of_screen(); }

bool ConsumableItem::out_of_screen() const {
  return entity.pos.x <= -texture->width || entity.pos.y >= GetScreenHeight();
}

void ConsumableItem::consume() { consumed = true; }

Color ConsumableItem::get_color() { return color; }

void ConsumableItem::set_color(Color new_color) { color = new_color; }

Rectangle ConsumableItem::rect() {
  return Rectangle{entity.pos.x, entity.pos.y, (float)texture->width,
                   (float)texture->height};
}
