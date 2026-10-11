// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Bullet.hpp"
#include <SFML/System/Time.hpp>

Bullet::Bullet(const sf::Texture &InitTexture,
               const sf::Vector2<float> &InitStartPos,
               const sf::Vector2<float> &InitDirection, float InitDegree,
               float InitSpeed, float InitLifetime, float InitDamage)
    : Entity(InitTexture, InitStartPos), direction(InitDirection),
      lifetime(InitLifetime), damage(InitDamage), dead(false), speed(InitSpeed),
      velocity({0.f, 0.f}) {
  sf::Rect<float> b = sprite.getLocalBounds();
  sprite.setOrigin(b.width / 2.f, b.height / 2.f);
  sprite.setScale(config::bullet::kScale, config::bullet::kScale);
  sprite.setRotation(InitDegree);
}

void Bullet::update(const sf::Time &dt) {
  sf::Vector2<float> offset = direction * speed;
  this->move(dt, offset);

  lifetime -= dt.asSeconds();
  if (lifetime <= 0) {
    dead = true;
    lifetime = 0;
  }
}

void Bullet::draw(sf::RenderWindow &window, DebugDraw debug) const {
  window.draw(sprite);

  if (debug.collisions) {
    sf::Rect<float> hb = this->getHitbox();
    sf::RectangleShape debugRect({hb.width, hb.height});
    debugRect.setPosition(hb.left, hb.top);
    debugRect.setFillColor(sf::Color::Transparent);
    debugRect.setOutlineColor(sf::Color::Red);
    debugRect.setOutlineThickness(config::debug::kOutlineThickness);
    window.draw(debugRect);
  }
}

void Bullet::move(const sf::Time &dt, const sf::Vector2<float> &offset) {
  sprite.move(dt.asSeconds() * offset);
}

int Bullet::getLifetime() const { return lifetime; }
float Bullet::getDamage() { return damage; }
bool Bullet::isBulletAlive() const { return !dead; }
sf::Rect<float> Bullet::getHitbox() const {
  sf::Vector2f pos = sprite.getPosition();
  return sf::Rect<float>(pos.x - config::bullet::kHitboxW / 2.f,
                         pos.y - config::bullet::kHitboxH / 2.f,
                         config::bullet::kHitboxW, config::bullet::kHitboxH);
}

float Bullet::getLayerY() const {
  sf::Rect<float> hitbox = this->getHitbox();
  return hitbox.top + (hitbox.height / 2.f);
}
