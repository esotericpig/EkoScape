/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "music.h"

namespace ekoscape {

Music::Music(const std::string& file) {
  music_ = Mix_LoadMUS(file.c_str());

  if(music_ == NULL) {
    throw EkoScapeError{Util::build_string("Failed to load music [",file,"]: "
        ,Util::get_sdl_mix_error(),'.')};
  }
}

Music::Music(Music&& other) noexcept {
  move_from(std::move(other));
}

void Music::move_from(Music&& other) noexcept {
  destroy();

  music_ = other.music_;
  other.music_ = NULL;
}

Music::~Music() noexcept {
  destroy();
}

void Music::destroy() noexcept {
  if(music_ != NULL) {
    Mix_FreeMusic(music_);
    music_ = NULL;
  }
}

Music& Music::operator=(Music&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

} // Namespace.
