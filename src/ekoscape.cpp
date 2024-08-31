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

  cybel_engine_ = std::make_unique<CybelEngine>(
    *this,config
    ,[&](int action) { return build_scene(action); }
  );
  scene_man_ = &cybel_engine_->scene_man();
  assets_ = std::make_unique<Assets>(StyledGraphics::Style::kRealistic,cybel_engine_->has_music_player());

  if(!scene_man_->push_scene(SceneAction::kGoToMenu)) {
    throw CybelError{"Failed to push the Menu Scene onto the stack."};
  }
}

SceneBag EkoScape::build_scene(int action) {
  SceneBag result{action};

  switch(action) {
    case SceneAction::kNil: return result;

    case SceneAction::kQuit:
      cybel_engine_->request_stop();
      return result;

    case SceneAction::kGoBack:
      pop_scene();
      return result;

    case SceneAction::kGoToBoringWork:
      result.scene = std::make_shared<BoringWorkScene>(*cybel_engine_,*assets_);
      break;

    case SceneAction::kGoToMenu:
      result.scene = std::make_shared<MenuScene>(*assets_);
      break;

    case SceneAction::kGoToMenuPlay:
      result.scene = std::make_shared<MenuPlayScene>(
        *cybel_engine_,*assets_,map_file_,is_rand_map_
        ,[&](const auto& file,bool is_rand) { select_map_file(file,is_rand); }
      );
      break;

    case SceneAction::kGoToGame:
      if(map_file_.empty()) {
        cybel_engine_->show_error("No map was selected.");
      } else {
        try {
          result.scene = std::make_shared<GameScene>(*assets_,map_file_,dantares_dist_);
          result.persist = true; // Preserve game state when pausing (e.g., for BoringWorkScene).
        } catch(const CybelError& e) {
          cybel_engine_->show_error(e.what());
          result.scene = nullptr;
        }
      }
      break;

    default: break;
  }

  switch(action) {
    case SceneAction::kGoToMenu:
    case SceneAction::kGoToMenuPlay:
    case SceneAction::kGoToMenuCredits:
      if(star_sys_.is_empty()) { star_sys_.init(cybel_engine_->dimens()); }
      break;

    default:
      star_sys_.clear();
      break;
  }

  return result;
}

void EkoScape::pop_scene() {
  if(!scene_man_->pop_scene()) {
    std::cerr << "[WARN] No scene to go back to. Quitting instead." << std::endl;
    cybel_engine_->request_stop();
  }
}

void EkoScape::run() {
  play_music();
  cybel_engine_->run(); // Game loop.
}

void EkoScape::on_key_down_event(SDL_Keycode key) {
  switch(key) {
    case SDLK_AUDIOPLAY:
      play_music();
      break;

    case SDLK_AUDIOSTOP:
      cybel_engine_->stop_music();
      break;

    // Toggle music.
    case SDLK_m:
      if(cybel_engine_->is_music_playing()) {
        cybel_engine_->stop_music();
      } else {
        play_music();
      }
      break;

    case SDLK_BACKSPACE:
      pop_scene();
      break;

    // Toggle BoringWorkScene.
    case SDLK_b:
      if(scene_man_->curr_scene_type() == SceneAction::kGoToBoringWork) {
        pop_scene();
      } else {
        scene_man_->push_scene(SceneAction::kGoToBoringWork);
      }
      break;
  }
}

int EkoScape::update_scene_logic(const FrameStep& step) {
  star_sys_.update(step);

  return SceneAction::kNil;
}

void EkoScape::draw_scene(Renderer& ren) {
  if(star_sys_.is_empty()) { return; }

  ren.begin_2d_scene()
     .begin_auto_scale()
     .begin_add_blend();

  star_sys_.draw(ren,assets_->star_texture());

  ren.end_blend()
     .end_scale();
}

void EkoScape::play_music() {
  if(cybel_engine_->has_music_player() && assets_->music() != nullptr) {
    cybel_engine_->play_music(*assets_->music());
  }
}

void EkoScape::select_map_file(const std::filesystem::path& file,bool is_rand) {
  map_file_ = file;
  is_rand_map_ = is_rand;
}

void EkoScape::show_error_global(const std::string& error) {
  CybelEngine::show_error_global(kTitle,error);
}

} // Namespace.
