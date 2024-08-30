/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "music.h"

namespace cybel {

Music::Music(const std::filesystem::path& file) {
  const std::u8string file_str = file.u8string();
  auto file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  music_ = Mix_LoadMUS(file_cstr);

  if(music_ == NULL) {
    throw CybelError{Util::build_str("Failed to load music [",file_cstr,"]: "
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
