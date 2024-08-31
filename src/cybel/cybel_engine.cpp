/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cybel_engine.h"

namespace cybel {

CybelEngine::Resources::Resources() noexcept {}

CybelEngine::Resources::~Resources() noexcept {
  if(has_music_player) {
    has_music_player = false;
    // Mix_CloseAudio() is supposed to auto-stop audio, but found it not to work once,
    //     so calling Mix_HaltMusic() just to make sure.
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();
  }

  IMG_Quit();

  if(context != NULL) {
    SDL_GL_DeleteContext(context);
    context = NULL;
  }
  if(window != NULL) {
    SDL_DestroyWindow(window);
    window = NULL;
  }

  SDL_Quit();
}

CybelEngine::CybelEngine(Scene& main_scene,Config config,const SceneMan::SceneBuilder& build_scene)
    : main_scene_(main_scene) {
  init_hints(config);

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    throw CybelError{Util::build_str("Failed to init SDL: ",Util::get_sdl_error(),'.')};
  }
  if(IMG_Init(config.image_types) == 0) {
    throw CybelError{Util::build_str("Failed to init SDL_image: ",Util::get_sdl_img_error(),'.')};
  }

  init_config(config);
  init_gui(config);
  init_renderer(config,build_scene);
  init_music_player(config);
}

void CybelEngine::init_hints(const Config& config) {
  // Not available in SDL v2.0.
  //SDL_SetHint(SDL_HINT_APP_NAME,config.title.c_str());
  SDL_SetHint(SDL_HINT_AUDIO_DEVICE_APP_NAME,config.title.c_str());
  // One of: nearest, linear, best.
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"best");
  //SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING,"1");
}

void CybelEngine::init_config(Config& config) {
  int width = config.size.w;
  int height = config.size.h;

  // If getting the current display mode fails, we fall back to Config.size.
  //     Therefore, scale_factor needs to have priority over Config.size.
  if(config.scale_factor > 0.0f) {
    SDL_DisplayMode display_mode;

    if(SDL_GetCurrentDisplayMode(0,&display_mode) != 0) {
      std::cerr << "[WARN] Failed to get current display mode: " << Util::get_sdl_error() << '.'
          << std::endl;
      // Don't fail; fall back to Config.size.
    } else if(display_mode.w > 0 && display_mode.h > 0) {
      float sw = static_cast<float>(display_mode.w) * config.scale_factor;
      float sh = static_cast<float>(display_mode.h) * config.scale_factor;

      // If target size set, preserve aspect ratio of target size.
      if(config.target_size.w > 0 && config.target_size.h > 0) {
        const float aspect_ratio = static_cast<float>(config.target_size.w)
            / static_cast<float>(config.target_size.h);
        const float ar_h = std::round(sw / aspect_ratio);

        if(ar_h <= std::round(sh)) {
          sh = ar_h; // Adjust height based on width.
        } else {
          sw = sh * aspect_ratio; // Adjust width based on height.
        }
      }

      width = static_cast<int>(std::round(sw));
      height = static_cast<int>(std::round(sh));
    }
  }

  title_ = config.title;
  config.size.w = (width > 0) ? width : kFallbackWidth;
  config.size.h = (height > 0) ? height : kFallbackHeight;
  config.target_size.w = (config.target_size.w > 0) ? config.target_size.w : config.size.w;
  config.target_size.h = (config.target_size.h > 0) ? config.target_size.h : config.size.h;

  // Allow 0 if the user wants to use delta time only (no delay).
  // - See: end_frame_timer()
  target_fps_ = (config.fps >= 0) ? config.fps : kFallbackFps;

  if(target_fps_ > 0) { // Avoid divide by 0.
    // Convert from FPS to Duration (milliseconds) Per Frame.
    target_dpf_.set_from_millis(std::round(1000.0f / static_cast<float>(target_fps_)));
  }
}

