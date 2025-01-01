/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_CYBEL_ENGINE_H_
#define CYBEL_CYBEL_ENGINE_H_

#include "common.h"

#include "audio/audio_player.h"
#include "gfx/image.h"
#include "render/render_types.h"
#include "render/renderer.h"
#include "scene/scene.h"
#include "scene/scene_man.h"
#include "util/cybel_error.h"
#include "util/duration.h"
#include "util/timer.h"
#include "util/util.h"

#include <cmath>

namespace cybel {

class CybelEngine {
private:
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
    Color4f clear_color{};

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

  static inline const int kFallbackWidth = 1600;
  static inline const int kFallbackHeight = 900;
  static inline const int kFallbackFps = 60;

  explicit CybelEngine(Scene& main_scene,Config config,const SceneMan::SceneBuilder& build_scene);

  CybelEngine(const CybelEngine& other) = delete;
  CybelEngine(CybelEngine&& other) noexcept = delete;
  virtual ~CybelEngine() noexcept = default;

  CybelEngine& operator=(const CybelEngine& other) = delete;
  CybelEngine& operator=(CybelEngine&& other) noexcept = delete;

  void run();
  void request_stop();

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
  AudioPlayer& audio_player() const;

  int target_fps() const;
  const Duration& target_dpf() const;
  const Duration& dpf() const;
  double delta_time() const;

private:
  std::string title_{};
  std::unique_ptr<Renderer> renderer_{};
  Scene& main_scene_;
  std::unique_ptr<SceneMan> scene_man_{}; // Must be defined after renderer_.

  bool is_running_ = false;
  Timer frame_timer_{};
  int target_fps_ = 0;
  Duration target_dpf_{};
  Duration dpf_{};
  double delta_time_ = 0.0;

  void init_hints();
  void init_config(Config& config);
  void init_gui(const Config& config);
  void init_renderer(const Config& config,const SceneMan::SceneBuilder& build_scene);
  void init_scene(Scene& scene);

  void start_frame_timer();
  void end_frame_timer();
  void handle_events();

  static void show_error_global(const std::string& title,const std::string& error,SDL_Window* window);
};

} // Namespace.
#endif
