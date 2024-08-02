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

Music::~Music() noexcept {
  if(music_ != NULL) {
    Mix_FreeMusic(music_);
    music_ = NULL;
  }
}

} // Namespace.
