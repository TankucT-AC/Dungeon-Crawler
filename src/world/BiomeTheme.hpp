// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BIOME_THEME_HPP
#define BIOME_THEME_HPP

#include <string>

struct BiomeTheme {
  const std::string &wallTexture;
  const std::string &floorTexture;
  const std::string &gateOpenTexture;
  const std::string &gateClosedTexture;
  const std::string &chestClosedTexture;
  const std::string &chestOpenTexture;
};

#endif // BIOME_THEME_HPP
