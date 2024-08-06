/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_engine.h"

namespace ekoscape {

GameEngine::Resources::Resources() noexcept {}

GameEngine::Resources::~Resources() noexcept {
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

GameEngine::GameEngine(Scene& main_scene,const Config& config)
    : main_scene_(main_scene) {
  init_hints(config);

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    throw EkoScapeError{Util::build_string("Failed to init SDL: ",Util::get_sdl_error(),'.')};
  }
  if(IMG_Init(config.image_types) == 0) {
    throw EkoScapeError{Util::build_string("Failed to init SDL_image: ",Util::get_sdl_img_error(),'.')};
  }

  init_config(config);
  init_gui(config.title);

  if(config.vsync) { set_vsync(true); }

  init_gl();
  init_music_player(config.music_types);
}

void GameEngine::init_hints(const Config& config) {
  // Not available in SDL v2.0.+.
  //SDL_SetHint(SDL_HINT_APP_NAME,config.title.c_str());
  SDL_SetHint(SDL_HINT_AUDIO_DEVICE_APP_NAME,config.title.c_str());
  // One of: nearest, linear, best.
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"best");
  //SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING,"1");
}

void GameEngine::init_config(const Config& config) {
  int width = config.width;
  int height = config.height;

  if(config.scale_factor > 0.0f) {
    SDL_DisplayMode display_mode;

    if(SDL_GetCurrentDisplayMode(0,&display_mode) != 0) {
      std::cerr << "[WARN] Failed to get current display mode: " << Util::get_sdl_error() << '.'
          << std::endl;
      // Don't fail; fall back to Config.width/height.
    } else {
      // Check values so can fall back to Config.width/height if necessary.
      if(display_mode.w > 0) {
        width = static_cast<int>(std::round(static_cast<float>(display_mode.w) * config.scale_factor));
      }
      if(display_mode.h > 0) {
        height = static_cast<int>(std::round(static_cast<float>(display_mode.h) * config.scale_factor));
      }
    }
  }

  title_ = config.title;
  target_width_ = (width > 0) ? width : kFallbackWidth;
  target_height_ = (height > 0) ? height : kFallbackHeight;
  // Allow 0 if the user wants to use delta time only (no delay).
  // - See: end_time()
  target_fps_ = (config.fps >= 0) ? config.fps : kFallbackFps;
  clear_color_ = config.clear_color;

  if(target_fps_ > 0) { // Avoid divide by 0.
    // Convert from FPS to Duration (milliseconds) Per Frame.
    target_dpf_.set_from_millis(static_cast<Uint32>(std::round(1000.0f / static_cast<float>(target_fps_))));
  }
}

void GameEngine::init_gui(const std::string& title) {
  // Use a 2004-2008 version.
  // - Must be set before SDL_CreateWindow().
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);

  res_.window = SDL_CreateWindow(
    title.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,target_width_,target_height_
    ,SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
  );

  if(res_.window == NULL) {
    throw EkoScapeError{Util::build_string("Failed to create window: ",Util::get_sdl_error(),'.')};
  }

  // SDL_WINDOW_RESIZABLE flag in SDL_CreateWindow() increases the size for some reason
  //     (even w/o SDL_WINDOW_ALLOW_HIGHDPI), but this explicit call doesn't.
  SDL_SetWindowResizable(res_.window,SDL_TRUE);
  // With SDL_WINDOW_ALLOW_HIGHDPI flag, the size might have changed.
  SDL_GL_GetDrawableSize(res_.window,&width_,&height_);

  res_.context = SDL_GL_CreateContext(res_.window);

  if (res_.context == NULL) {
    throw EkoScapeError{Util::build_string("Failed to create GL context: ",Util::get_sdl_error(),'.')};
  }

  GLenum error = glewInit();

  if(error != GLEW_OK) {
    throw EkoScapeError{Util::build_string("Failed to init GLEW [",error,"]: "
        ,Util::get_glew_error(error),'.')};
  }
}

void GameEngine::set_vsync(bool enable) {
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

void GameEngine::init_gl() {
  glClearColor(clear_color_.r,clear_color_.g,clear_color_.b,clear_color_.a);
  glClearDepth(1.0);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_TEXTURE_2D);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

  glShadeModel(GL_SMOOTH);
  //glShadeModel(GL_FLAT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    throw EkoScapeError{Util::build_string("Failed to init GL [",error,"]: "
        ,Util::get_gl_error(error),'.')};
  }
}

void GameEngine::resize() {
  int width = 0;
  int height = 0;

  SDL_GL_GetDrawableSize(res_.window,&width,&height);
  resize(width,height);
}

