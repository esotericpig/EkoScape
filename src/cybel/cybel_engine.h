/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_CYBEL_ENGINE_H_
#define CYBEL_CYBEL_ENGINE_H_

#include "cybel/common.h"

#include "cybel/audio/audio_player.h"
#include "cybel/gfx/image.h"
#include "cybel/gfx/renderer.h"
#include "cybel/input/input_man.h"
#include "cybel/scene/scene.h"
#include "cybel/scene/scene_man.h"
#include "cybel/types/color.h"
#include "cybel/types/duration.h"
#include "cybel/types/frame_step.h"
#include "cybel/types/size.h"
#include "cybel/types/view_dimens.h"
#include "cybel/util/timer.h"

namespace cybel {

class CybelEngine {
  /**
   * This is necessary for RAII, since CybelEngine() ctor can throw an exception.
   * I decided to do this over using `unique_ptr`s or individual wrappers.
   */
  // NOTE: This must be defined first so that its dtor is called last.
  class Resources {
  public:
    SDL_Window* window = NULL;
    SDL_GLContext context = NULL;

    explicit Resources() noexcept = default;

    Resources(const Resources& other) = delete;
    Resources(Resources&& other) noexcept = delete;
    virtual ~Resources() noexcept;

    Resources& operator=(const Resources& other) = delete;
    Resources& operator=(Resources&& other) noexcept = delete;
  } res_{};

  std::unique_ptr<AudioPlayer> audio_player_{};

public:
  struct Config {
    std::string title{};
    float scale_factor = 0.0f;
    Size2i size{kFallbackWidth,kFallbackHeight};
    Size2i target_size{0,0};
    int fps = kFallbackFps;
    bool vsync = false;
    Color4f clear_color{0.0f,1.0f};
    int max_input_id = 0;

    /**
     * All:
     *   IMG_INIT_AVIF | IMG_INIT_JPG  | IMG_INIT_JXL | IMG_INIT_PNG |
     *   IMG_INIT_TIF  | IMG_INIT_WEBP
     *
     * See: https://wiki.libsdl.org/SDL2_image/IMG_Init
     */
    int image_types = IMG_INIT_PNG;

    /**
     * All:
     *   MIX_INIT_FLAC | MIX_INIT_MID  | MIX_INIT_MOD     | MIX_INIT_MP3 |
     *   MIX_INIT_OGG  | MIX_INIT_OPUS | MIX_INIT_WAVPACK
     *
     * For MIDI on Linux, need to install:
     *   timidity++ libtimidity-devel
     *
     * See: https://wiki.libsdl.org/SDL2_mixer/Mix_Init
     */
    int music_types = MIX_INIT_OGG;
  };

  static constexpr int kFallbackWidth = 1600;
  static constexpr int kFallbackHeight = 900;
  static constexpr int kFallbackFps = 60;

  explicit CybelEngine(Scene& main_scene,Config config,const SceneMan::SceneBuilder& build_scene);

  CybelEngine(const CybelEngine& other) = delete;
  CybelEngine(CybelEngine&& other) noexcept = delete;
  virtual ~CybelEngine() noexcept = default;

  CybelEngine& operator=(const CybelEngine& other) = delete;
  CybelEngine& operator=(CybelEngine&& other) noexcept = delete;

  void run_loop();
  void run_on_web(std::shared_ptr<CybelEngine> cybel_engine);
  bool run_frame();
  void request_stop();

  void on_context_lost();
  void restore_context();

  void sync_size(bool force = true);
  void resize(const Size2i& size,bool force = true);

  void show_error(const std::string& error) const;
  void show_error(const std::string& title,const std::string& error) const;
  static void show_error_global(const std::string& title,const std::string& error);

  void set_icon(const Image& img);
  void set_title(const std::string& title);
  void reset_title();
  void set_fullscreen(bool fullscreen,bool windowed = true);
  void set_cursor_visible(bool visible);
  void set_vsync(bool enable);

  const std::string& title() const;
  bool is_fullscreen() const;
  bool is_cursor_visible() const;
  bool is_vsync() const;

  Renderer& renderer() const;
  const ViewDimens& dimens() const;
  Scene& main_scene() const;
  SceneMan& scene_man() const;
  InputMan& input_man() const;
  AudioPlayer& audio_player() const;

  int target_fps() const;
  const Duration& target_dpf() const;
  const Duration& dpf() const;
  double delta_time() const;
  float avg_fps() const;

private:
  static constexpr float kAvgFpsSmoothing = 0.3f; // Smoothing factor. Usually from 0.1 to 0.3.

  void handle_input_event(int input_id);

  std::string title_{};
  int target_fps_ = 0;
  Duration target_dpf_{};
  float avg_fps_{};
  bool is_vsync_ = false;

  std::unique_ptr<Renderer> renderer_{};
  Scene& main_scene_;
  std::unique_ptr<SceneMan> scene_man_{}; // Must be defined after `renderer_`.
  std::unique_ptr<InputMan> input_man_{};
  InputMan::OnInputEvent on_input_event_ = [&](int id) { handle_input_event(id); };

  bool is_running_ = false;
  Timer frame_timer_{};
  FrameStep frame_step_{};

  static Size2i calc_scaled_view(const Size2i& view,float scale_factor,const Size2i& target_size);

  void init_hints();
  void init_config(Config& config);
  void init_gui(const Config& config);
  void init_context();
  void check_versions();
  void init_scene(Scene& scene);
  void init_run();

  void start_frame_timer();
  void stop_frame_timer();
  void handle_events();
  void handle_non_context_events_only();
  void handle_input_states();

  static void show_error_global(const std::string& title,const std::string& error,SDL_Window* window);
};

} // namespace cybel
#endif
