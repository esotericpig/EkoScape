/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ekoscape.h"

namespace ekoscape {

const std::string EkoScape::kTitle = "EkoScape v2.0";

EkoScape::EkoScape(Config config) {
  config.title = kTitle;
  dantares_dist_ = (config.dantares_dist >= 2) ? config.dantares_dist : 2;

  game_engine_ = std::make_unique<GameEngine>(*this,config
      ,[&](int action) { return build_scene(action); });
  assets_ = std::make_unique<Assets>(Assets::TexturesType::kRealistic,game_engine_->has_music_player());
  map_file_ = "assets/maps/classic/tron.txt"; // TODO: Set by callback passed to MenuPlayScene.

  if(!game_engine_->push_scene(SceneAction::kGoToMenu)) {
    throw EkoScapeError{"Failed to push the Menu Scene onto the stack."};
  }
}

std::shared_ptr<Scene> EkoScape::build_scene(int action) {
  std::shared_ptr<Scene> scene = nullptr;

  switch(action) {
    case SceneAction::kQuit:
      game_engine_->request_stop();
      break;

    case SceneAction::kGoBack:
      if(!game_engine_->pop_scene()) {
        std::cerr << "[WARN] No scene to go back to. Quitting instead." << std::endl;
        game_engine_->request_stop();
      }
      break;

    case SceneAction::kGoToMenu:
      scene = std::make_shared<MenuScene>(*assets_);
      break;

    case SceneAction::kGoToGame:
      try {
        scene = std::make_shared<GameScene>(*assets_,map_file_,dantares_dist_);
      } catch(const EkoScapeError& e) {
        game_engine_->show_error(e.what());
        scene = nullptr;
      }
      break;

    default: break;
  }

  return scene;
}

void EkoScape::run() {
  play_music();
  game_engine_->run(); // Game loop.
}

void EkoScape::play_music() {
  if(game_engine_->has_music_player() && assets_->music() != nullptr) {
    game_engine_->play_music(*assets_->music());
  }
}

void EkoScape::on_key_down_event(SDL_Keycode key) {
  switch(key) {
    case SDLK_HOME:
    case SDLK_AUDIOPLAY:
      play_music();
      break;

    case SDLK_END:
    case SDLK_AUDIOSTOP:
      game_engine_->stop_music();
      break;

    case SDLK_BACKSPACE:
      if(!game_engine_->pop_scene()) {
        std::cerr << "[WARN] No scene to go back to. Quitting instead." << std::endl;
        game_engine_->request_stop();
      }
      break;
  }
}

void EkoScape::show_error_global(const std::string& error) {
  GameEngine::show_error_global(kTitle,error);
}

} // Namespace.
