/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_context.h"

namespace ekoscape {

GameContext::GameContext(CybelEngine& cybel_engine,Assets& assets) noexcept
  : cybel_engine(cybel_engine),audio_player(cybel_engine.audio_player()),assets(assets) {}

} // Namespace.