void CybelEngine::init_gui(const Config& config) {
  // Use a 2004-2008 version.
  // - Must be set before SDL_CreateWindow().
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);

  // With the SDL_WINDOW_ALLOW_HIGHDPI flag, the size might change after, therefore it's important that
  //     we call fetch_resize() later, which we do in run().
  res_.window = SDL_CreateWindow(
    config.title.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,config.size.w,config.size.h
    ,SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
  );

  if(res_.window == NULL) {
    throw CybelError{Util::build_str("Failed to create window: ",Util::get_sdl_error(),'.')};
  }

  // The SDL_WINDOW_RESIZABLE flag in SDL_CreateWindow() increases the size for some reason
  //     (even w/o SDL_WINDOW_ALLOW_HIGHDPI), but this explicit call doesn't.
  SDL_SetWindowResizable(res_.window,SDL_TRUE);

  res_.context = SDL_GL_CreateContext(res_.window);

  if(res_.context == NULL) {
    throw CybelError{Util::build_str("Failed to create OpenGL context: ",Util::get_sdl_error(),'.')};
  }

  GLenum error = glewInit();

  if(error != GLEW_OK) {
    throw CybelError{Util::build_str("Failed to init OpenGL GLEW [",error,"]: "
        ,Util::get_glew_error(error),'.')};
  }

  if(config.vsync) { set_vsync(true); }
}

void CybelEngine::init_renderer(const Config& config,const SceneMan::SceneBuilder& build_scene) {
  renderer_ = std::make_unique<Renderer>(config.size,config.target_size,config.clear_color);
  scene_man_ = std::make_unique<SceneMan>(build_scene,[&](Scene& scene) { init_scene(scene); });
}

void CybelEngine::init_scene(Scene& scene) {
  scene.init_scene(*renderer_);
  scene.resize_scene(*renderer_,renderer_->dimens());
}

void CybelEngine::init_music_player(const Config& config) {
  if(Mix_Init(config.music_types) == 0) {
    std::cerr << "[WARN] Failed to init SDL_mixer: " << Util::get_sdl_mix_error() << '.' << std::endl;
    return; // Don't fail, since music is optional.
  }

  int result = -1;

  if(config.music_types & MIX_INIT_MID) {
    // Since we're playing a MIDI file, use these settings according to the doc:
    // - https://wiki.libsdl.org/SDL2_mixer/FrontPage
    // - For SDL3, use SDL_AUDIO_S8, probably? Not defined in SDL2.
    result = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,AUDIO_S8,1,2048);
  } else {
    // Defaults:
    //   MIX_DEFAULT_FREQUENCY(44100) or 48000,
    //   MIX_DEFAULT_FORMAT(SDL_AUDIO_S16 or AUDIO_S16SYS),
    //   MIX_DEFAULT_CHANNELS(2),
    //   2048
    result = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,MIX_DEFAULT_CHANNELS,2048);
  }

  if(result != 0) {
    Mix_Quit();
    std::cerr << "[WARN] Failed to open audio device: " << Util::get_sdl_mix_error() << '.' << std::endl;
    return; // Don't fail, since music is optional.
  }

  res_.has_music_player = true;
}

void CybelEngine::set_vsync(bool enable) {
  if(enable) {
    SDL_SetHint(SDL_HINT_RENDER_VSYNC,"1");

    // First, try with adaptive vsync.
    if(SDL_GL_SetSwapInterval(-1) != 0) {
      SDL_GL_SetSwapInterval(1);
    }
  } else {
    SDL_SetHint(SDL_HINT_RENDER_VSYNC,"0");
    SDL_GL_SetSwapInterval(0);
  }
}

void CybelEngine::fetch_resize(bool force) {
  Size2i size{};

  SDL_GL_GetDrawableSize(res_.window,&size.w,&size.h);
  resize(size,force);
}

void CybelEngine::resize() {
  resize(renderer_->dimens().size,true);
}

void CybelEngine::resize(const Size2i& size,bool force) {
  if(!force && size.w == renderer_->dimens().size.w && size.h == renderer_->dimens().size.h) {
    return; // Size didn't change.
  }

  renderer_->resize(size);
  main_scene_.resize_scene(*renderer_,renderer_->dimens());
  scene_man_->curr_scene().resize_scene(*renderer_,renderer_->dimens());
}

void CybelEngine::run() {
  is_running_ = true;

  // No need to init the current scene in scene_man_,
  //     since it would have already been called on push_scene().
  main_scene_.init_scene(*renderer_);

  // Check the size again, due to SDL_WINDOW_ALLOW_HIGHDPI,
  //     and also need to call the scenes' resize() after init_scene().
  fetch_resize();

  while(is_running_) {
    start_frame_timer();
    handle_events();

    const Uint8* keys = fetch_key_states();
    main_scene_.handle_key_states(keys);
    scene_man_->curr_scene().handle_key_states(keys);

    const FrameStep& step = {dpf_,delta_time_};
    main_scene_.update_scene_logic(step);
    int scene_result = scene_man_->curr_scene().update_scene_logic(step);

    if(scene_result != Scene::kNilType) {
      scene_man_->push_scene(scene_result);
    }

    // Check if event/scene requested to stop.
    if(!is_running_) { break; }

    renderer_->clear_view();
    main_scene_.draw_scene(*renderer_);
    scene_man_->curr_scene().draw_scene(*renderer_);

    SDL_GL_SwapWindow(res_.window);
    end_frame_timer();
  }

  std::cerr << "[INFO] Stopping gracefully." << std::endl;
}

