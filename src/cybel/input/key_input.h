/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_INPUT_KEY_INPUT_H_
#define CYBEL_INPUT_KEY_INPUT_H_

#include "cybel/common.h"

#include "cybel/util/util.h"

namespace cybel {

using KeyMods = Uint16;

template <typename T>
class KeyInput {
public:
  class Hash {
  public:
    std::size_t operator()(const KeyInput& ki) const {
      return Util::build_hash(ki.key_,ki.mods_);
    }
  };

  explicit KeyInput(T key,KeyMods mods = KeyMods{}) noexcept
    : key_(key),mods_(mods & kNormMods) {}

  bool operator==(const KeyInput& other) const {
    return key_ == other.key_ && mods_ == other.mods_;
  }

  T key() const { return key_; }
  KeyMods mods() const { return mods_; }

private:
  // Ignore Caps Lock, etc.
  // See: https://wiki.libsdl.org/SDL2/SDL_Keymod
  static inline const KeyMods kNormMods = static_cast<KeyMods>(
    ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE | KMOD_SCROLL)
  );

  T key_{};
  KeyMods mods_{};
};

using RawKey = SDL_Scancode;
using RawKeyInput = KeyInput<RawKey>;
using SymKey = SDL_Keycode;
using SymKeyInput = KeyInput<SymKey>;

} // namespace cybel
#endif
