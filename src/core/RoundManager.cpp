// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RoundManager.hpp"
#include "src/core/config.hpp"
#include "src/world/BiomeTheme.hpp"
#include "src/world/Prefabs.hpp"

namespace {
inline BiomeTheme DEFAULT = {config::DEFAULT_WALL_TEXTURE,
                             config::DEFAULT_FLOOR_TEXTURE,
                             config::DEFAUTL_GATE_OPEN,
                             config::DEFAULT_GATE_CLOSED,
                             config::DEFAUTL_CHEST_CLOSED_TEXTURE,
                             config::DEFAUTL_CHEST_OPEN_TEXTURE};

inline BiomeTheme DESERT = {
    config::DESERT_WALL_TEXTURE,         config::DESERT_FLOOR_TEXTURE,
    config::DEFAUTL_GATE_OPEN,           config::DEFAULT_GATE_CLOSED,
    config::DESERT_CHEST_CLOSED_TEXTURE, config::DESERT_CHEST_OPEN_TEXTURE};

enum class BIOM_TYPE { DEFAULT = 1, DESERT };
}; // namespace

RoundManager::RoundManager(DungeonGenerator &dungeonGen,
                           LevelManager &levelManager, ResourceManager &rm,
                           Player &player, CombatManager &combat,
                           PickupManager &pickups)
    : m_dungeonGen(dungeonGen), m_levelManager(levelManager), m_rm(rm),
      m_player(player), m_combat(combat), m_pickups(pickups) {}

void RoundManager::generateRound() {
  m_pickups.clear();
  m_combat.reset();
  m_combat.setMaxWaves(
      m_round); // какой раунд, такое и количество волн в конмнате.
  m_portalRoom.reset();
  m_portal.reset();

  m_dungeonGen.reseed();
  DungeonData dungeon = m_dungeonGen.generate();

  switch (m_round) {
  case static_cast<int>(BIOM_TYPE::DEFAULT):
    m_levelManager.buildFromData(dungeon, m_rm, DEFAULT);
    break;
  case static_cast<int>(BIOM_TYPE::DESERT):
    m_levelManager.buildFromData(dungeon, m_rm, DESERT);
    break;
  default:
    m_levelManager.buildFromData(dungeon, m_rm, DEFAULT);
    break;
  }
  m_player.setPosition(dungeon.playerSpawnPoint);
  m_player.initAnimation(config::PLAYER_FRAME_W, config::PLAYER_FRAME_H,
                         config::PLAYER_FRAME_COUNT, config::PLAYER_FRAME_TIME,
                         config::PLAYER_IDLE_ROW, config::PLAYER_RUN_ROW);
  m_player.getSprite().setScale({config::PLAYER_SCALE, config::PLAYER_SCALE});

  for (const auto &r : m_levelManager.getRooms()) {
    if (r->getRoomType() == RoomType::Portal) {
      m_portalRoom = std::ref(*r);
      break;
    }
  }
}

void RoundManager::tryAdvanceRound(sf::Vector2<float> worldPos) {
  if (!m_portal || !m_portal->isActive())
    return;
  if (!m_portal->getHitbox().contains(worldPos))
    return;

  m_round++;
  // m_combat.setMaxWaves(m_round);
  if (m_round > MAX_ROUNDS)
    goToPurgatory();
  else
    generateRound();
}

void RoundManager::goToPurgatory() {
  RoomPlacement rp;
  rp.prefabIndex = Prefabs::IDX_PURIFICATION;
  DungeonData purgData;
  purgData.rooms.push_back(rp);
  purgData.corridorWidth = 1;
  m_levelManager.buildFromData(purgData, m_rm, DEFAULT);
  m_pickups.clear();
  float center = config::TILE_SIZE * Prefabs::PREFAB_SIZE / 2.f;
  m_player.setPosition({center, center});
  m_portal.reset();
}

void RoundManager::spawnPortal() {
  if (!m_portalRoom.has_value())
    return;
  m_portal =
      std::make_unique<Portal>(m_rm.getTexture(config::PORTAL_SPRITESHEET),
                               m_portalRoom.value().get().getChestPos());
  m_portal->activate();
}
