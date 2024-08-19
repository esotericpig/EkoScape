/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_AUDIO_MUSIC_H_
#define EKOSCAPE_CORE_AUDIO_MUSIC_H_

#include "core/common.h"

#include "core/util/cybel_error.h"
#include "core/util/util.h"

#include <filesystem>

namespace cybel {

class GameEngine;

class Music {
public:
  explicit Music(const std::filesystem::path& file);
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
