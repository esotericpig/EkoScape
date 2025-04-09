/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_AUDIO_MUSIC_H_
#define CYBEL_AUDIO_MUSIC_H_

#include "cybel/common.h"

#include <filesystem>

namespace cybel {

class AudioPlayer;

class Music {
public:
  explicit Music(const std::filesystem::path& file);

  Music(const Music& other) = delete;
  Music(Music&& other) noexcept;
  virtual ~Music() noexcept;

  Music& operator=(const Music& other) = delete;
  Music& operator=(Music&& other) noexcept;

  const std::string& id() const;

  friend class AudioPlayer;

private:
  std::string id_{};
  Mix_Music* object_ = NULL;

  void move_from(Music&& other) noexcept;
  void destroy() noexcept;
};

} // namespace cybel
#endif
