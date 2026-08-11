/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ekoscape.h"

#include "ekoscape/assets/asset_ids.h"
#include "ekoscape/assets/font_renderer.h"
#include "ekoscape/core/input_action.h"
#include "ekoscape/core/scene_action.h"
#include "ekoscape/game_scene/game_scene.h"
#include "ekoscape/menu_scenes/boring_work_scene.h"
#include "ekoscape/menu_scenes/credits_scene.h"
#include "ekoscape/menu_scenes/main_menu_scene.h"
#include "ekoscape/menu_scenes/play_menu_scene.h"

#include <cybel/core/cybel_error.h>
#include <cybel/gfx/color.h>
#include <cybel/inputs/joypad_input.h>
#include <cybel/inputs/key_input.h>
#include <cybel/math/pos.h>
#include <cybel/math/size.h>
#include <cybel/scenes/scene_context.h>

#include <cmath>
#include <filesystem>

namespace ekoscape {

CybelEngine::Config EkoScape::build_config() {
  return CybelEngine::Config{
    .title = kTitle,
    .scale_factor = 0.8333f, // Arrival?
    //.size = Size2i{740,500}, // For GIFs/screenshots.

    // This is the width/height that the game is developed in and used for scaling 2D sprites (menu, etc.).
    // These are fixed values and should not be changed.
    .target_size = Size2i{1600,900},

    .fps = 60,
    .vsync = true, // In Web, always enable VSync [i.e., use requestAnimationFrame()].

    .image_types = IMG_INIT_PNG,
    .music_types = MIX_INIT_OGG,
  };
}

EkoScape::EkoScape(CybelEngine& engine)
  : assets_{std::make_shared<Assets>(engine,"realistic")},
    sesh_{*assets_} {}

void EkoScape::on_game_start(CybelEngine& engine,SceneContext& ctx) {
  engine.assets().load_assets(assets_);

  engine.set_icon(engine.assets().image(ImageId::kEkoScapeIcon));

  // TEST: Uncomment to test joypad input.
  //engine.input().use_fake_joypad(false,FakeJoypadInputType::kAxis);
  // TEST: Uncomment to test touch input.
  //engine.input().use_mouse_as_finger();

  init_input_map(engine.input());

  if(!engine.scenes().push_scene(SceneAction::kGoToMainMenu)) {
    throw CybelError{"Failed to push the Main Menu scene."};
  }

  // When the game first starts, play from a random position.
  play_music(ctx,true);
}

void EkoScape::init_input_map(InputMan& im) {
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
  // In Web, don't have a quit hotkey.
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
  im.map_input(InputAction::kToggleBoringWork,[](auto& i) {
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

SceneBag EkoScape::build_scene(scene_id_t id,SceneContext& ctx) {
  SceneBag result{id};
  const auto action = static_cast<SceneAction>(id);

  switch(action) {
    case SceneAction::kQuit:
      quit(ctx);
      break;

    case SceneAction::kGoToMainMenu:
      result.scene = std::make_shared<MainMenuScene>(sesh_,ctx);
      break;

    case SceneAction::kGoToPlayMenu:
      result.scene = std::make_shared<PlayMenuScene>(sesh_,ctx.assets);
      break;

    case SceneAction::kGoToCredits:
      result.scene = std::make_shared<CreditsScene>(sesh_);
      break;

    case SceneAction::kGoToGame: {
      const auto& map_file = sesh_.menu_play_scene_state.map_file;

      if(map_file.empty()) {
        CybelEngine::show_error("No map was selected.");
      } else {
        try {
          result.scene = std::make_shared<GameScene>(sesh_,ctx.engine.renderer(),ctx.assets,map_file);
          result.persist = true; // Preserve GameScene when pausing (e.g., for BoringWorkScene).
        } catch(const CybelError& e) {
          CybelEngine::show_error(e.what());
          result.scene = nullptr;
        }
      }
    } break;

    case SceneAction::kGoToBoringWork:
      result.scene = std::make_shared<BoringWorkScene>();
      stop_music(ctx,true);
      break;

    case SceneAction::kNone:
      break;
  }

  if(!result.scene) { return result; }

  // If leaving BoringWork & was playing music before BoringWork, turn the music back on.
  if(ctx.scenes.curr_scene_id() == static_cast<scene_id_t>(SceneAction::kGoToBoringWork) &&
     action != SceneAction::kGoToBoringWork &&
     was_music_playing_) {
    play_music(ctx);
  }

  if(SceneActions::has_stars(action)) {
    if(star_sys_.is_empty()) { star_sys_.init(ctx.view); }
  } else if(action != SceneAction::kGoToBoringWork) {
    star_sys_.clear(); // Free memory, for GameScene in particular.
  }

  return result;
}

void EkoScape::quit(const SceneContext& ctx) {
#if defined(__EMSCRIPTEN__)
  ctx.engine.nav_back_in_web(); // In Web, just go back a page.
#else // Desktop.
  ctx.engine.request_stop();
#endif
}

void EkoScape::on_scene_input_event(input_id_t input_id,SceneContext& ctx) {
  switch(static_cast<InputAction>(input_id)) {
    case InputAction::kGoBack:
      if(ctx.scenes.has_prev_scenes()) {
        ctx.scenes.pop_scene();
      } else {
        // In Web, go back a page, else just ignore.
        ctx.engine.nav_back_in_web();
      }
      break;

    case InputAction::kQuit:
      quit(ctx);
      break;

    case InputAction::kToggleMusic:
      if(ctx.audio.is_music_playing()) {
        stop_music(ctx);
      } else {
        play_music(ctx);
      }
      break;

    case InputAction::kToggleFullscreen: {
      const bool fullscreen = !ctx.engine.is_fullscreen();

      ctx.engine.set_fullscreen(fullscreen,true);
      ctx.engine.set_cursor_visible(!fullscreen);
    } break;

    case InputAction::kToggleBoringWork:
      if(ctx.scenes.curr_scene_id() == static_cast<scene_id_t>(SceneAction::kGoToBoringWork)) {
        ctx.scenes.pop_scene();
      } else {
        ctx.scenes.push_scene(SceneAction::kGoToBoringWork);
      }
      break;

    case InputAction::kRefresh: {
      const auto curr_scene_id = static_cast<SceneAction>(ctx.scenes.curr_scene_id());

      // Do not reload the graphics during GameScene or BoringWorkScene (which affects GameScene),
      // else it'll be all white due to not re-generating the map.
      if(curr_scene_id == SceneAction::kGoToGame || curr_scene_id == SceneAction::kGoToBoringWork) {
        assets_->make_weird(ctx,false);
      }
    } break;

    case InputAction::kToggleFps:
      avg_fps_ticker_.toggle(TickerFlag::kFireAsap);
      break;

    case InputAction::kToggleFrozen:
      ctx.engine.set_logic_running(!ctx.engine.is_logic_running());
      break;

    default: break;
  }
}

void EkoScape::update_scene_logic(const FrameDelta& delta,SceneContext& ctx) {
  star_sys_.update(delta,ctx.view);

  // Only update the shown FPS at an interval, else the digits change too fast to read.
  if(avg_fps_ticker_.tick(delta)) {
    avg_fps_str_ = std::to_string(static_cast<int>(std::round(ctx.engine.avg_fps())));
  }
}

void EkoScape::draw_scene(Renderer& ren,SceneContext& ctx) {
  if(!star_sys_.is_empty() && SceneActions::has_stars(ctx.scenes.curr_scene_id())) {
    ren.begin_2d_scene()
       .begin_auto_scale()
       .begin_add_blend();

    star_sys_.draw(ren,ctx.assets.texture(TextureId::kStar));

    ren.end_blend()
       .end_scale();
  }

  if(avg_fps_ticker_.is_ticking) {
    ren.begin_2d_scene()
       .begin_auto_anchor_scale(Pos2f{0.0f,0.0f}); // Top left.

    assets_->font_renderer().wrap(ren,ctx,Pos3i{0,0,0},0.33f,[&](auto& font) {
      font.set_bg_padding(Size2i{5,5});
      font.draw_bg(Color4f{0.0f,0.5f},Size2i{static_cast<int>(avg_fps_str_.length()),1});
      font.print(avg_fps_str_);
    });

    ren.end_scale();
  }
}

void EkoScape::play_music(const SceneContext& ctx,bool rand_pos) {
  if(!ctx.audio.is_alive()) { return; }

  const auto* music = ctx.assets.music(MusicId::kEkoScape);
  if(!music) { return; }

  ctx.audio.play_or_resume_music(music);
  was_music_playing_ = true;

  if(rand_pos) {
    ctx.audio.set_music_pos_to_rand();
  }
}

void EkoScape::stop_music(const SceneContext& ctx,bool going_to_boring_work) {
  ctx.audio.pause_music();
  if(!going_to_boring_work) { was_music_playing_ = false; }
}

} // namespace ekoscape
