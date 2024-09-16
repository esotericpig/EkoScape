/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ekoscape_game.h"

namespace ekoscape {

const std::string EkoScapeGame::kTitle = "EkoScape v2.0";

EkoScapeGame::EkoScapeGame() {
  CybelEngine::Config config{};

  config.title = kTitle;
  config.scale_factor = 0.8333f; // Arrival?
  //config.size = {740,500}; // For GIFs/screenshots.
  config.fps = 60;
  config.vsync = true;

  // This is the width/height that the game is developed in and used for scaling 2D sprites (menu, etc.).
  // These are fixed values and should not be changed.
  config.target_size = {1600,900};

  cybel_engine_ = std::make_unique<CybelEngine>(
    *this,config,[&](int action) { return build_scene(action); }
  );
  scene_man_ = &cybel_engine_->scene_man();
  assets_ = std::make_unique<Assets>(
    StyledGraphics::Style::kRealistic,cybel_engine_->has_music_player()
  );

  cybel_engine_->set_icon(assets_->icon_image());

  if(!scene_man_->push_scene(SceneAction::kGoToMenu)) {
    throw CybelError{"Failed to push the Menu Scene onto the stack."};
  }
}

SceneBag EkoScapeGame::build_scene(int action) {
  SceneBag result{action};

  switch(action) {
    case SceneAction::kQuit:
      cybel_engine_->request_stop();
      break;

    case SceneAction::kGoBack:
      pop_scene();
      break;

    case SceneAction::kGoToMenu:
      result.scene = std::make_shared<MenuScene>(*assets_);
      break;

    case SceneAction::kGoToMenuPlay:
      result.scene = std::make_shared<MenuPlayScene>(
        *cybel_engine_,*assets_,menu_play_scene_state_,
        [&](const auto& state) { menu_play_scene_state_ = state; }
      );
      break;

    case SceneAction::kGoToMenuCredits:
      result.scene = std::make_shared<MenuCreditsScene>(*assets_);
      break;

    case SceneAction::kGoToGame:
      if(menu_play_scene_state_.map_file.empty()) {
        cybel_engine_->show_error("No map was selected.");
      } else {
        try {
          result.scene = std::make_shared<GameScene>(
            *assets_,menu_play_scene_state_.map_file,game_scene_state_,
            [&](const auto& state) { game_scene_state_ = state; }
          );
          result.persist = true; // Preserve GameScene when pausing (e.g., for BoringWorkScene).
        } catch(const CybelError& e) {
          cybel_engine_->show_error(e.what());
          result.scene = nullptr;
        }
      }
      break;

    case SceneAction::kGoToBoringWork:
      result.scene = std::make_shared<BoringWorkScene>(*cybel_engine_,*assets_);
      break;

    case SceneAction::kNil:
    default:
      break;
  }

  if(!result.scene) { return result; }

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

void EkoScapeGame::pop_scene() {
  if(!scene_man_->pop_scene()) {
    std::cerr << "[WARN] No scene to go back to. Quitting instead." << std::endl;
    cybel_engine_->request_stop();
  }
}

void EkoScapeGame::run() {
  play_music();
  cybel_engine_->run(); // Game loop.
}

void EkoScapeGame::on_key_down_event(SDL_Keycode key) {
  switch(key) {
    case SDLK_AUDIOPLAY:
      play_music();
      break;

    case SDLK_AUDIOSTOP:
      cybel_engine_->stop_music();
      break;

    // Toggle music.
    case SDLK_n:
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

    // Refresh.
    case SDLK_r:
      assets_->reload_graphics();
      break;
  }
}

int EkoScapeGame::update_scene_logic(const FrameStep& step,const ViewDimens& /*dimens*/) {
  star_sys_.update(step);

  return SceneAction::kNil;
}

void EkoScapeGame::draw_scene(Renderer& ren) {
  if(star_sys_.is_empty()) { return; }

  ren.begin_2d_scene()
     .begin_auto_scale()
     .begin_add_blend();

  star_sys_.draw(ren,assets_->star_texture());

  ren.end_blend()
     .end_scale();
}

void EkoScapeGame::play_music() {
  if(cybel_engine_->has_music_player() && assets_->music() != nullptr) {
    cybel_engine_->play_music(*assets_->music());
  }
}

void EkoScapeGame::show_error_global(const std::string& error) {
  CybelEngine::show_error_global(kTitle,error);
}

} // Namespace.
