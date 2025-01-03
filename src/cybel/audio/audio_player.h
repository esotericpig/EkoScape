/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_AUDIO_AUDIO_PLAYER_H_
#define CYBEL_AUDIO_AUDIO_PLAYER_H_

#include "cybel/common.h"

#include "cybel/audio/music.h"
#include "cybel/types/duration.h"
#include "cybel/util/util.h"

namespace cybel {

/**
 * This class is a bit odd and unique in that it can be a Zombie, which
 * allows audio to be completely optional if the audio system doesn't load.
 *
 * Because of this, the functions take in pointers and ignores them if they are
 * nullptrs. So in this way, if the audio system doesn't load, you can avoid
 * loading the audio files/data, but still pass them in safely as nullptrs to
 * the functions, without having to write additional if-statements around the
 * calls.
 *
 * Example:
 *   @code
 *   AudioPlayer audio_player{MIX_INIT_OGG};
 *   std::unique_ptr<Music> music{};
 *
 *   if(audio_player.is_alive()) {
 *     try {
 *       music = std::make_unique<Music>("music.ogg");
 *     } catch(const CybelError& e) {
 *       // Ignore.
 *     }
 *   }
 *
 *   // If the music didn't load, no problem.
 *   audio_player.play_music(music.get());
 *   audio_player.play_music(nullptr); // No-op, no err, no prob!
 *   @endcode
 */
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

  void play_music(const Music* music);
  void play_or_resume_music(const Music* music);
  void pause_music();
  void stop_music();

  void set_music_pos(const Duration& pos);
  Duration fetch_duration(const Music* music,const Duration& fallback = Duration::kZero) const;

  bool is_alive() const;
  bool is_music_playing() const;

private:
  bool is_alive_ = false;
  std::string curr_music_id_{};
};

} // Namespace.
#endif
