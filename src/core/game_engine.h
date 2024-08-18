/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_GAME_ENGINE_H_
#define EKOSCAPE_CORE_GAME_ENGINE_H_

#include "common.h"

#include "audio/music.h"
#include "util/duration.h"
#include "util/timer.h"
#include "util/util.h"
#include "cybel_error.h"
#include "renderer.h"
#include "scene.h"

#include <cmath>
#include <functional>

namespace cybel {

class GameEngine {
public:
  struct Config {
    std::string title{};
    float scale_factor = 0.0f;
    Size2i size = {kFallbackWidth,kFallbackHeight};
    Size2i target_size = {0,0};
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

  using SceneBuilder = std::function<std::shared_ptr<Scene>(int type)>;

  static const int kFallbackWidth = 1600;
  static const int kFallbackHeight = 900;
  static const int kFallbackFps = 60;

  GameEngine(Scene& main_scene,Config config,SceneBuilder build_scene);
  GameEngine(const GameEngine& other) = delete;
  GameEngine(GameEngine&& other) noexcept = delete;
  virtual ~GameEngine() noexcept = default;

  GameEngine& operator=(const GameEngine& other) = delete;
  GameEngine& operator=(GameEngine&& other) noexcept = delete;

  void set_vsync(bool enable);
  void fetch_size_and_resize(bool force = true);
  void resize();
  void resize(const Size2i& size,bool force = true);

  bool push_scene(int type);
  bool pop_scene();
  void pop_all_scenes();

  void run();
  void request_stop();

  bool has_music_player() const;
  void play_music(const Music& music);
  void stop_music();
  bool is_music_playing() const;

  const Uint8* get_key_states() const;

  void show_error(const std::string& error);
  void show_error(const std::string& title,const std::string& error);
  static void show_error_global(const std::string& title,const std::string& error,SDL_Window* window = NULL);

  Scene& main_scene();
  std::shared_ptr<Scene> curr_scene() const;
  int curr_scene_type() const;
  const std::string& title() const;
  Renderer& renderer() const;
  const ViewDimens& dimens() const;
  int target_fps() const;
  const Duration& target_dpf() const;
  const Duration& dpf() const;
  double delta_time() const;

private:
  /**
   * This must be defined first so that its dtor is called last.
   *
   * This is necessary for RAII, since GameEngine() ctor can throw an exception.
   * I decided to do this over using `unique_ptr`s or individual wrappers.
   */
  class Resources {
  public:
    SDL_Window* window = NULL;
    SDL_GLContext context = NULL;
    bool has_music_player = false;

    Resources() noexcept;
    Resources(const Resources& other) = delete;
    Resources(Resources&& other) noexcept = delete;
    virtual ~Resources() noexcept;

    Resources& operator=(const Resources& other) = delete;
    Resources& operator=(Resources&& other) noexcept = delete;
  } res_{};

private:
  Scene& main_scene_;
  std::shared_ptr<Scene> curr_scene_{std::make_shared<Scene>()}; // Never null.
  int curr_scene_type_ = 0;
  std::vector<int> prev_scene_types_{};
  SceneBuilder build_scene_{};

  std::string title_{};
  std::unique_ptr<Renderer> renderer_{};
  int target_fps_ = 0;
  Duration target_dpf_{};
  Duration dpf_{};
  double delta_time_ = 0.0;

  bool is_running_ = false;
  Timer frame_timer_{};

  void init_hints(const Config& config);
  void init_config(Config& config);
  void init_gui(const Config& config);
  void init_renderer(const Config& config);
  void init_music_player(const Config& config);

  bool set_scene(int type);
  void start_frame_timer();
  void end_frame_timer();
  void handle_events();
};

} // Namespace.
#endif