void CybelEngine::request_stop() { is_running_ = false; }

void CybelEngine::start_frame_timer() {
  frame_timer_.start();
}

void CybelEngine::end_frame_timer() {
  frame_timer_.end();
  dpf_ = frame_timer_.duration();

  // If target FPS/DPF is 0, then will use delta time only (no delay).
  if(dpf_ < target_dpf_) {
    SDL_Delay((target_dpf_ - dpf_).round_millis());
    dpf_ = target_dpf_;
  }

  delta_time_ = dpf_.secs(); // Delta time should be in fractional seconds.
}

void CybelEngine::handle_events() {
  SDL_Event event{};
  // Store in a var to prevent re-sizing a bunch of times in the loop (costly),
  //     while the user is still dragging the window corner/edge.
  bool should_resize = false;

  while(is_running_ && SDL_PollEvent(&event) != 0) {
    switch(event.type) {
      case SDL_QUIT:
        std::cerr << "[EVENT] Received Quit event." << std::endl;
        request_stop();
        return;

      case SDL_KEYDOWN: {
        SDL_Keycode key = event.key.keysym.sym;

        if(key == SDLK_ESCAPE) {
          std::cerr << "[EVENT] Received Esc key event." << std::endl;
          request_stop();
          return;
        }

        main_scene_.on_key_down_event(key);
        scene_man_->curr_scene().on_key_down_event(key);
      } break;

      case SDL_KEYUP: {
        SDL_Keycode key = event.key.keysym.sym;

        if(key == SDLK_ESCAPE) {
          std::cerr << "[EVENT] Received Esc key event." << std::endl;
          request_stop();
          return;
        }
      } break;

      case SDL_WINDOWEVENT:
        switch(event.window.event) {
          case SDL_WINDOWEVENT_RESIZED:
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            should_resize = true;
            break;
        }
        break;
    }
  }

  if(should_resize) { fetch_resize(false); }
}

void CybelEngine::play_music(const Music& music) {
  if(!res_.has_music_player) { return; }

  // -1 to play indefinitely.
  if(Mix_PlayMusic(music.music_,-1) != 0) {
    std::cerr << "[WARN] Failed to play music: " << Util::get_sdl_mix_error() << '.' << std::endl;
    // Don't fail, since music is optional.
  }
}

void CybelEngine::stop_music() {
  if(!res_.has_music_player) { return; }

  Mix_HaltMusic();
}

void CybelEngine::show_error(const std::string& error) const {
  show_error(title_,error);
}

void CybelEngine::show_error(const std::string& title,const std::string& error) const {
  show_error_global(title,error,res_.window);
}

void CybelEngine::show_error_global(const std::string& title,const std::string& error,SDL_Window* window) {
  std::cerr << "[ERROR] " << error << std::endl;

  // This can be called before/after SDL_Init()/SDL_Quit().
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,title.c_str(),error.c_str(),window);
}

void CybelEngine::set_title(const std::string& title) { SDL_SetWindowTitle(res_.window,title.c_str()); }

void CybelEngine::reset_title() { set_title(title_); }

bool CybelEngine::has_music_player() const { return res_.has_music_player; }

bool CybelEngine::is_music_playing() const {
  return res_.has_music_player && Mix_PlayingMusic() == 1;
}

const Uint8* CybelEngine::fetch_key_states() const { return SDL_GetKeyboardState(NULL); }

Scene& CybelEngine::main_scene() const { return main_scene_; }

SceneMan& CybelEngine::scene_man() const { return *scene_man_; }

const std::string& CybelEngine::title() const { return title_; }

Renderer& CybelEngine::renderer() const { return *renderer_; }

const ViewDimens& CybelEngine::dimens() const { return renderer_->dimens(); }

int CybelEngine::target_fps() const { return target_fps_; }

const Duration& CybelEngine::target_dpf() const { return target_dpf_; }

const Duration& CybelEngine::dpf() const { return dpf_; }

double CybelEngine::delta_time() const { return delta_time_; }

} // Namespace.
