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
  config.ge.title = kTitle;
  dantares_dist_ = (config.dantares_dist >= 2) ? config.dantares_dist : 2;

  game_engine_ = std::make_unique<GameEngine>(*this,config.ge);
  assets_ = std::make_unique<Assets>(Assets::TexturesType::kRealistic,game_engine_->has_music_player());

  // TODO: Menu Scene.
  current_scene_ = build_game_scene();
}

std::unique_ptr<GameScene> EkoScape::build_game_scene() {
  // TODO: Callback in Menu Scene so can create this in this class after player picks a map.
  return std::make_unique<GameScene>(*assets_,"assets/maps/classic/tron.txt",dantares_dist_);
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

void EkoScape::init_scene() { current_scene_->init_scene(); }

void EkoScape::handle_key_down_event(SDL_Keycode key) {
  switch(key) {
    case SDLK_HOME:
    case SDLK_AUDIOPLAY:
      play_music();
      break;

    case SDLK_END:
    case SDLK_AUDIOSTOP:
      game_engine_->stop_music();
      break;
  }

  current_scene_->handle_key_down_event(key);
}

void EkoScape::handle_key_states(const Uint8* keys) { current_scene_->handle_key_states(keys); }

int EkoScape::update_scene_logic(const Duration& last_dpf,double delta_time) {
  SceneResult result = SceneResults::to_scene_result(
    current_scene_->update_scene_logic(last_dpf,delta_time)
  );

  switch(result) {
    case SceneResult::kNil:
      break;

    case SceneResult::kQuit:
      game_engine_->request_stop();
      break;

    case SceneResult::kMenuScene:
      // TODO: Implement build_menu_scene().
      //next_scene_ = nullptr;
      //current_scene_ = build_menu_scene();
      //current_scene_->init_scene();
      game_engine_->request_stop();
      break;

    case SceneResult::kNextScene:
      // TODO: Need to test next scene code.
      if(next_scene_) {
        current_scene_ = std::move(next_scene_);
        next_scene_ = nullptr;
        current_scene_->init_scene();
      } else {
        show_error_globally("No next scene to go to. Quitting instead.");
        game_engine_->request_stop();
      }
      break;
  }

  return 0;
}

void EkoScape::draw_scene() { current_scene_->draw_scene(); }

void EkoScape::show_error_globally(const std::string& error) {
  GameEngine::show_error_globally(kTitle,error);
}

} // Namespace.
