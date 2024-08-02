/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_MUSIC_H_
#define EKOSCAPE_CORE_MUSIC_H_

#include "common.h"

#include "ekoscape_error.h"
#include "util.h"

namespace ekoscape {

class GameEngine;

class Music {
public:
  Music(const std::string& file);
  Music(const Music&) = delete;
  Music(Music&&) noexcept = delete;
  virtual ~Music() noexcept;

  Music& operator=(const Music&) = delete;
  Music& operator=(Music&&) noexcept = delete;

  friend class GameEngine;

private:
  Mix_Music* music_ = NULL;
};

} // Namespace.
#endif
