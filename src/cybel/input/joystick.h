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
  struct State {
    Sint16 axis = -1;
    Sint16 axis_value = 0;
    Sint16 hat_value = -1;
    Sint16 button = -1;
  };

  State state{};

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
  SDL_Joystick* object() const;

private:
  int id_ = -1;
  SDL_Joystick* object_ = NULL;

  void move_from(Joystick&& other) noexcept;
};

} // namespace cybel
#endif
