// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ROUNDMANAGER_HPP
#define ROUNDMANAGER_HPP

#include "CombatManager.hpp"
#include "LevelMap.hpp"
#include "PickupManager.hpp"
#include "ResourceManager.hpp"
#include "src/game/Player.hpp"
#include "src/world/DungeonGenerator.hpp"
#include "src/world/Portal.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <functional>
#include <memory>
#include <optional>

/**
 * @brief Менеджер прогрессии раундов.
 *
 * Отвечает за генерацию нового уровня при старте раунда, спавн портала
 * после зачистки всех боевых комнат, переход по порталу и финальное
 * чистилище (game over) после MAX_ROUNDS раундов.
 */
class RoundManager {
private:
  DungeonGenerator &m_dungeonGen;
  LevelMap &m_levelManager;
  ResourceManager &m_rm;
  Player &m_player;
  CombatManager &m_combat;
  PickupManager &m_pickups;

  int m_round = 1;
  static constexpr int MAX_ROUNDS = 3;
  std::unique_ptr<Portal> m_portal;
  std::optional<std::reference_wrapper<Room>> m_portalRoom;

  /**
   * @brief Загружает чистилище (комната-стена) — конец игры.
   * @param enemies Враги текущего раунда (очищаются).
   * @param bullets Пули текущего раунда (очищаются).
   */
  void goToPurgatory();

public:
  /**
   * @brief Внедрение зависимостей — все ссылки живут дольше менеджера.
   */
  RoundManager(DungeonGenerator &dungeonGen, LevelMap &levelManager,
               ResourceManager &rm, Player &player, CombatManager &combat,
               PickupManager &pickups);

  /**
   * @brief Генерирует новый уровень и сбрасывает состояние раунда.
   *
   * Ресидит генератор, перестраивает комнаты, переставляет игрока на спавн
   */
  void generateRound();

  /**
   * @brief Переход на следующий раунд по клику на портал.
   * @param worldPos Позиция курсора в мировых координатах.
   */
  void tryAdvanceRound(sf::Vector2<float> worldPos);
  /** @brief Спавнит портал в портальной комнате (если она найдена). */
  void spawnPortal();

  bool hasPortal() const { return m_portal != nullptr; }
  Portal &portal() { return *m_portal; }
  const Portal &portal() const { return *m_portal; }
  int round() const { return m_round; }
};

#endif
