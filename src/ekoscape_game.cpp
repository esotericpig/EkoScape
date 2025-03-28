/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ekoscape_game.h"

#include "cybel/input/joypad_input.h"
#include "cybel/types/cybel_error.h"
#include "cybel/util/rando.h"

#include "input/input_action.h"
#include "scenes/boring_work_scene.h"
#include "scenes/menu_credits_scene.h"
#include "scenes/menu_scene.h"

namespace ekoscape {

#if defined(__EMSCRIPTEN__)

std::unique_ptr<EkoScapeGame> g_eko_game{};

#endif // __EMSCRIPTEN__

EkoScapeGame::EkoScapeGame() {
  CybelEngine::Config config{
    .title = kTitle,
    .scale_factor = 0.8333f, // Arrival?
    //.size = Size2i{740,500}, // For GIFs/screenshots.
    .fps = 60,
    .vsync = true,
    .max_input_id = InputAction::kMaxId,
    .image_types = IMG_INIT_PNG,
    .music_types = MIX_INIT_OGG,
  };

#if defined(__EMSCRIPTEN__)
  // In Emscripten, VSync is emulated, so it's slower. Therefore, turn it off by default.
  config.vsync = false;
#endif

  // This is the width/height that the game is developed in and used for scaling 2D sprites (menu, etc.).
  // These are fixed values and should not be changed.
  config.target_size = Size2i{1600,900};

  cybel_engine_ = std::make_shared<CybelEngine>(
    *this,config,[&](int type) { return build_scene(type); }
  );
  scene_man_ = &cybel_engine_->scene_man();
  assets_ = std::make_unique<Assets>("realistic",cybel_engine_->audio_player().is_alive());
  ctx_ = std::make_unique<GameContext>(*cybel_engine_,*assets_);

  cybel_engine_->set_icon(assets_->icon_img());

  // TEST: Uncomment to test joypad input.
  //cybel_engine_->input_man().use_fake_joypad(false,FakeJoypadInputType::kAxis);
  // TEST: Uncomment to test touch input.
  //cybel_engine_->input_man().use_mouse_as_finger();

  init_input_map();

  if(!scene_man_->push_scene(SceneAction::kGoToMenu)) {
    throw CybelError{"Failed to push the Menu Scene onto the stack."};
  }

  play_music(true); // When the game first starts, play from a random position.
}

void EkoScapeGame::init_input_map() {
  auto& im = cybel_engine_->input_man();

  // Movement.
  im.map_input(InputAction::kUp,[](auto& i) {
    i.raw_key({SDL_SCANCODE_UP,SDL_SCANCODE_W});
    i.joypad({JoypadInput::kUp});
  });
  im.map_input(InputAction::kDown,[](auto& i) {
    i.raw_key({SDL_SCANCODE_DOWN,SDL_SCANCODE_S});
    i.joypad({JoypadInput::kDown});
  });
  im.map_input(InputAction::kLeft,[](auto& i) {
    i.raw_key({SDL_SCANCODE_LEFT,SDL_SCANCODE_A});
    i.joypad({JoypadInput::kLeft});
  });
  im.map_input(InputAction::kRight,[](auto& i) {
    i.raw_key({SDL_SCANCODE_RIGHT,SDL_SCANCODE_D});
    i.joypad({JoypadInput::kRight});
  });

  // Menu Navigation.
  im.map_input(InputAction::kPageUp,[](auto& i) {
    i.raw_key({SDL_SCANCODE_PAGEUP});
  });
  im.map_input(InputAction::kPageDown,[](auto& i) {
    i.raw_key({SDL_SCANCODE_PAGEDOWN});
  });
  im.map_input(InputAction::kSelect,[](auto& i) {
    i.raw_key({SDL_SCANCODE_RETURN,SDL_SCANCODE_SPACE,SDL_SCANCODE_KP_ENTER});
    i.joypad({JoypadInput::kA});
  });
  im.map_input(InputAction::kGoBack,[](auto& i) {
    i.raw_key({SDL_SCANCODE_BACKSPACE});
    i.joypad({JoypadInput::kB});
  });
#if !defined(__EMSCRIPTEN__)
  im.map_input(InputAction::kQuit,[](auto& i) {
      i.raw_key({SDL_SCANCODE_ESCAPE});
  });
#endif

  // Options/Features.
  im.map_input(InputAction::kToggleMusic,[](auto& i) {
    i.raw_key({SDL_SCANCODE_N});
  });
  im.map_input(InputAction::kToggleFullscreen,[](auto& i) {
    i.raw_key({SDL_SCANCODE_F11});
  });
  im.map_input(InputAction::kToggleMiniMap,[](auto& i) {
    i.raw_key({SDL_SCANCODE_M});
  });
  im.map_input(InputAction::kToggleSpeedrun,[](auto& i) {
    i.raw_key({SDL_SCANCODE_T});
  });
  im.map_input(InputAction::kToggleBossOma,[](auto& i) {
    i.raw_key({SDL_SCANCODE_B});
  });

  // Dev/Secrets.
  im.map_input(InputAction::kRefresh,[](auto& i) {
    i.raw_key({SDL_SCANCODE_R});
  });
  im.map_input(InputAction::kMakeWeird,[](auto& i) {
    i.raw_key({SDL_SCANCODE_F});
  });
  im.map_input(InputAction::kToggleFps,[](auto& i) {
    i.raw_key({SDL_SCANCODE_F3});
  });
}

SceneBag EkoScapeGame::build_scene(int type) {
  SceneBag result{type};

  switch(type) {
    case SceneAction::kQuit:
      cybel_engine_->request_stop();
      break;

    case SceneAction::kGoBack:
      pop_scene();
      break;

    case SceneAction::kRestart:
      if(!scene_man_->restart_scene()) {
        show_error(Util::build_str("Failed to restart scene [",scene_man_->curr_scene_type(),
                                   "]. Going back to the previous scene instead."));
        pop_scene();
      }
      break;

    case SceneAction::kGoToMenu:
      result.scene = std::make_shared<MenuScene>(*ctx_);
      break;

    case SceneAction::kGoToMenuPlay:
      result.scene = std::make_shared<MenuPlayScene>(*ctx_,menu_play_scene_state_);
      break;

    case SceneAction::kGoToMenuCredits:
      result.scene = std::make_shared<MenuCreditsScene>(*ctx_);
      break;

    case SceneAction::kGoToGame: {
      const auto& map_file = menu_play_scene_state_.map_file;

      if(map_file.empty()) {
        show_error("No map was selected.");
      } else {
        try {
          result.scene = std::make_shared<GameScene>(*ctx_,game_scene_state_,map_file);
          result.persist = true; // Preserve GameScene when pausing (e.g., for BoringWorkScene).
        } catch(const CybelError& e) {
          show_error(e.what());
          result.scene = nullptr;
        }
      }
    } break;

    case SceneAction::kGoToBoringWork:
      result.scene = std::make_shared<BoringWorkScene>(*ctx_);
      stop_music(true);
      break;

    case SceneAction::kNil:
    default:
      break;
  }

  if(!result.scene) { return result; }

  if(SceneActions::is_menu(type)) {
    if(star_sys_.is_empty()) { star_sys_.init(cybel_engine_->dimens()); }
  } else if(type != SceneAction::kGoToBoringWork) {
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
#if defined(__EMSCRIPTEN__)
    std::cerr << "[WARN] No scene to go back to; going back a page in Web browser instead." << std::endl;
    EM_ASM( window.history.back(); );
#else
    std::cerr << "[WARN] No scene to go back to; ignoring pop." << std::endl;
#endif
  }
}

void EkoScapeGame::run_loop() { cybel_engine_->run_loop(); }

void EkoScapeGame::run_on_web() {
#if defined(__EMSCRIPTEN__)
  g_eko_game = std::make_unique<EkoScapeGame>();

  g_eko_game->cybel_engine_->run_on_web(g_eko_game->cybel_engine_);
#endif
}

void EkoScapeGame::on_scene_context_restored() { assets_->on_context_restored(); }

void EkoScapeGame::on_scene_input_event(int input_id,const ViewDimens& /*dimens*/) {
  switch(input_id) {
    // Go back a scene.
    case InputAction::kGoBack:
      pop_scene();
      break;

    case InputAction::kQuit:
      cybel_engine_->request_stop();
      break;

    case InputAction::kToggleMusic:
      if(ctx_->audio_player.is_music_playing()) {
        stop_music();
      } else {
        play_music();
      }
      break;

    case InputAction::kToggleFullscreen: {
      const bool fullscreen = !cybel_engine_->is_fullscreen();

      cybel_engine_->set_fullscreen(fullscreen,true);
      cybel_engine_->set_cursor_visible(!fullscreen);
    } break;

    // Toggle BoringWorkScene.
    case InputAction::kToggleBossOma:
      if(scene_man_->curr_scene_type() == SceneAction::kGoToBoringWork) {
        pop_scene();
      } else {
        scene_man_->push_scene(SceneAction::kGoToBoringWork);
      }
      break;

    case InputAction::kRefresh:
      // Do not reload the graphics during GameScene or BoringWorkScene (which affects GameScene),
      //     else it'll be all white due to not re-generating the map.
      if(SceneActions::is_menu(scene_man_->curr_scene_type())) { assets_->reload_gfx(); }
      break;

    case InputAction::kToggleFps:
      if(avg_fps_ < 0.0) {
        avg_fps_age_ = 1.0f; // Show immediately.
        avg_fps_ = 0.0;
      } else {
        avg_fps_ = -1.0;
      }
      break;
  }
}

int EkoScapeGame::update_scene_logic(const FrameStep& step,const ViewDimens& /*dimens*/) {
  star_sys_.update(step);

  if(avg_fps_ >= 0.0) {
    const double mpf = step.dpf.millis(); // Milliseconds Per Frame.
    const double fps = (mpf > 0.0) ? (1000.0 / mpf) : 0.0;

    if(avg_fps_ == 0.0) {
      avg_fps_ = fps;
    } else {
      constexpr double smoothing_factor = 0.3; // Usually 0.1 to 0.3.

      // Exponential Moving Average (EMA) to reduce the effects of hiccups,
      //     instead of a typical average: avg = (avg + fps) / 2.
      avg_fps_ = (avg_fps_ * (1.0 - smoothing_factor)) + (fps * smoothing_factor);
    }

    // Only update the shown FPS at an interval, else the digits change too fast to read.
    if((avg_fps_age_ += static_cast<float>(step.delta_time)) >= 1.0f) {
      avg_fps_str_ = std::to_string(static_cast<int>(std::round(avg_fps_)));
      avg_fps_age_ = 0.0f;
    }
  }

  return SceneAction::kNil;
}

void EkoScapeGame::draw_scene(Renderer& ren,const ViewDimens& /*dimens*/) {
  if(!star_sys_.is_empty() && SceneActions::is_menu(scene_man_->curr_scene_type())) {
    ren.begin_2d_scene()
       .begin_auto_scale()
       .begin_add_blend();

    star_sys_.draw(ren,assets_->star_tex());

    ren.end_blend()
       .end_scale();
  }

  if(avg_fps_ >= 0.0) {
    ren.begin_2d_scene()
       .begin_auto_anchor_scale(Pos2f{0.0f,0.0f}); // Top left.

    const Size2i padding{5,5};

    assets_->font_renderer().wrap(ren,Pos3i{padding.w,padding.h,0},0.33f,[&](auto& font) {
      font.draw_bg(Color4f{0.0f,0.5f},Size2i{static_cast<int>(avg_fps_str_.length()),1},padding);
      font.print(avg_fps_str_);
    });

    ren.end_scale();
  }
}

void EkoScapeGame::play_music(bool rand_pos) {
  if(ctx_->audio_player.is_alive() && assets_->music() != nullptr) {
    ctx_->audio_player.play_or_resume_music(assets_->music());

    if(rand_pos) {
      const auto dur_secs = ctx_->audio_player.fetch_duration(assets_->music()).secs();

      if(dur_secs > 1.0) {
        ctx_->audio_player.set_music_pos(Duration::from_secs(Rando::it().rand_double(0.0,dur_secs - 1.0)));
      }
    }

    was_music_playing_ = true;
  } else {
    was_music_playing_ = false;
  }
}

void EkoScapeGame::stop_music(bool going_to_boring_work) {
  ctx_->audio_player.pause_music();
  if(!going_to_boring_work) { was_music_playing_ = false; }
}

void EkoScapeGame::show_error(const std::string& error) {
  cybel_engine_->show_error(error);
}

void EkoScapeGame::show_error_global(const std::string& error) {
  CybelEngine::show_error_global(kTitle,error);
}

} // namespace ekoscape
