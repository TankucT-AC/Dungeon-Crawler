// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Portal.hpp"
#include "src/core/config.hpp"

Portal::Portal(const sf::Texture &tex, sf::Vector2<float> pos)
    : Entity(tex, pos), m_active(false) {
  sprite.setScale(config::portal::kInactiveScale,
                  config::portal::kInactiveScale);
}

void Portal::activate() {
  m_active = true;
  m_animator.init(config::portal::kFrameW, config::portal::kFrameH,
                  config::portal::kFrameCount, config::portal::kFrameTime,
                  config::portal::kIdleRow, config::portal::kRunRow);
  sprite.setTextureRect(
      {0, 0, config::portal::kFrameW, config::portal::kFrameH});
  sprite.setOrigin(config::portal::kFrameW / 2.f,
                   config::portal::kFrameH / 2.f);
  sprite.setScale({config::portal::kScale * config::math::kTileSize /
                       static_cast<float>(config::portal::kFrameW),
                   config::portal::kScale * config::math::kTileSize /
                       static_cast<float>(config::portal::kFrameW)});
}

void Portal::update(const sf::Time &dt) {
  if (m_active)
    m_animator.update(dt.asSeconds(), true, sprite);
}

void Portal::draw(sf::RenderWindow &window, DebugDraw /*debug*/) const {
  window.draw(sprite);
}

float Portal::getLayerY() const {
  auto hb = sprite.getGlobalBounds();
  return hb.top + hb.height / 2.f;
}
