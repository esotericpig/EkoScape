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
  assets_ = std::make_unique<Assets>(StyledGraphics::Style::kRealistic,game_engine_->has_music_player());
  map_file_ = "assets/maps/classic/castles_garden.txt"; // TODO: Set by callback passed to MenuPlayScene.

  if(!game_engine_->push_scene(SceneAction::kGoToMenu)) {
    throw CybelError{"Failed to push the Menu Scene onto the stack."};
  }
}

std::shared_ptr<Scene> EkoScape::build_scene(int action) {
  std::shared_ptr<Scene> scene = nullptr;

  switch(action) {
    case SceneAction::kQuit:
      game_engine_->request_stop();
      break;

    case SceneAction::kGoBack:
      pop_scene();
      break;

    case SceneAction::kGoToBoringWork:
      scene = std::make_shared<BoringWorkScene>(*assets_);
      break;

    case SceneAction::kGoToMenu:
      scene = std::make_shared<MenuScene>(*assets_);
      break;

    case SceneAction::kGoToGame:
      // Was not paused?
      if(!game_scene_) {
        try {
          game_scene_ = std::make_shared<GameScene>(*assets_,map_file_,dantares_dist_);
        } catch(const CybelError& e) {
          game_engine_->show_error(e.what());
          game_scene_ = nullptr;
        }
      }

      scene = game_scene_;
      break;

    default: break;
  }

  return scene;
}

void EkoScape::pop_scene() {
  if(game_engine_->curr_scene_type() == SceneAction::kGoToGame) {
    game_scene_ = nullptr;
  }

  if(!game_engine_->pop_scene()) {
    std::cerr << "[WARN] No scene to go back to. Quitting instead." << std::endl;
    game_engine_->request_stop();
  }
}

void EkoScape::run() {
  play_music();
  game_engine_->run(); // Game loop.
}

void EkoScape::on_key_down_event(SDL_Keycode key) {
  switch(key) {
    case SDLK_AUDIOPLAY:
      play_music();
      break;

    case SDLK_AUDIOSTOP:
      game_engine_->stop_music();
      break;

    // Toggle music.
    case SDLK_m:
      if(game_engine_->is_music_playing()) {
        game_engine_->stop_music();
      } else {
        play_music();
      }
      break;

    case SDLK_BACKSPACE:
      pop_scene();
      break;

    // Toggle boring work scene.
    case SDLK_b:
      if(game_engine_->curr_scene_type() == SceneAction::kGoToBoringWork) {
        pop_scene();
      } else {
        game_engine_->push_scene(SceneAction::kGoToBoringWork);
      }
      break;
  }
}

void EkoScape::play_music() {
  if(game_engine_->has_music_player() && assets_->music() != nullptr) {
    game_engine_->play_music(*assets_->music());
  }
}

void EkoScape::show_error_global(const std::string& error) {
  GameEngine::show_error_global(kTitle,error);
}

} // Namespace.
