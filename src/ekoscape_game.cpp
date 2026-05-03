/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ekoscape_game.h"

#include "cybel/input/joypad_input.h"
#include "cybel/scene/scene_context.h"
#include "cybel/types/cybel_error.h"
#include "cybel/util/rando.h"

#include "core/input_action.h"
#include "scenes/boring_work_scene.h"
#include "scenes/game_scene.h"
#include "scenes/menu_credits_scene.h"
#include "scenes/menu_play_scene.h"
#include "scenes/menu_scene.h"
#include "scenes/scene_action.h"

namespace ekoscape {

CybelEngine::Config EkoScapeGame::build_config() {
  return CybelEngine::Config{
    .title = kTitle,
    .scale_factor = 0.8333f, // Arrival?
    //.size = Size2i{740,500}, // For GIFs/screenshots.

    // This is the width/height that the game is developed in and used for scaling 2D sprites (menu, etc.).
    // These are fixed values and should not be changed.
    .target_size = Size2i{1600,900},

    .fps = 60,

#if defined(__EMSCRIPTEN__)
    // In Emscripten, VSync is emulated, so it's slower. Therefore, turn it off by default.
    .vsync = false,
#else
    .vsync = true,
#endif

    .max_input_id = InputAction::kMax,
    .image_types = IMG_INIT_PNG,
    .music_types = MIX_INIT_OGG,
  };
}

EkoScapeGame::EkoScapeGame(CybelEngine& engine)
  : assets_{"realistic",engine.audio_player().is_alive()},
    sesh_{assets_} {}

void EkoScapeGame::on_game_start(CybelEngine& engine) {
  engine.set_icon(*assets_.image(ImageId::kEkoScapeIcon));

  // TEST: Uncomment to test joypad input.
  //engine.input_man().use_fake_joypad(false,FakeJoypadInputType::kAxis);
  // TEST: Uncomment to test touch input.
  //engine.input_man().use_mouse_as_finger();

  init_input_map(engine.input_man());

  if(!engine.scene_man().push_scene(SceneAction::kGoToMenu)) {
    throw CybelError{"Failed to push the Menu Scene onto the stack."};
  }

  play_music(engine.audio_player(),true); // When the game first starts, play from a random position.
}

void EkoScapeGame::init_input_map(InputMan& im) {
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
  im.map_input(InputAction::kToggleFrozen,[](auto& i) {
    i.raw_key({{KMOD_CTRL,SDL_SCANCODE_F6}});
  });
}

SceneBag EkoScapeGame::build_scene(int type,SceneContext& ctx) {
  SceneBag result{type};

  switch(type) {
    case SceneAction::kQuit:
      ctx.engine.request_stop();
      break;

    case SceneAction::kGoToMenu:
      result.scene = std::make_shared<MenuScene>(sesh_,ctx);
      break;

    case SceneAction::kGoToMenuPlay:
      result.scene = std::make_shared<MenuPlayScene>(sesh_,ctx.engine);
      break;

    case SceneAction::kGoToMenuCredits:
      result.scene = std::make_shared<MenuCreditsScene>(sesh_);
      break;

    case SceneAction::kGoToGame: {
      const auto& map_file = sesh_.menu_play_scene_state.map_file;

      if(map_file.empty()) {
        ctx.engine.show_error("No map was selected.");
      } else {
        try {
          result.scene = std::make_shared<GameScene>(sesh_,ctx.engine.renderer(),map_file);
          result.persist = true; // Preserve GameScene when pausing (e.g., for BoringWorkScene).
        } catch(const CybelError& e) {
          ctx.engine.show_error(e.what());
          result.scene = nullptr;
        }
      }
    } break;

    case SceneAction::kGoToBoringWork:
      result.scene = std::make_shared<BoringWorkScene>(sesh_);
      stop_music(ctx.audio_player,true);
      break;

    case SceneAction::kNone:
    default:
      break;
  }

  if(!result.scene) { return result; }

  // If leaving BOMA & was playing music before BOMA, turn the music back on.
  if(ctx.scene_man.curr_scene_type() == SceneAction::kGoToBoringWork &&
     type != SceneAction::kGoToBoringWork &&
     was_music_playing_) {
    play_music(ctx.audio_player);
  }

  if(SceneActions::is_menu(type)) {
    if(star_sys_.is_empty()) { star_sys_.init(ctx.dimens); }
  } else if(type != SceneAction::kGoToBoringWork) {
    star_sys_.clear(); // Free memory, for GameScene in particular.
  }

  return result;
}

void EkoScapeGame::on_scene_context_loss([[maybe_unused]] SceneContext& ctx) {
  assets_.on_context_loss();
}

void EkoScapeGame::on_scene_context_restore([[maybe_unused]] SceneContext& ctx) {
  assets_.on_context_restore();
}

void EkoScapeGame::on_scene_input_event(input_id_t input_id,SceneContext& ctx) {
  switch(input_id) {
    // Go back a scene.
    case InputAction::kGoBack:
      if(!ctx.scene_man.pop_scene()) {
#if defined(__EMSCRIPTEN__)
        std::cerr << "[WARN] No scene to go back to; going back a page in Web browser instead." << std::endl;
        EM_ASM( window.history.back(); );
#endif
        // No scene to go back to, just ignore pop.
      }
      break;

    case InputAction::kQuit:
      ctx.engine.request_stop();
      break;

    case InputAction::kToggleMusic:
      if(ctx.audio_player.is_music_playing()) {
        stop_music(ctx.audio_player);
      } else {
        play_music(ctx.audio_player);
      }
      break;

    case InputAction::kToggleFullscreen: {
      const bool fullscreen = !ctx.engine.is_fullscreen();

      ctx.engine.set_fullscreen(fullscreen,true);
      ctx.engine.set_cursor_visible(!fullscreen);
    } break;

    // Toggle BoringWorkScene.
    case InputAction::kToggleBossOma:
      if(ctx.scene_man.curr_scene_type() == SceneAction::kGoToBoringWork) {
        ctx.scene_man.pop_scene();
      } else {
        ctx.scene_man.push_scene(SceneAction::kGoToBoringWork);
      }
      break;

    case InputAction::kRefresh:
      // Do not reload the graphics during GameScene or BoringWorkScene (which affects GameScene),
      // else it'll be all white due to not re-generating the map.
      if(SceneActions::is_menu(ctx.scene_man.curr_scene_type())) {
        assets_.reload_gfx(false);
      }
      break;

    case InputAction::kToggleFps:
      if(avg_fps_age_ < 0.0f) {
        avg_fps_age_ = 1.0f; // Show immediately.
      } else {
        avg_fps_age_ = -1.0f;
      }
      break;

    case InputAction::kToggleFrozen:
      ctx.engine.set_logic_running(!ctx.engine.is_logic_running());
      break;
  }
}

void EkoScapeGame::update_scene_logic(const FrameStep& step,SceneContext& ctx) {
  star_sys_.update(step);

  // Only update the shown FPS at an interval, else the digits change too fast to read.
  if(avg_fps_age_ >= 0.0f && (avg_fps_age_ += static_cast<float>(step.delta_time)) >= 1.0f) {
    avg_fps_str_ = std::to_string(static_cast<int>(std::round(ctx.engine.avg_fps())));
    avg_fps_age_ = 0.0f;
  }
}

void EkoScapeGame::draw_scene(Renderer& ren,SceneContext& ctx) {
  if(!star_sys_.is_empty() && SceneActions::is_menu(ctx.scene_man.curr_scene_type())) {
    ren.begin_2d_scene()
       .begin_auto_scale()
       .begin_add_blend();

    star_sys_.draw(ren,assets_.star_tex());

    ren.end_blend()
       .end_scale();
  }

  if(avg_fps_age_ >= 0.0f) {
    ren.begin_2d_scene()
       .begin_auto_anchor_scale(Pos2f{0.0f,0.0f}); // Top left.

    assets_.font_renderer().wrap(ren,Pos3i{0,0,0},0.33f,[&](auto& font) {
      font.set_bg_padding(Size2i{5,5});
      font.draw_bg(Color4f{0.0f,0.5f},Size2i{static_cast<int>(avg_fps_str_.length()),1});
      font.print(avg_fps_str_);
    });

    ren.end_scale();
  }
}

void EkoScapeGame::play_music(AudioPlayer& audio_player,bool rand_pos) {
  if(!audio_player.is_alive()) { return; }

  const auto* music = assets_.music(MusicId::kEkoScape);
  if(!music) { return; }

  audio_player.play_or_resume_music(music);
  was_music_playing_ = true;

  if(rand_pos) {
    const auto dur_secs = audio_player.fetch_duration(music).secs();

    if(dur_secs > 1.0) {
      audio_player.set_music_pos(Duration::from_secs(Rando::it().rand_double(0.0,dur_secs - 1.0)));
    }
  }
}

void EkoScapeGame::stop_music(AudioPlayer& audio_player,bool going_to_boring_work) {
  audio_player.pause_music();
  if(!going_to_boring_work) { was_music_playing_ = false; }
}

} // namespace ekoscape
