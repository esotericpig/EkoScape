/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_AUDIO_AUDIO_H_
#define CYBEL_AUDIO_AUDIO_H_

#include "cybel/common.h"

#include <filesystem>

namespace cybel {

class AudioPlayer;

class Audio {
public:
  explicit Audio(const std::filesystem::path& file);

  Audio(const Audio& other) = delete;
  Audio(Audio&& other) noexcept;
  virtual ~Audio() noexcept;

  Audio& operator=(const Audio& other) = delete;
  Audio& operator=(Audio&& other) noexcept;

  friend class AudioPlayer;

private:
  Mix_Chunk* handle_ = NULL;

  void move_from(Audio&& other) noexcept;
  void destroy() noexcept;
};

} // namespace cybel
#endif
