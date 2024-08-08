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

  game_engine_ = std::make_unique<GameEngine>(*this,config);
  assets_ = std::make_unique<Assets>(Assets::TexturesType::kRealistic,game_engine_->has_music_player());

  current_scene_ = build_menu_scene();
}

std::unique_ptr<MenuScene> EkoScape::build_menu_scene() {
  return std::make_unique<MenuScene>(*game_engine_,*assets_,[&](const std::string& map_file) {
    next_scene_ = build_game_scene(map_file);
  });
}

std::unique_ptr<GameScene> EkoScape::build_game_scene(const std::string& map_file) {
  return std::make_unique<GameScene>(*assets_,map_file,dantares_dist_);
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

void EkoScape::init_scene(Renderer& ren) { current_scene_->init_scene(ren); }

void EkoScape::resize_scene(Renderer& ren,const ViewDimens& dimens) {
  current_scene_->resize_scene(ren,dimens);
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
  }

  current_scene_->on_key_down_event(key);
}

void EkoScape::handle_key_states(const Uint8* keys) { current_scene_->handle_key_states(keys); }

int EkoScape::update_scene_logic(const FrameStep& step) {
  int result = current_scene_->update_scene_logic(step);
  bool has_new_scene = false;

  switch(result) {
    case SceneResult::kQuit:
      game_engine_->request_stop();
      break;

    case SceneResult::kMenuScene:
      next_scene_ = nullptr;
      current_scene_ = build_menu_scene();
      has_new_scene = true;
      break;

    case SceneResult::kNextScene:
      if(next_scene_) {
        current_scene_ = std::move(next_scene_);
        next_scene_ = nullptr;
        has_new_scene = true;
      } else {
        show_error_globally("No next scene to go to. Quitting instead.");
        game_engine_->request_stop();
      }
      break;

    default: break;
  }

  if(has_new_scene) {
    current_scene_->init_scene(game_engine_->renderer());
    current_scene_->resize_scene(game_engine_->renderer(),game_engine_->dimens());
  }

  return 0;
}

void EkoScape::draw_scene(Renderer& ren) { current_scene_->draw_scene(ren); }

void EkoScape::show_error_globally(const std::string& error) {
  GameEngine::show_error_globally(kTitle,error);
}

} // Namespace.
