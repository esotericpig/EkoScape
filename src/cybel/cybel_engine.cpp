/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cybel_engine.h"

#include "cybel/str/utf8/str_util.h"
#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

#if defined(CYBEL_RENDERER_GLES)
  #include "cybel/gfx/renderer_gles.h"
#else // CYBEL_RENDERER_GL
  #include "cybel/gfx/renderer_gl.h"
#endif

namespace cybel {

CybelEngine::Resources::~Resources() noexcept {
  if(context != NULL) {
    SDL_GL_DeleteContext(context);
    context = NULL;
  }
  if(window != NULL) {
    SDL_DestroyWindow(window);
    window = NULL;
  }

  IMG_Quit();
  SDL_Quit();
}

Size2i CybelEngine::calc_scaled_view(const Size2i& view,float scale_factor,const Size2i& target_size) {
  // Scaled size.
  float sw = static_cast<float>(view.w) * scale_factor;
  float sh = static_cast<float>(view.h) * scale_factor;

  // If target size set, preserve aspect ratio of target size.
  if(target_size.w > 0 && target_size.h > 0) {
    // Aspect ratio.
    const float ar = static_cast<float>(target_size.w) / static_cast<float>(target_size.h);
    const float ar_h = std::round(sw / ar);

    if(ar_h <= std::round(sh)) {
      sh = ar_h; // Adjust height based on width.
    } else {
      sw = sh * ar; // Adjust width based on height.
    }
  }

  return Size2i{
    static_cast<int>(std::round(sw)),
    static_cast<int>(std::round(sh))
  };
}

CybelEngine::CybelEngine(Scene& main_scene,Config config,const SceneMan::SceneBuilder& build_scene)
  : title_(config.title),is_vsync_(config.vsync),main_scene_(main_scene) {
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
  init_context();
  check_versions();

  #if defined(CYBEL_RENDERER_GLES)
    renderer_ = std::make_unique<RendererGles>(config.size,config.target_size,config.clear_color);
  #else // CYBEL_RENDERER_GL
    renderer_ = std::make_unique<RendererGl>(config.size,config.target_size,config.clear_color);
  #endif

  scene_man_ = std::make_unique<SceneMan>(build_scene,[&](Scene& scene) { init_scene(scene); });
  input_man_ = std::make_unique<InputMan>(config.max_input_id);
  audio_player_ = std::make_unique<AudioPlayer>(config.music_types);

  init_run();
}

void CybelEngine::init_hints() {
  //SDL_SetHint(SDL_HINT_APP_NAME,title_.c_str());
  SDL_SetHint(SDL_HINT_AUDIO_DEVICE_APP_NAME,title_.c_str());

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"nearest"); // One of: nearest, linear, best.
  //SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING,"1");

  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
  //SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS,"1");
  //SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS,"0");
}

void CybelEngine::init_config(Config& config) {
  Size2i size = config.size;

  // If getting the current display mode fails, we fall back to Config.size.
  //     Therefore, scale_factor needs to have priority over Config.size.
  if(config.scale_factor > 0.0f) {
    SDL_DisplayMode dm{};

    if(SDL_GetCurrentDisplayMode(0,&dm) != 0) {
      std::cerr << "[WARN] Failed to get current display mode: " << Util::get_sdl_error() << '.' << std::endl;
      // Don't fail; fall back to Config.size.
    } else if(dm.w > 0 && dm.h > 0) {
      size = calc_scaled_view(Size2i{dm.w,dm.h},config.scale_factor,config.target_size);
    }
  }

  config.size.w = (size.w > 0) ? size.w : kFallbackWidth;
  config.size.h = (size.h > 0) ? size.h : kFallbackHeight;
  config.target_size.w = (config.target_size.w > 0) ? config.target_size.w : config.size.w;
  config.target_size.h = (config.target_size.h > 0) ? config.target_size.h : config.size.h;

  // Allow 0 if the user wants to use delta time only (no delay).
  // - See: stop_frame_timer()
  target_fps_ = (config.fps >= 0) ? config.fps : kFallbackFps;

  if(target_fps_ > 0) { // Avoid divide by 0.
    // Convert from FPS to Duration (millis) Per Frame.
    target_dpf_.set_from_millis(std::round(1000.0 / static_cast<double>(target_fps_)));
  }
}

