/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_GAME_CONTEXT_H_
#define EKOSCAPE_GAME_CONTEXT_H_

#include "common.h"

#include "cybel/audio/audio_player.h"
#include "cybel/cybel_engine.h"

#include "assets/assets.h"

namespace ekoscape {

class GameContext final {
public:
  CybelEngine& cybel_engine;
  AudioPlayer& audio_player;
  Assets& assets;

  explicit GameContext(CybelEngine& cybel_engine,Assets& assets) noexcept;
};

} // Namespace.
#endif
