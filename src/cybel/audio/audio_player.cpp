/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "audio_player.h"

#include "cybel/util/util.h"

namespace cybel {

AudioPlayer::AudioPlayer(int music_types) {
  if(Mix_Init(music_types) == 0) {
    std::cerr << "[WARN] Failed to init SDL_mixer: " << Util::get_sdl_mix_error() << '.' << std::endl;
    return; // Don't fail, since audio is optional.
  }

  int result = -1;

  // NOTE: Use a chunk size of 4096 to avoid staticky audio on the Web.
  if(music_types & MIX_INIT_MID) {
    // Since we're playing a MIDI file, use these settings according to the doc:
    // - https://wiki.libsdl.org/SDL2_mixer/FrontPage
    // - For SDL3, use SDL_AUDIO_S8, probably? Not defined in SDL2.
    result = Mix_OpenAudio(44100,AUDIO_S8,1,4096);
  } else {
    // Defaults:
    //   frequency = MIX_DEFAULT_FREQUENCY(44100) or 48000,
    //   format    = MIX_DEFAULT_FORMAT(AUDIO_S16SYS or SDL_AUDIO_S16),
    //   channels  = MIX_DEFAULT_CHANNELS(2),
    //   chunksize = 2048
    result = Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,4096);
  }

  if(result != 0) {
    Mix_Quit();
    std::cerr << "[WARN] Failed to open audio device: " << Util::get_sdl_mix_error() << '.' << std::endl;
    return; // Don't fail, since audio is optional.
  }

  is_alive_ = true;
}

AudioPlayer::~AudioPlayer() noexcept {
  if(is_alive_) {
    is_alive_ = false;

    // Mix_CloseAudio() is supposed to auto-stop audio, but found it not to work once,
    //     so calling Mix_HaltMusic() just to make sure.
    Mix_HaltMusic();
    Mix_HaltChannel(-1); // -1 for all channels.

    Mix_CloseAudio();
    Mix_Quit();
  }
}

void AudioPlayer::play_music(const Music* music) {
  if(!is_alive_ || music == nullptr) { return; }

  Mix_HaltMusic(); // Avoid Mix_PlayMusic() blocking thread.
  curr_music_id_.clear();

  // -1 to play indefinitely.
  if(Mix_PlayMusic(music->handle_,-1) != 0) {
    std::cerr << "[WARN] Failed to play music: " << Util::get_sdl_mix_error() << '.' << std::endl;
    // Don't fail, since music is optional.
  } else {
    curr_music_id_ = music->id();
  }
}

void AudioPlayer::play_or_resume_music(const Music* music) {
  if(!is_alive_ || music == nullptr) { return; }

  if(!curr_music_id_.empty() && curr_music_id_ == music->id()) {
    if(is_music_playing()) { return; }

    if(is_music_paused()) {
      Mix_ResumeMusic();
      return;
    }
  }

  play_music(music);
}

void AudioPlayer::pause_music() {
  if(!is_alive_) { return; }

  Mix_PauseMusic();
}

void AudioPlayer::stop_music() {
  if(!is_alive_) { return; }

  Mix_HaltMusic();
  curr_music_id_.clear();
}

void AudioPlayer::play_audio(const Audio* audio) {
  if(!is_alive_ || audio == nullptr) { return; }

  // -1 to play on the first free track ("channel").
  Mix_PlayChannel(-1,audio->handle_,0);
}

void AudioPlayer::set_music_pos(const Duration& pos) {
  if(!is_alive_) { return; }

  Mix_SetMusicPosition(pos.secs());
}

Duration AudioPlayer::fetch_duration(const Music* music,const Duration& fallback) const {
  if(!is_alive_ || music == nullptr) { return fallback; }

  const double secs = Mix_MusicDuration(music->handle_);

  return (secs <= 0.0) ? fallback : Duration::from_secs(secs);
}

bool AudioPlayer::is_alive() const { return is_alive_; }

bool AudioPlayer::is_music_playing() const {
  // If paused, Mix_PlayingMusic() will still return as true, so must also check if not paused.
  return is_alive_ && Mix_PlayingMusic() != 0 && !is_music_paused();
}

bool AudioPlayer::is_music_paused() const { return is_alive_ && Mix_PausedMusic() != 0; }

} // namespace cybel