void CybelEngine::init_gui(const Config& config) {
  // NOTE: Must set GL attrs after SDL_Init() and before SDL_CreateWindow().
  #if defined(CYBEL_RENDERER_GLES)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
  #else // CYBEL_RENDERER_GL
    // Use a 2004-2008 version.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
  #endif

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

  if(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24) != 0) {
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
  }

  Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;

  // NOTE: There is a bug in SDL2 where SDL_SetWindowResizable() doesn't work to enable receiving resize
  //       events from the browser. We must explicitly pass SDL_WINDOW_RESIZABLE in the flags.
  #if defined(__EMSCRIPTEN__)
    window_flags |= SDL_WINDOW_RESIZABLE;
  #endif

  // With the SDL_WINDOW_ALLOW_HIGHDPI flag, the size might change after, therefore it's important that
  //     we call sync_size() later, which we do in init_run().
  res_.window = SDL_CreateWindow(
    title_.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,config.size.w,config.size.h,window_flags
  );

  if(res_.window == NULL) {
    throw CybelError{"Failed to create window: ",Util::get_sdl_error(),'.'};
  }

  // The SDL_WINDOW_RESIZABLE flag in SDL_CreateWindow() increases the size for some reason
  //     (even w/o SDL_WINDOW_ALLOW_HIGHDPI), but this explicit call doesn't.
  SDL_SetWindowResizable(res_.window,SDL_TRUE);
}

void CybelEngine::init_context() {
  res_.context = SDL_GL_CreateContext(res_.window);

  if(res_.context == NULL) {
    throw CybelError{"Failed to create OpenGL context: ",Util::get_sdl_error(),'.'};
  }

  has_context_ = true;

  const GLenum error = glewInit();

  if(error != GLEW_OK) {
    throw CybelError{"Failed to init OpenGL GLEW [",error,"]: ",Util::get_glew_error(error),'.'};
  }

  set_vsync(is_vsync_);
  Util::clear_gl_errors(); // Mainly for WebGL context restored.
}

