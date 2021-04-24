#include "screen_objects.h"

#include <stdlib.h>

#include <iostream>

#include "common.h"
#include "raylib.h"

#define SHIELD_LIFESPAN 400
#define SHIELD_END_WARNING 150

/// CLOUD /////////////////////////////////////////////////////////////////////

Cloud::Cloud(float vx, Texture2D* texture) : texture(texture) {
  fade = (float)((rand() % 10) + 1) / 10.0f;
  entity.vx = vx;
  entity.pos.x = GetScreenWidth();
  entity.pos.y = (rand() % GetScreenHeight()) - (texture->height / 2);
}

Cloud::~Cloud() {}

void Cloud::update() { entity.pos.x += entity.vx; }

bool Cloud::should_die() { return entity.pos.x < -texture->width; }

/// PLANE /////////////////////////////////////////////////////////////////////

Plane::Plane() { reset(); }

void Plane::init(Texture2D* _texture) { texture = _texture; }

void Plane::reset() {
  entity.pos.x = 0.0;
  entity.pos.y = 0.0;
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

/// MOVABLE IMAGE /////////////////////////////////////////////////////////////

void Background::init(Texture2D* _texture) { texture = _texture; }

Background::Background(int vx) : vx(vx) { entity.pos.x = 0; }

Background::~Background() {}

void Background::draw_and_move(int pos_y) {
  if (entity.pos.x <= -texture->width) entity.pos.x = 0;

  DrawTexture(*texture, entity.pos.x, pos_y, WHITE);
  DrawTexture(*texture, entity.pos.x + texture->width, pos_y, WHITE);

  entity.pos.x -= vx;
}

/// CONSUMABLE ITEM ///////////////////////////////////////////////////////////

ConsumableItem::ConsumableItem(float vx, Texture2D* texture)
    : color(WHITE), texture(texture) {
  entity.vx = vx;

  consumed = false;
  flags = 0;
  entity.pos.x = GetScreenWidth();
  entity.pos.y = (rand() % GetScreenHeight()) - (texture->height / 2);
}

ConsumableItem::~ConsumableItem() {}

void ConsumableItem::update() { entity.pos.x += entity.vx; }

bool ConsumableItem::should_die() const { return consumed || out_of_screen(); }

bool ConsumableItem::out_of_screen() const {
  return entity.pos.x <= -texture->width;
}

void ConsumableItem::consume() { consumed = true; }

Color ConsumableItem::get_color() { return color; }

void ConsumableItem::set_color(Color new_color) { color = new_color; }

Rectangle ConsumableItem::rect() {
  return Rectangle{entity.pos.x, entity.pos.y, (float)texture->width,
                   (float)texture->height};
}
