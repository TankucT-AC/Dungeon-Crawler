// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PickupManager.hpp"
#include "LevelMap.hpp"
#include "src/core/config.hpp"
#include "src/world/Chest.hpp"

namespace {
/** @brief Спека оружия по индексу: 1 — пистолет, 2 — винтовка, 3 — ПП. */
const config::weapons::Spec &weaponSpec(int idx) {
  if (idx == 2)
    return config::weapons::kRifle;
  if (idx == 3)
    return config::weapons::kSmg;
  return config::weapons::kPistol;
}

/**
 * @brief Создаёт оружие по индексу (статы из config).
 * @param idx Индекс оружия: 1 — пистолет, 2 — винтовка, 3 — ПП.
 */
std::unique_ptr<Weapon> makeWeapon(int idx) {
  const auto &spec = weaponSpec(idx);
  return std::make_unique<RangedWeapon>(spec.texture, spec.bulletTexture,
                                        spec.fireRate, spec.damage,
                                        spec.bulletSpeed, spec.scale);
}

/** @brief Путь к текстуре оружия по индексу. */
const std::string &weaponTexPath(int idx) { return weaponSpec(idx).texture; }
} // namespace

std::unique_ptr<WeaponPickup> PickupManager::swapWeapon(Player &player,
                                                        ResourceManager &rm,
                                                        WeaponPickup &pickup) {
  std::unique_ptr<Weapon> newWeapon = pickup.takeWeapon();
  if (!newWeapon)
    return nullptr;

  if (player.getWeaponCount() <
      static_cast<size_t>(config::weapons::kMaxSlots)) {
    player.addWeapon(std::move(newWeapon), rm);
    return nullptr;
  }

  int oldSlot = player.activeWeaponIndex();
  std::unique_ptr<Weapon> oldWeapon = player.removeWeapon(oldSlot);
  player.addWeapon(std::move(newWeapon), rm);
  player.switchWeapon(player.getWeaponCount() - 1, rm);

  if (oldWeapon) {
    return std::make_unique<WeaponPickup>(
        rm.getTexture(oldWeapon->getWeaponTexPath()), pickup.getPosition(),
        std::move(oldWeapon));
  }
  return nullptr;
}

void PickupManager::trySwapWeapon(Player &player, ResourceManager &rm,
                                  sf::Vector2<float> mousePos) {
  std::unique_ptr<WeaponPickup> droppedPickup;
  for (auto it = m_pickups.begin(); it != m_pickups.end();) {
    if ((*it)->getHitbox().contains(mousePos)) {
      droppedPickup = swapWeapon(player, rm, **it);
      it = m_pickups.erase(it);
      break;
    }
    ++it;
  }
  if (droppedPickup)
    m_pickups.push_back(std::move(droppedPickup));
}

void PickupManager::tryOpenChest(LevelMap &levelManager, ResourceManager &rm,
                                 sf::Vector2<float> worldPos, int round) {
  std::optional<std::reference_wrapper<Chest>> opt =
      levelManager.findChestAt(worldPos);
  if (!opt.has_value())
    return;
  Chest &chest = opt.value().get();
  if (chest.isOpen())
    return;

  chest.open();
  int wIdx = 1 + (round % 3);
  chest.setWeaponDrop(wIdx);
  m_pickups.push_back(
      std::make_unique<WeaponPickup>(rm.getTexture(weaponTexPath(wIdx)),
                                     chest.getPosition(), makeWeapon(wIdx)));
}
