// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APP_OPTIONS_HPP
#define APP_OPTIONS_HPP

#include "src/core/DebugDraw.hpp"
#include <cctype>
#include <cstdlib>
#include <string>

/**
 * @brief Опции запуска игры
 *
 * Источники по приоритету: CLI-флаги, затем переменные окружения.
 *   --no-auth / 2DGAME_NO_AUTH=1 — пропустить окно авторизации;
 *   --debug-collisions / 2DGAME_DEBUG_COLLISIONS=1 — рисовать хитбоксы.
 * Значения env "1/true/yes/on" включают флаг (регистронезависимо).
 */
struct AppOptions {
  bool skipAuth = false;
  bool drawCollisions = false;

  static AppOptions parse(int argc, char *argv[]);

  DebugDraw debugDraw() const;
};

inline bool envEnabled(const char *name) {
  const char *raw = std::getenv(name);
  if (raw == nullptr || *raw == '\0')
    return false;
  std::string v(raw);
  for (char &c : v)
    c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
  return v == "1" || v == "true" || v == "yes" || v == "on";
}

inline AppOptions AppOptions::parse(int argc, char *argv[]) {
  AppOptions opts;
  opts.skipAuth = envEnabled("2DGAME_NO_AUTH");
  opts.drawCollisions = envEnabled("2DGAME_DEBUG_COLLISIONS");
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--no-auth")
      opts.skipAuth = true;
    else if (arg == "--debug-collisions")
      opts.drawCollisions = true;
  }
  return opts;
}

inline DebugDraw AppOptions::debugDraw() const {
  return DebugDraw{drawCollisions};
}

#endif // APP_OPTIONS_HPP
