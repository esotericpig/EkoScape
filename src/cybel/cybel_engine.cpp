/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cybel_engine.h"

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

CybelEngine::Resources::~Resources() noexcept {
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
  : title_(config.title),main_scene_(main_scene) {
  init_hints();

  // Don't use SDL_INIT_AUDIO here, since audio is optional.
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    throw CybelError{"Failed to init SDL: ",Util::get_sdl_error(),'.'};
  }
  if(IMG_Init(config.image_types) == 0) {
    throw CybelError{"Failed to init SDL_image: ",Util::get_sdl_img_error(),'.'};
  }

  init_config(config);
  init_gui(config);

  renderer_ = std::make_unique<Renderer>(config.size,config.target_size,config.clear_color);
  scene_man_ = std::make_unique<SceneMan>(build_scene,[&](Scene& scene) { init_scene(scene); });
  input_man_ = std::make_unique<InputMan>(config.max_input_id);
  audio_player_ = std::make_unique<AudioPlayer>(config.music_types);
}

void CybelEngine::init_hints() {
  //SDL_SetHint(SDL_HINT_APP_NAME,title_.c_str());
  SDL_SetHint(SDL_HINT_AUDIO_DEVICE_APP_NAME,title_.c_str());
  // One of: nearest, linear, best.
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"nearest");
  //SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING,"1");
}

