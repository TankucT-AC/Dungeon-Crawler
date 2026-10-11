// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEBUG_DRAW_HPP
#define DEBUG_DRAW_HPP

/**
 * @brief Runtime-флаги отладочной отрисовки.
 *
 * Задаётся через AppOptions (CLI/env), прокидывается Engine -> RenderManager -> draw().
 * Отдельный хедер без зависимостей, чтобы его могли включать и игровой
 * слой (Entity.hpp), и core (AppOptions.hpp, RenderManager.hpp).
 */
struct DebugDraw {
  bool collisions = false;
};

#endif // DEBUG_DRAW_HPP
