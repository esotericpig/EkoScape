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

#include <filesystem>

namespace ekoscape {

class GameEngine;

class Music {
public:
  Music(const std::filesystem::path& file);
  Music(const Music& other) = delete;
  Music(Music&& other) noexcept;
  virtual ~Music() noexcept;

  Music& operator=(const Music& other) = delete;
  Music& operator=(Music&& other) noexcept;

  friend class GameEngine;

private:
  Mix_Music* music_ = NULL;

  void move_from(Music&& other) noexcept;
  void destroy() noexcept;
};

} // Namespace.
#endif
