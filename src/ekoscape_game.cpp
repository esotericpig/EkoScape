/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ekoscape_game.h"

namespace ekoscape {

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

  cybel_engine_ = std::make_unique<CybelEngine>(*this,config,[&](int action) { return build_scene(action); });
  audio_player_ = &cybel_engine_->audio_player();
  scene_man_ = &cybel_engine_->scene_man();
  assets_ = std::make_unique<Assets>("realistic",audio_player_->is_alive());

  cybel_engine_->set_icon(assets_->icon_img());

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

    case SceneAction::kRestart:
      scene_man_->restart_scene();
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
      stop_music(true);
      break;

    case SceneAction::kNil:
    default:
      break;
  }

  if(!result.scene) { return result; }

  if(SceneActions::is_menu(action)) {
    if(star_sys_.is_empty()) { star_sys_.init(cybel_engine_->dimens()); }
  } else {
    star_sys_.clear(); // Free memory, for GameScene in particular.
  }

  return result;
}

void EkoScapeGame::pop_scene() {
  // If on BoringWorkScene, turn music back on, if it was playing before.
  if(scene_man_->curr_scene_type() == SceneAction::kGoToBoringWork) {
    if(was_music_playing_) { play_music(); }
  }

  if(!scene_man_->pop_scene()) {
    std::cerr << "[WARN] No scene to go back to. Quitting instead." << std::endl;
    audio_player_->stop_music();
    cybel_engine_->request_stop();
  }
}

void EkoScapeGame::run() {
  play_music();
  cybel_engine_->run(); // Game loop.
}

void EkoScapeGame::on_key_down_event(const KeyEvent& event,const ViewDimens& /*dimens*/) {
  switch(event.key) {
    case SDLK_AUDIOPLAY:
      play_music();
      break;

    case SDLK_AUDIOSTOP:
      stop_music();
      break;

    // Toggle music.
    case SDLK_n:
      if(audio_player_->is_music_playing()) {
        stop_music();
      } else {
        play_music();
      }
      break;

    // Go back a scene.
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
      // Do not reload the graphics during GameScene or BoringWorkScene (which affects GameScene),
      //     else it'll be all white due to not re-generating the map.
      if(SceneActions::is_menu(scene_man_->curr_scene_type())) { assets_->reload_gfx(); }
      break;

    // Toggle FPS.
    case SDLK_F3:
      if(avg_fps_to_show_ < 0) {
        update_avg_fps_timer_ = 1.0f;
        avg_fps_ = -1.0f;
        avg_fps_to_show_ = 0;
      } else {
        avg_fps_to_show_ = -1;
      }
      break;

    // Toggle fullscreen.
    case SDLK_F11:
      const bool fullscreen = !cybel_engine_->is_fullscreen();

      cybel_engine_->set_fullscreen(fullscreen,true);
      cybel_engine_->set_cursor_visible(!fullscreen);
      break;
  }
}

int EkoScapeGame::update_scene_logic(const FrameStep& step,const ViewDimens& /*dimens*/) {
  star_sys_.update(step);

  if(avg_fps_to_show_ >= 0) {
    const double mpf = step.dpf.millis(); // Milliseconds Per Frame.
    const float fps = (mpf > 0.0) ? static_cast<float>(1000.0 / mpf) : 0.0f;

    if(avg_fps_ < 0.0f) {
      avg_fps_ = fps;
    } else {
      const float smoothing_factor = 0.3f; // Usually 0.1 to 0.3.

      // Exponential Moving Average (EMA) to reduce the effects of hiccups,
      //     instead of a typical average: avg = (avg + fps) / 2.0f.
      avg_fps_ = (avg_fps_ * (1.0f - smoothing_factor)) + (fps * smoothing_factor);
    }

    // Only update the shown FPS at an interval, else the digits change too fast to read.
    if((update_avg_fps_timer_ += static_cast<float>(step.delta_time)) >= 1.0f) {
      avg_fps_to_show_ = static_cast<int>(std::round(avg_fps_));
      update_avg_fps_timer_ = 0.0f;
    }
  }

  return SceneAction::kNil;
}

void EkoScapeGame::draw_scene(Renderer& ren,const ViewDimens& /*dimens*/) {
  if(!star_sys_.is_empty()) {
    ren.begin_2d_scene()
       .begin_auto_scale()
       .begin_add_blend();

    star_sys_.draw(ren,assets_->star_tex());

    ren.end_blend()
       .end_scale();
  }

  if(avg_fps_to_show_ >= 0) {
    ren.begin_2d_scene()
       .begin_auto_anchor_scale({0.0f,0.0f}); // Top left.

    const Size2i padding{5,5};
    const StrUtf8 fps = std::to_string(avg_fps_to_show_);

    assets_->font_renderer().wrap(ren,{padding.w,padding.h,0},0.33f,[&](auto& font) {
      font.draw_bg({0.0f,0.5f},{static_cast<int>(fps.length()),1},padding);
      font.print(fps);
    });

    ren.end_scale();
  }
}

void EkoScapeGame::play_music() {
  if(audio_player_->is_alive() && assets_->music() != nullptr) {
    audio_player_->play_or_resume_music(*assets_->music());
    was_music_playing_ = true;
  } else {
    was_music_playing_ = false;
  }
}

void EkoScapeGame::stop_music(bool on_boring_work) {
  audio_player_->pause_music();
  if(!on_boring_work) { was_music_playing_ = false; }
}

void EkoScapeGame::show_error_global(const std::string& error) {
  CybelEngine::show_error_global(kTitle,error);
}

} // Namespace.