void CybelEngine::init_config(Config& config) {
  int width = config.size.w;
  int height = config.size.h;

  // If getting the current display mode fails, we fall back to Config.size.
  //     Therefore, scale_factor needs to have priority over Config.size.
  if(config.scale_factor > 0.0f) {
    SDL_DisplayMode display_mode{};

    if(SDL_GetCurrentDisplayMode(0,&display_mode) != 0) {
      std::cerr << "[WARN] Failed to get current display mode: " << Util::get_sdl_error() << '.' << std::endl;
      // Don't fail; fall back to Config.size.
    } else if(display_mode.w > 0 && display_mode.h > 0) {
      float sw = static_cast<float>(display_mode.w) * config.scale_factor;
      float sh = static_cast<float>(display_mode.h) * config.scale_factor;

      // If target size set, preserve aspect ratio of target size.
      if(config.target_size.w > 0 && config.target_size.h > 0) {
        const float aspect_ratio = static_cast<float>(config.target_size.w) /
                                   static_cast<float>(config.target_size.h);
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

  config.size.w = (width > 0) ? width : kFallbackWidth;
  config.size.h = (height > 0) ? height : kFallbackHeight;
  config.target_size.w = (config.target_size.w > 0) ? config.target_size.w : config.size.w;
  config.target_size.h = (config.target_size.h > 0) ? config.target_size.h : config.size.h;

  // Allow 0 if the user wants to use delta time only (no delay).
  // - See: end_frame_timer()
  target_fps_ = (config.fps >= 0) ? config.fps : kFallbackFps;

  if(target_fps_ > 0) { // Avoid divide by 0.
    // Convert from FPS to Duration (milliseconds) Per Frame.
    target_dpf_.set_from_millis(std::round(1000.0 / static_cast<double>(target_fps_)));
  }
}

void CybelEngine::init_gui(const Config& config) {
  // Use a 2004-2008 version.
  // - NOTE: Must be set before SDL_CreateWindow().
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);

  // With the SDL_WINDOW_ALLOW_HIGHDPI flag, the size might change after, therefore it's important that
  //     we call sync_size() later, which we do in run().
  res_.window = SDL_CreateWindow(
    title_.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,config.size.w,config.size.h
    ,SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
  );

  if(res_.window == NULL) {
    throw CybelError{"Failed to create window: ",Util::get_sdl_error(),'.'};
  }

  // The SDL_WINDOW_RESIZABLE flag in SDL_CreateWindow() increases the size for some reason
  //     (even w/o SDL_WINDOW_ALLOW_HIGHDPI), but this explicit call doesn't.
  SDL_SetWindowResizable(res_.window,SDL_TRUE);

  res_.context = SDL_GL_CreateContext(res_.window);

  if(res_.context == NULL) {
    throw CybelError{"Failed to create OpenGL context: ",Util::get_sdl_error(),'.'};
  }

  GLenum error = glewInit();

  if(error != GLEW_OK) {
    throw CybelError{"Failed to init OpenGL GLEW [",error,"]: ",Util::get_glew_error(error),'.'};
  }

  check_gl_version();
  set_vsync(config.vsync);
}

void CybelEngine::check_gl_version() {
  auto gl_version_cstr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const std::string gl_version = (gl_version_cstr != nullptr)
                                 ? gl_version_cstr : "Failed to get OpenGL version";

  std::cerr << "[INFO] OpenGL version: " << gl_version << '.' << std::endl;

  if(!GLEW_VERSION_2_1) {
    std::cerr << "[WARN] OpenGL version is < 2.1." << std::endl;

    std::string msg = Util::build_str(
      "This system's OpenGL version is less than 2.1.\n",
      "The game may not function correctly.\n",
      "Consider downloading & using Mesa for your platform.\n\n",
      "OpenGL version: ",gl_version,'.'
    );

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,title_.c_str(),msg.c_str(),res_.window);
  }
}

void CybelEngine::init_scene(Scene& scene) {
  scene.init_scene(renderer_->dimens());
  scene.resize_scene(*renderer_,renderer_->dimens());
}

void CybelEngine::run() {
  is_running_ = true;

  // No need to init the current scene in scene_man_,
  //     since it would have already been called on push_scene().
  main_scene_.init_scene(renderer_->dimens());

  // Check the size again, due to SDL_WINDOW_ALLOW_HIGHDPI,
  //     and also need to call the scenes' resize() after init_scene().
  sync_size(true);

  while(is_running_) {
    start_frame_timer();

    input_man_->reset_states();
    handle_events();
    handle_input_states();

    main_scene_.update_scene_logic(frame_step_,renderer_->dimens());
    const int scene_result = scene_man_->curr_scene().update_scene_logic(frame_step_,renderer_->dimens());

    if(scene_result != Scene::kNilType) {
      scene_man_->push_scene(scene_result);
    }

    renderer_->clear_view();
    main_scene_.draw_scene(*renderer_,renderer_->dimens());
    scene_man_->curr_scene().draw_scene(*renderer_,renderer_->dimens());
    SDL_GL_SwapWindow(res_.window);

    end_frame_timer();
  }

  std::cerr << "[INFO] Stopping gracefully." << std::endl;
}

void CybelEngine::request_stop() { is_running_ = false; }

void CybelEngine::sync_size(bool force) {
  Size2i size{};

  SDL_GL_GetDrawableSize(res_.window,&size.w,&size.h);
  resize(size,force);
}

void CybelEngine::resize(const Size2i& size,bool force) {
  if(!force && size.w == renderer_->dimens().size.w && size.h == renderer_->dimens().size.h) {
    return; // Size didn't change.
  }

  renderer_->resize(size);
  main_scene_.resize_scene(*renderer_,renderer_->dimens());
  scene_man_->curr_scene().resize_scene(*renderer_,renderer_->dimens());
}

void CybelEngine::start_frame_timer() {
  frame_timer_.start();
}

void CybelEngine::end_frame_timer() {
  frame_step_.dpf = frame_timer_.peek();

  // If target_dpf_ (target_fps_) is 0, then will use delta time only (no delay).
  if(frame_step_.dpf < target_dpf_) {
    SDL_Delay((target_dpf_ - frame_step_.dpf).round_millis());
    frame_step_.dpf = frame_timer_.end();
  }

  frame_step_.delta_time = frame_step_.dpf.secs(); // Delta time should be in fractional seconds.
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

      case SDL_KEYDOWN:
        handle_keydown_event(event);
        break;

      case SDL_KEYUP:
        handle_keyup_event(event);
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

  if(!is_running_) { return; }
  if(should_resize) { sync_size(false); }
}

void CybelEngine::handle_keydown_event(const SDL_Event& event) {
  const RawKeyInput raw_key{event.key.keysym.scancode,event.key.keysym.mod};
  const SymKeyInput sym_key{event.key.keysym.sym,event.key.keysym.mod};

  if(raw_key.key() == SDL_SCANCODE_ESCAPE) {
    std::cerr << "[EVENT] Received Esc key event." << std::endl;
    request_stop();
    return;
  }

  std::unordered_set<int> processed_ids{};

  for(auto id : input_man_->fetch_ids(raw_key)) {
    // Not inserted? (already processed)
    if(!processed_ids.insert(id).second) { continue; }

    main_scene_.on_input_event(id,renderer_->dimens());
    scene_man_->curr_scene().on_input_event(id,renderer_->dimens());
  }
  for(auto id : input_man_->fetch_ids(sym_key)) {
    // Not inserted? (already processed)
    if(!processed_ids.insert(id).second) { continue; }

    main_scene_.on_input_event(id,renderer_->dimens());
    scene_man_->curr_scene().on_input_event(id,renderer_->dimens());
  }
}

void CybelEngine::handle_keyup_event(const SDL_Event& event) {
  const RawKeyInput raw_key{event.key.keysym.scancode,event.key.keysym.mod};

  if(raw_key.key() == SDL_SCANCODE_ESCAPE) {
    std::cerr << "[EVENT] Received Esc key event." << std::endl;
    request_stop();
    return;
  }
}

void CybelEngine::handle_input_states() {
  int num_keys = 0;
  const auto* raw_keys = SDL_GetKeyboardState(&num_keys);
  const KeyMods mods = SDL_GetModState();

  for(int i = 0; i < num_keys; ++i) {
    if(raw_keys[i] == 1) {
      auto raw_key = static_cast<RawKey>(i);
      SymKey sym_key = SDL_GetKeyFromScancode(raw_key);

      input_man_->set_state(RawKeyInput{raw_key,mods},true);
      input_man_->set_state(SymKeyInput{sym_key,mods},true);
    }
  }

  main_scene_.handle_input_states(input_man_->states(),renderer_->dimens());
  scene_man_->curr_scene().handle_input_states(input_man_->states(),renderer_->dimens());
}

void CybelEngine::show_error(const std::string& error) const {
  show_error(title_,error);
}

void CybelEngine::show_error(const std::string& title,const std::string& error) const {
  show_error_global(title,error,res_.window);
}

void CybelEngine::show_error_global(const std::string& title,const std::string& error) {
  show_error_global(title,error,NULL);
}

void CybelEngine::show_error_global(const std::string& title,const std::string& error,SDL_Window* window) {
  std::cerr << "[ERROR] " << error << std::endl;

  const std::size_t max_len = 80;

  // Avoid copy if possible.
  if(error.length() <= max_len) {
    // SDL_ShowSimpleMessageBox() can be called before/after SDL_Init()/SDL_Quit().
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,title.c_str(),error.c_str(),window);
  } else {
    const auto wrapped_error = Util::wrap_str(error,max_len);

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,title.c_str(),wrapped_error.c_str(),window);
  }
}

