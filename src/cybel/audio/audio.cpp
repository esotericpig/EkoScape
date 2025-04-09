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

Audio::Audio(const std::filesystem::path& file)
  : id_(file.string()) {
  const auto file_str = file.u8string();
  const auto* file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  object_ = Mix_LoadWAV(file_cstr);

  if(object_ == NULL) {
    throw CybelError{"Failed to load audio [",file_cstr,"]: ",Util::get_sdl_mix_error(),'.'};
  }
}

Audio::Audio(Audio&& other) noexcept {
  move_from(std::move(other));
}

void Audio::move_from(Audio&& other) noexcept {
  destroy();

  object_ = other.object_;
  other.object_ = NULL;

  id_ = std::exchange(other.id_,"");
}

Audio::~Audio() noexcept {
  destroy();
}

void Audio::destroy() noexcept {
  if(object_ != NULL) {
    Mix_FreeChunk(object_);
    object_ = NULL;
  }
}

Audio& Audio::operator=(Audio&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

const std::string& Audio::id() const { return id_; }

} // namespace cybel
