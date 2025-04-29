/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_ctrl.h"

#include "cybel/util/util.h"

namespace cybel {

GameCtrl::GameCtrl(GameCtrl&& other) noexcept {
  move_from(std::move(other));
}

void GameCtrl::move_from(GameCtrl&& other) noexcept {
  close();

  handle_ = other.handle_;
  other.handle_ = NULL;

  id_ = std::exchange(other.id_,-1);
}

GameCtrl::~GameCtrl() noexcept {
  close();
}

GameCtrl& GameCtrl::operator=(GameCtrl&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

GameCtrl::operator bool() const { return handle_ != NULL; }

void GameCtrl::open(int id) noexcept {
  close();

  handle_ = SDL_GameControllerOpen(id);

  if(handle_ == NULL) {
    std::cerr << "[WARN] Failed to open game controller [" << id << "]: " << Util::get_sdl_error() << '.'
              << std::endl;
    return;
  }

  id_ = id;
}

void GameCtrl::close() noexcept {
  if(handle_ != NULL) {
    SDL_GameControllerClose(handle_);
    handle_ = NULL;

    id_ = -1;
  }
}

bool GameCtrl::matches(int id) const { return handle_ != NULL && id_ == id; }

int GameCtrl::id() const { return id_; }

SDL_GameController* GameCtrl::handle() const { return handle_; }

} // namespace cybel