void CybelEngine::set_icon(const Image& img) { SDL_SetWindowIcon(res_.window,img.surface_); }

void CybelEngine::set_title(const std::string& title) { SDL_SetWindowTitle(res_.window,title.c_str()); }

void CybelEngine::reset_title() { set_title(title_); }

void CybelEngine::set_fullscreen(bool fullscreen,bool windowed) {
  const auto result = SDL_SetWindowFullscreen(
    res_.window,
    fullscreen ? (windowed ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN) : 0
  );

  if(result != 0) {
    std::string desc = fullscreen ? (windowed ? "windowed fullscreen" : "fullscreen") : "windowed";
    std::cerr << "[WARN] Failed to set window to [" << desc << "] with error [" << result << "]: "
              << Util::get_sdl_error() << '.' << std::endl;
  }
}

void CybelEngine::set_cursor_visible(bool visible) {
  SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
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

const std::string& CybelEngine::title() const { return title_; }

bool CybelEngine::is_fullscreen() const {
  const auto flags = SDL_GetWindowFlags(res_.window);

  return (flags & SDL_WINDOW_FULLSCREEN) || (flags & SDL_WINDOW_FULLSCREEN_DESKTOP);
}

bool CybelEngine::is_cursor_visible() const { return SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE; }

bool CybelEngine::is_vsync() const { return SDL_GL_GetSwapInterval() != 0; }

Renderer& CybelEngine::renderer() const { return *renderer_; }

const ViewDimens& CybelEngine::dimens() const { return renderer_->dimens(); }

Scene& CybelEngine::main_scene() const { return main_scene_; }

SceneMan& CybelEngine::scene_man() const { return *scene_man_; }

InputMan& CybelEngine::input_man() const { return *input_man_; }

AudioPlayer& CybelEngine::audio_player() const { return *audio_player_; }

int CybelEngine::target_fps() const { return target_fps_; }

const Duration& CybelEngine::target_dpf() const { return target_dpf_; }

const Duration& CybelEngine::dpf() const { return frame_step_.dpf; }

double CybelEngine::delta_time() const { return frame_step_.delta_time; }

} // Namespace.
