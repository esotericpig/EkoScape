/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "music.h"

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

Music::Music(const std::filesystem::path& file)
  : id_(file.string()) {
  const auto file_str = file.u8string();
  const auto* file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  handle_ = Mix_LoadMUS(file_cstr);

  if(handle_ == NULL) {
    throw CybelError{"Failed to load music [",file_cstr,"]: ",Util::get_sdl_mix_error(),'.'};
  }
}

Music::Music(Music&& other) noexcept {
  move_from(std::move(other));
}

void Music::move_from(Music&& other) noexcept {
  destroy();

  handle_ = other.handle_;
  other.handle_ = NULL;

  id_ = std::exchange(other.id_,"");
}

Music::~Music() noexcept {
  destroy();
}

void Music::destroy() noexcept {
  if(handle_ != NULL) {
    Mix_FreeMusic(handle_);
    handle_ = NULL;
  }
}

Music& Music::operator=(Music&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

const std::string& Music::id() const { return id_; }

} // namespace cybel
