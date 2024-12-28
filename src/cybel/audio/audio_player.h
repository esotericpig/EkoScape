/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_AUDIO_AUDIO_PLAYER_H_
#define CYBEL_AUDIO_AUDIO_PLAYER_H_

#include "cybel/common.h"

#include "cybel/util/duration.h"
#include "cybel/util/util.h"
#include "music.h"

namespace cybel {

class AudioPlayer {
public:
  /**
   * All:
   *   MIX_INIT_FLAC | MIX_INIT_MID  | MIX_INIT_MOD     | MIX_INIT_MP3 |
   *   MIX_INIT_OGG  | MIX_INIT_OPUS | MIX_INIT_WAVPACK
   *
   * For MIDI on Linux, need to install:
   *   timidity++ libtimidity-devel
   *
   * See: https://wiki.libsdl.org/SDL2_mixer/Mix_Init
   */
  explicit AudioPlayer(int music_types);

  AudioPlayer(const AudioPlayer& other) = delete;
  AudioPlayer(AudioPlayer&& other) noexcept = delete;
  virtual ~AudioPlayer() noexcept;

  AudioPlayer& operator=(const AudioPlayer& other) = delete;
  AudioPlayer& operator=(AudioPlayer&& other) noexcept = delete;

  void play_music(const Music& music);
  void play_or_resume_music(const Music& music);
  void pause_music();
  void stop_music();

  void set_music_pos(const Duration& pos);
  Duration fetch_duration(const Music& music,const Duration& fallback = Duration::kZero) const;

  bool is_alive() const;
  bool is_music_playing() const;

private:
  bool is_alive_ = false;
  std::string curr_music_id_{};
};

} // Namespace.
#endif
