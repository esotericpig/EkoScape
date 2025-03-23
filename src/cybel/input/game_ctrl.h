/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_INPUT_GAME_CTRL_H_
#define CYBEL_INPUT_GAME_CTRL_H_

#include "cybel/common.h"

namespace cybel {

class GameCtrl {
public:
  explicit GameCtrl() noexcept = default;

  GameCtrl(const GameCtrl& other) = delete;
  GameCtrl(GameCtrl&& other) noexcept;
  virtual ~GameCtrl() noexcept;

  GameCtrl& operator=(const GameCtrl& other) = delete;
  GameCtrl& operator=(GameCtrl&& other) noexcept;
  explicit operator bool() const;

  void open(int id) noexcept;
  void close() noexcept;

  bool matches(int id) const;
  int id() const;
  SDL_GameController* object() const;

private:
  int id_ = -1;
  SDL_GameController* object_ = NULL;

  void move_from(GameCtrl&& other) noexcept;
};

} // namespace cybel
#endif