void CybelEngine::check_versions() {
  #if defined(__EMSCRIPTEN__)
    // Output Emscripten version because different/newer versions can break the build,
    //     and Emscripten doesn't store the version anywhere in the generated files.
    std::cout << "[INFO] Emscripten version: " << __EMSCRIPTEN_major__ << '.' << __EMSCRIPTEN_minor__ << '.'
              << __EMSCRIPTEN_tiny__ << '.' << std::endl;
  #endif

  const auto gl_version_cstr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const std::string gl_version = (gl_version_cstr != nullptr)
                                 ? gl_version_cstr : "Failed to get OpenGL version";

  std::cout << "[INFO] OpenGL version: " << gl_version << '.' << std::endl;

  if(!GLEW_VERSION_2_1) {
    std::cerr << "[WARN] OpenGL version is < 2.1." << std::endl;

    const std::string msg = Util::build_str(
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

void CybelEngine::init_run() {
  is_running_ = true;

  // No need to init the current scene in scene_man_,
  //     since it would have already been called on push_scene().
  main_scene_.init_scene(renderer_->dimens());

  // Check the size again, due to SDL_WINDOW_ALLOW_HIGHDPI,
  //     and also need to call the scenes' resize() after init_scene().
  sync_size(true);
}

void CybelEngine::on_context_lost() {
  has_context_ = false;
  main_scene_.on_scene_exit();
  scene_man_->curr_scene().on_scene_exit();
}

void CybelEngine::restore_context() {
  init_context();
  renderer_->on_context_restored();

  // NOTE: Must call main scene first so that it can reload textures, etc.
  main_scene_.on_context_restored();
  scene_man_->curr_scene().on_context_restored();

  for(auto& bag : scene_man_->prev_scene_bags()) {
    if(bag.scene) { bag.scene->on_context_restored(); }
  }

  main_scene_.init_scene(renderer_->dimens());
  scene_man_->curr_scene().init_scene(renderer_->dimens());
  sync_size(true); // Call scenes' resize_scene().
}

void CybelEngine::run_loop() { while(run_frame()) {} }

bool CybelEngine::run_frame() {
  if(!is_running_) { return false; }

  if(!has_context_) {
    // Don't hog CPU while waiting for WebGL context to be restored.
    // - NOTE: Must use at least 30 FPS to avoid `[Violation] '...' handler took <N>ms`.
    SDL_Delay(33);
    start_frame_timer();
    return true;
  }

  // NOTE: For the Web, we must call stop_frame_timer() here and not at the end of this function.
  //       I'm not sure of the exact cause, but could be because of double/Uint32 [Duration/SDL_Delay()],
  //           Chrome's async rendering, and/or Timer [SDL_GetTicks64()].
  //       I can only recreate it on an older laptop (but with up-to-date software & Fedora Linux) with
  //           "graphics acceleration" turned off in Chrome.
  stop_frame_timer();
  start_frame_timer();

  input_man_->begin_input();
  handle_events();
  handle_input_states();

  // Event/Input requested to stop.
  if(!is_running_) { return false; }

  main_scene_.update_scene_logic(frame_step_,renderer_->dimens());
  const int scene_result = scene_man_->curr_scene().update_scene_logic(frame_step_,renderer_->dimens());

  if(scene_result != Scene::kNilType) {
    scene_man_->push_scene(scene_result);
  }

  renderer_->clear_view();
  main_scene_.draw_scene(*renderer_,renderer_->dimens());
  scene_man_->curr_scene().draw_scene(*renderer_,renderer_->dimens());
  SDL_GL_SwapWindow(res_.window);

  return true;
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

void CybelEngine::stop_frame_timer() {
  frame_step_.dpf = frame_timer_.peek();

  // If target_dpf_ (target_fps_) is 0, then will use delta time only (no delay).
  if(frame_step_.dpf < target_dpf_) {
    SDL_Delay((target_dpf_ - frame_step_.dpf).round_millis());
  }

  frame_step_.dpf = frame_timer_.pause();
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
        std::cout << "[EVENT] Received Quit event." << std::endl;
        request_stop();
        return;

      case SDL_WINDOWEVENT:
        switch(event.window.event) {
          case SDL_WINDOWEVENT_RESIZED:
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            should_resize = true;
            break;
        }
        break;

      default:
        input_man_->handle_event(event,on_input_event_);
        break;
    }
  }

  if(!is_running_) { return; }
  if(should_resize) { sync_size(false); }
}

void CybelEngine::handle_input_event(int id) {
  main_scene_.on_input_event(id,renderer_->dimens());
  scene_man_->curr_scene().on_input_event(id,renderer_->dimens());
}

void CybelEngine::handle_input_states() {
  input_man_->update_states();

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

  constexpr std::size_t max_len = 80;

  // Avoid copy if possible.
  if(error.length() <= max_len) {
    // SDL_ShowSimpleMessageBox() can be called before/after SDL_Init()/SDL_Quit().
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,title.c_str(),error.c_str(),window);
  } else {
    const auto wrapped_error = utf8::StrUtil::wrap_words(error,max_len);

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
    const std::string desc = fullscreen ? (windowed ? "windowed fullscreen" : "fullscreen") : "windowed";
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

  is_vsync_ = (SDL_GL_GetSwapInterval() != 0);
}

const std::string& CybelEngine::title() const { return title_; }

bool CybelEngine::is_fullscreen() const {
  const auto flags = SDL_GetWindowFlags(res_.window);

  return (flags & SDL_WINDOW_FULLSCREEN) || (flags & SDL_WINDOW_FULLSCREEN_DESKTOP);
}

bool CybelEngine::is_cursor_visible() const { return SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE; }

bool CybelEngine::is_vsync() const { return is_vsync_; }

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

} // namespace cybel
