/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "audio.h"

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

Audio::Audio(const std::filesystem::path& file) {
  const auto file_str = file.u8string();
  const auto* file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  handle_ = Mix_LoadWAV(file_cstr);

  if(handle_ == NULL) {
    throw CybelError{"Failed to load audio [",file_cstr,"]: ",Util::get_sdl_mix_error(),'.'};
  }
}

Audio::Audio(Audio&& other) noexcept {
  move_from(std::move(other));
}

void Audio::move_from(Audio&& other) noexcept {
  destroy();

  handle_ = other.handle_;
  other.handle_ = NULL;
}

Audio::~Audio() noexcept {
  destroy();
}

void Audio::destroy() noexcept {
  if(handle_ != NULL) {
    Mix_FreeChunk(handle_);
    handle_ = NULL;
  }
}

Audio& Audio::operator=(Audio&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

} // namespace cybel
