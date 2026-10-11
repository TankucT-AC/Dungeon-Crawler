// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/core/AppOptions.hpp"

#include <gtest/gtest.h>

namespace {

void clearAuthEnv() {
  unsetenv("2DGAME_NO_AUTH");
  unsetenv("2DGAME_DEBUG_COLLISIONS");
}

} // namespace

TEST(AppOptionsTest, DefaultsAreOff) {
  clearAuthEnv();
  char *argv[] = {const_cast<char *>("game")};
  const AppOptions opts = AppOptions::parse(1, argv);
  EXPECT_FALSE(opts.skipAuth);
  EXPECT_FALSE(opts.drawCollisions);
  EXPECT_FALSE(opts.debugDraw().collisions);
}

TEST(AppOptionsTest, CliFlagsEnableOptions) {
  clearAuthEnv();
  char *argv[] = {const_cast<char *>("game"), const_cast<char *>("--no-auth"),
                  const_cast<char *>("--debug-collisions")};
  const AppOptions opts = AppOptions::parse(3, argv);
  EXPECT_TRUE(opts.skipAuth);
  EXPECT_TRUE(opts.drawCollisions);
  EXPECT_TRUE(opts.debugDraw().collisions);
}

TEST(AppOptionsTest, UnknownArgsAreIgnored) {
  clearAuthEnv();
  char *argv[] = {const_cast<char *>("game"), const_cast<char *>("--whatever")};
  const AppOptions opts = AppOptions::parse(2, argv);
  EXPECT_FALSE(opts.skipAuth);
  EXPECT_FALSE(opts.drawCollisions);
}

TEST(AppOptionsTest, EnvEnablesOptions) {
  clearAuthEnv();
  setenv("2DGAME_NO_AUTH", "1", 1);
  setenv("2DGAME_DEBUG_COLLISIONS", "true", 1);
  char *argv[] = {const_cast<char *>("game")};
  const AppOptions opts = AppOptions::parse(1, argv);
  EXPECT_TRUE(opts.skipAuth);
  EXPECT_TRUE(opts.drawCollisions);
  clearAuthEnv();
}

TEST(AppOptionsTest, EnvFalsyValuesKeepDefaults) {
  clearAuthEnv();
  setenv("2DGAME_NO_AUTH", "0", 1);
  setenv("2DGAME_DEBUG_COLLISIONS", "", 1);
  char *argv[] = {const_cast<char *>("game")};
  const AppOptions opts = AppOptions::parse(1, argv);
  EXPECT_FALSE(opts.skipAuth);
  EXPECT_FALSE(opts.drawCollisions);
  clearAuthEnv();
}
