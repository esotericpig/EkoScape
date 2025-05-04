/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_GAME_CONTEXT_H_
#define EKOSCAPE_CORE_GAME_CONTEXT_H_

#include "common.h"

#include "cybel/cybel_engine.h"

#include "assets/assets.h"

namespace ekoscape {

class GameContext final {
public:
  CybelEngine& cybel_engine;
  AudioPlayer& audio_player;
  Assets& assets;

  explicit GameContext(CybelEngine& cybel_engine,Assets& assets) noexcept
    : cybel_engine(cybel_engine),
      audio_player(cybel_engine.audio_player()),
      assets(assets) {}
};

} // namespace ekoscape
#endif
