/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_INPUT_JOYSTICK_H_
#define CYBEL_INPUT_JOYSTICK_H_

#include "cybel/common.h"

namespace cybel {

class Joystick {
public:
  explicit Joystick() noexcept = default;

  Joystick(const Joystick& other) = delete;
  Joystick(Joystick&& other) noexcept;
  virtual ~Joystick() noexcept;

  Joystick& operator=(const Joystick& other) = delete;
  Joystick& operator=(Joystick&& other) noexcept;
  explicit operator bool() const;

  void open(int id) noexcept;
  void close() noexcept;

  bool matches(int id) const;
  int id() const;
  SDL_Joystick* handle() const;

private:
  int id_ = -1;
  SDL_Joystick* handle_ = NULL;

  void move_from(Joystick&& other) noexcept;
};

} // namespace cybel
#endif
