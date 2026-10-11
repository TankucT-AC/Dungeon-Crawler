// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WEAPONPICKUP_HPP
#define WEAPONPICKUP_HPP

#include "src/game/Entity.hpp"
#include "src/game/Weapon.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <memory>

class WeaponPickup : public Entity, public IDrawAble {
private:
  std::unique_ptr<Weapon> m_weapon;

public:
  WeaponPickup(const sf::Texture &tex, sf::Vector2<float> pos,
               std::unique_ptr<Weapon> weapon)
      : Entity(tex, pos), m_weapon(std::move(weapon)) {
    float scale = config::math::kTileSize / sprite.getLocalBounds().width;
    sprite.setScale({scale, scale});
  }

  std::unique_ptr<Weapon> takeWeapon() { return std::move(m_weapon); }

  virtual void update(const sf::Time &dt) override {}
  virtual void draw(sf::RenderWindow &window,
                    DebugDraw /*debug*/) const override {
    window.draw(sprite);
  }
  virtual float getLayerY() const override {
    auto hb = sprite.getGlobalBounds();
    return hb.top + hb.height / 2.f;
  }
};

#endif