void GameEngine::resize(int width,int height) {
  // Avoid potential divide by 0s [like in begin_3d_scene()].
  if(width < 1) { width = 1; }
  if(height < 1) { height = 1; }

  // Allow resize even if the width & height haven't changed.
  // - If decide to change this logic, need to allow force resize in GameEngine() ctor.

  width_ = width;
  height_ = height;
  view_scale_ = std::min(
    static_cast<float>(width_) / static_cast<float>(target_width_)
    ,static_cast<float>(height_) / static_cast<float>(target_height_)
  );

  glViewport(0,0,width_,height_);
  main_scene_.resize({width_,height_,target_width_,target_height_,view_scale_});
}

void GameEngine::begin_2d_scene() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0.0,width_,height_,0.0,-1.0,1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void GameEngine::begin_3d_scene() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // With 0.1 & 100.0, it had some weird clipping on the edges for 1600x900 for some reason.
  //gluPerspective(45.0,static_cast<GLdouble>(width_) / height_,0.1,100.0);
  gluPerspective(45.0,static_cast<GLdouble>(width_) / height_,0.01,5.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void GameEngine::init_music_player(int music_types) {
  if(Mix_Init(music_types) == 0) {
    std::cerr << "[WARN] Failed to init SDL_mixer: " << Util::get_sdl_mix_error() << '.' << std::endl;
    return; // Don't fail, since music is optional.
  }

  int result = -1;

  if(music_types & MIX_INIT_MID) {
    // Since we're playing a MIDI file, I use these settings according to the doc:
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

void GameEngine::run() {
  is_running_ = true;

  main_scene_.init_scene();
  resize(width_,height_); // Call after init_scene() because it calls `main_scene_`'s resize().

  while(is_running_) {
    start_time();

    handle_events();
    main_scene_.handle_key_states(get_key_states());
    main_scene_.update_scene_logic({dpf_,delta_time_});

    // Check if event/scene requested to stop.
    if(!is_running_) { break; }

    clear_screen();
    main_scene_.draw_scene({width_,height_,target_width_,target_height_,view_scale_});
    SDL_GL_SwapWindow(res_.window);

    end_time();
  }

  std::cerr << "[INFO] Stopping gracefully." << std::endl;
}

void GameEngine::request_stop() { is_running_ = false; }

bool GameEngine::has_music_player() const { return res_.has_music_player; }

void GameEngine::play_music(const Music& music) {
  if(!res_.has_music_player) { return; }

  // -1 to play indefinitely.
  if(Mix_PlayMusic(music.music_,-1) != 0) {
    std::cerr << "[WARN] Failed to play music: " << Util::get_sdl_mix_error() << '.' << std::endl;
    // Don't fail, since music is optional.
  }
}

void GameEngine::stop_music() {
  if(!res_.has_music_player) { return; }

  Mix_HaltMusic();
}

bool GameEngine::is_music_playing() const {
  return res_.has_music_player && Mix_PlayingMusic() == 1;
}

void GameEngine::start_time() {
  frame_timer_.start();
}

void GameEngine::end_time() {
  frame_timer_.end();
  dpf_ = frame_timer_.duration();

  // If target FPS/DPF is 0, then will use delta time only (no delay).
  if(dpf_ < target_dpf_) {
    SDL_Delay((target_dpf_ - dpf_).round_millis());
    dpf_ = target_dpf_;
  }

  delta_time_ = dpf_.secs(); // Delta time should be in fractional seconds.
}

void GameEngine::handle_events() {
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

      case SDL_KEYDOWN:
        if(event.key.keysym.sym == SDLK_ESCAPE) {
          std::cerr << "[EVENT] Received Esc key event." << std::endl;
          request_stop();
          return;
        }

        main_scene_.handle_key_down_event(event.key.keysym.sym);
        break;

      case SDL_KEYUP:
        if(event.key.keysym.sym == SDLK_ESCAPE) {
          std::cerr << "[EVENT] Received Esc key event." << std::endl;
          request_stop();
          return;
        }
        break;

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

  if(should_resize) { resize(); }
}

const Uint8* GameEngine::get_key_states() const { return SDL_GetKeyboardState(NULL); }

void GameEngine::clear_screen() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void GameEngine::show_error(const std::string& error) {
  show_error_globally(title_,error,res_.window);
}

void GameEngine::show_error(const std::string& title,const std::string& error) {
  show_error_globally(title,error,res_.window);
}

void GameEngine::show_error_globally(const std::string& title,const std::string& error,SDL_Window* window) {
  std::cerr << "[ERROR] " << error << std::endl;

  // This can be called before/after SDL_Init()/SDL_Quit().
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,title.c_str(),error.c_str(),window);
}

int GameEngine::target_width() const { return target_width_; }

int GameEngine::target_height() const { return target_height_; }

int GameEngine::width() const { return width_; }

int GameEngine::height() const { return height_; }

float GameEngine::view_scale() const { return view_scale_; }

int GameEngine::target_fps() const { return target_fps_; }

const Duration& GameEngine::target_dpf() const { return target_dpf_; }

const Duration& GameEngine::dpf() const { return dpf_; }

double GameEngine::delta_time() const { return delta_time_; }

} // Namespace.
