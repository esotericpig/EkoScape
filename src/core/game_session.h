/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_GAME_SESSION_H_
#define EKOSCAPE_CORE_GAME_SESSION_H_

#include "common.h"

#include "assets/assets.h"

#include <filesystem>

namespace ekoscape {

struct GameSession final {
  Assets& assets;

  struct MenuPlaySceneState {
    std::filesystem::path map_file{};
    bool is_rand_map = true;
  } menu_play_scene_state{};

  struct GameSceneState {
    bool show_mini_map = true;
    bool show_speedrun = true;
  } game_scene_state{};
};

} // namespace ekoscape
#endif
