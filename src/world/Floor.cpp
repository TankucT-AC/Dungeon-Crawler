#include "Floor.hpp"
#include "src/core/config.hpp"

Floor::Floor(const sf::Texture &InitTexture, sf::Vector2<float> InitPos)
    : Entity(InitTexture, InitPos) {
  float scale = config::math::kTileSize / sprite.getLocalBounds().width;
  sprite.setScale({scale, scale});
}

void Floor::draw(sf::RenderWindow &window, DebugDraw /*debug*/) const {
  window.draw(sprite);
}

float Floor::getLayerY() const { return -2000.f; }
