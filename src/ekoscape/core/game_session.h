/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_GAME_SESSION_H_
#define EKOSCAPE_CORE_GAME_SESSION_H_

#include "ekoscape/global.h"

#include "ekoscape/assets/assets.h"

#include <cstddef>
#include <filesystem>

namespace ekoscape {

struct GameSession final {
  Assets& assets;

  struct MenuSceneState {
    std::size_t opt_index = 0;
  } menu_scene_state{};

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
