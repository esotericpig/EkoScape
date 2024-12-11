/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "scene_types.h"

namespace cybel {

KeyEvent::KeyEvent(const SDL_Event& sdl_event) noexcept
    : key(sdl_event.key.keysym.sym),mods(sdl_event.key.keysym.mod) {}

KeyStates::KeyStates() noexcept { refresh(); }

void KeyStates::refresh() noexcept { keys_ = SDL_GetKeyboardState(NULL); }

bool KeyStates::operator[](SDL_Scancode key) const { return keys_[key] == 1; }

} // Namespace.
