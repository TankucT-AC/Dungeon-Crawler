// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CONFIG_PROJECT_HPP
#define CONFIG_PROJECT_HPP

#include <string>

namespace config {

// Централизованные константы игры
namespace window {
constexpr int kWidth = 1200;
constexpr int kHeight = 800;
inline const std::string kTitle = "2DGame";
} // namespace window

namespace paths {
inline const std::string kAssets = "assets/";
inline const std::string kTextures = kAssets + "textures/";
} // namespace paths

namespace math {
constexpr float kPi = 3.14159265f;
constexpr int kTileSize = 64;
} // namespace math

namespace player {
inline const std::string kSpritesheet = paths::kTextures + "player/knight.png";
constexpr float kScale = 2.5f;
constexpr int kFrameW = 40;
constexpr int kFrameH = 40;
constexpr int kFrameCount = 4;
constexpr float kFrameTime = 0.08f;
constexpr int kIdleRow = 0;
constexpr int kRunRow = 1;
constexpr float kWeaponYOffset = 26.f;
constexpr float kHitboxYOffset = 20.f;
constexpr float kHitboxHalfW = 12.f;
constexpr float kHitboxHalfH = 8.f;
constexpr float kHitboxW = 24.f;
constexpr float kHitboxH = 16.f;

constexpr float kDefaultSpeed = 300.f;
constexpr float kDefaultShootDelay = 0.4f;
constexpr float kDefaultHealth = 10.f;
} // namespace player

namespace enemy {
inline const std::string kSpritesheet =
    paths::kTextures + "enemies/small_enemies.png";
constexpr float kScale = 2.f;
constexpr int kFrameW = 32;
constexpr int kFrameH = 32;
constexpr int kFrameCount = 4;
constexpr float kFrameTime = 0.15f;

constexpr float kDefaultSpeed = 150.f;
constexpr float kDefaultHealth = 5.f;
} // namespace enemy

namespace portal {
inline const std::string kSpritesheet =
    paths::kTextures + "world/portal_states.png";
constexpr int kFrameW = 212;
constexpr int kFrameH = 208;
constexpr int kFrameCount = 8;
constexpr float kFrameTime = 0.08f;
constexpr float kScale = 3.f;
constexpr float kInactiveScale = 0.25f;
constexpr int kIdleRow = 2;
constexpr int kRunRow = 2;
} // namespace portal

namespace bullet {
constexpr float kScale = 2.f;
constexpr float kHitboxW = 12.f;
constexpr float kHitboxH = 4.f;
constexpr float kDefaultLifetime = 2.f;
constexpr float kDefaultSpeed = 200.f;
constexpr float kDefaultDamage = 0.5f;
} // namespace bullet

namespace weapons {
// Бывшие WEAPON_01 (пистолет), WEAPON_02 (винтовка), WEAPON_03 (ПП).
struct Spec {
  const std::string texture;
  const std::string bulletTexture;
  float fireRate;
  float damage;
  float bulletSpeed;
  float scale;
};

inline const Spec kPistol = {
    paths::kTextures + "weapon/weapon_01/weapon_01.png",
    paths::kTextures + "weapon/weapon_01/weapon_bullet_01.png",
    0.4f,
    10.f,
    300.f,
    2.2f, // пистолет 26x8 -> 57x18 px
};

inline const Spec kRifle = {
    paths::kTextures + "weapon/weapon_02/weapon_02.png",
    paths::kTextures + "weapon/weapon_02/weapon_bullet_02.png",
    0.6f,
    1.5f,
    250.f,
    2.0f, // винтовка 38x15 -> 76x30 px
};

inline const Spec kSmg = {
    paths::kTextures + "weapon/weapon_03/weapon_03.png",
    paths::kTextures + "weapon/weapon_03/weapon_bullet_03.png",
    0.15f,
    0.3f,
    400.f,
    2.4f, // ПП 21x11 -> 50x26 px
};

constexpr int kMaxSlots = 2;
constexpr float kMeleeFireRate = 0.5f;
} // namespace weapons

namespace debug {
// Только параметры отрисовки. Само включение отладочной отрисовки —
// runtime-флаг DebugDraw, см. src/game/Entity.hpp.
constexpr float kOutlineThickness = 1.f;
} // namespace debug

namespace textures {
namespace wall {
inline const std::string kDefault =
    paths::kTextures + "world/default/default_wall.png";
inline const std::string kDesert =
    paths::kTextures + "world/desert/desert_wall.png";
inline const std::string kWinter =
    paths::kTextures + "world/winter/winter_wall.png";
} // namespace wall

namespace floor {
inline const std::string kDefault =
    paths::kTextures + "world/default/default_floor.png";
inline const std::string kDesert =
    paths::kTextures + "world/desert/desert_floor.png";
inline const std::string kWinter =
    paths::kTextures + "world/winter/winter_floor.png";
} // namespace floor

namespace chest {
inline const std::string kClosedDefault =
    paths::kTextures + "world/default/default_chest_closed.png";
inline const std::string kOpenDefault =
    paths::kTextures + "world/default/default_chest_open.png";
inline const std::string kClosedDesert =
    paths::kTextures + "world/desert/desert_chest_closed.png";
inline const std::string kOpenDesert =
    paths::kTextures + "world/desert/desert_chest_open.png";
inline const std::string kClosedWinter =
    paths::kTextures + "world/winter/winter_chest_closed.png";
inline const std::string kOpenWinter =
    paths::kTextures + "world/winter/winter_chest_open.png";
} // namespace chest

namespace gate {
inline const std::string kClosed =
    paths::kTextures + "world/default_gate_closed.jpg";
inline const std::string kOpen =
    paths::kTextures + "world/default_gate_open.jpg";
} // namespace gate
} // namespace textures

} // namespace config

#endif // CONFIG_PROJECT_HPP
