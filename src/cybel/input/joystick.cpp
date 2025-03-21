/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "joystick.h"

#include "cybel/util/util.h"

namespace cybel {

Joystick::Joystick(Joystick&& other) noexcept {
  move_from(std::move(other));
}

void Joystick::move_from(Joystick&& other) noexcept {
  close();

  object_ = other.object_;
  other.object_ = NULL;

  id_ = std::exchange(other.id_,-1);
  state = std::exchange(other.state,State{});
}

Joystick::~Joystick() noexcept {
  close();
}

Joystick& Joystick::operator=(Joystick&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

Joystick::operator bool() const { return object_ != NULL; }

void Joystick::open(int id) noexcept {
  close();

  object_ = SDL_JoystickOpen(id);

  if(object_ == NULL) {
    std::cerr << "[WARN] Failed to open joystick [" << id << "]: " << Util::get_sdl_error() << '.'
              << std::endl;
    return;
  }

  id_ = id;
}

void Joystick::close() noexcept {
  if(object_ != NULL) {
    SDL_JoystickClose(object_);
    object_ = NULL;

    id_ = -1;
    state = State{};
  }
}

bool Joystick::matches(int id) const { return object_ != NULL && id_ == id; }

int Joystick::id() const { return id_; }

SDL_Joystick* Joystick::object() const { return object_; }

} // namespace cybel
