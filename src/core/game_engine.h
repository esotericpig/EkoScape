/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_GAME_ENGINE_H_
#define EKOSCAPE_CORE_GAME_ENGINE_H_

#include "common.h"

#include "duration.h"
#include "ekoscape_error.h"
#include "music.h"
#include "scene.h"
#include "timer.h"
#include "util.h"

#include <cmath>

namespace ekoscape {

class GameEngine {
private:
  // This is necessary for RAII, since GameEngine() ctor can throw an exception.
  // I decided to do this over using `unique_ptr`s or individual wrappers.
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

public:
  struct Config {
    std::string title{};
    int width = kFallbackWidth;
    int height = kFallbackHeight;
    float scale_factor = 0.0f;
    int fps = kFallbackFps;
    bool vsync = false;
    Color4f clear_color{};

    /**
     * All:
     *   IMG_INIT_AVIF | IMG_INIT_JPG | IMG_INIT_JXL | IMG_INIT_PNG |
     *   IMG_INIT_TIF | IMG_INIT_WEBP
     *
     * See: https://wiki.libsdl.org/SDL2_image/IMG_Init
     */
    int image_types = IMG_INIT_PNG;

    /**
     * All:
     *   MIX_INIT_FLAC | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_MP3 |
     *   MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_WAVPACK
     *
     * For MIDI on Linux, need to install:
     *   timidity++ libtimidity-devel
     *
     * See: https://wiki.libsdl.org/SDL2_mixer/Mix_Init
     */
    int music_types = MIX_INIT_OGG;
  };

  static const int kFallbackWidth = 1600;
  static const int kFallbackHeight = 900;
  static const int kFallbackFps = 60;

  GameEngine(Scene& main_scene,const Config& config);
  GameEngine(const GameEngine& other) = delete;
  GameEngine(GameEngine&& other) noexcept = delete;

  GameEngine& operator=(const GameEngine& other) = delete;
  GameEngine& operator=(GameEngine&& other) noexcept = delete;

  void set_vsync(bool enable);
  void resize();
  void resize(int width,int height);
  void begin_2d_scene();
  void begin_3d_scene();

  void run();
  void request_stop();

  bool has_music_player() const;
  void play_music(const Music& music);
  void stop_music();
  bool is_music_playing() const;

  const Uint8* get_key_states() const;
  void clear_screen();

  void show_error(const std::string& error);
  void show_error(const std::string& title,const std::string& error);
  static void show_error_globally(const std::string& title,const std::string& error,SDL_Window* window = NULL);

  int target_width() const;
  int target_height() const;
  int width() const;
  int height() const;
  double view_scale() const;
  int target_fps() const;
  const Duration& target_dpf() const;
  const Duration& dpf() const;
  double delta_time() const;

private:
  Scene& main_scene_;
  std::string title_{};
  int target_width_ = 1;
  int target_height_ = 1;
  int width_ = 1;
  int height_ = 1;
  double view_scale_ = 1.0;
  int target_fps_ = 0;
  Duration target_dpf_{};
  Duration dpf_{};
  double delta_time_ = 0.0;
  Color4f clear_color_{};

  bool is_running_ = false;
  Timer frame_timer_{};

  void init_hints(const Config& config);
  void init_config(const Config& config);
  void init_gui(const std::string& title);
  void init_gl();
  void init_music_player(int music_types);

  void handle_events();
  void start_time();
  void end_time();
};

} // Namespace.
#endif
