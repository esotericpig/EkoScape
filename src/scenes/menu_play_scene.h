/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_MENU_PLAY_SCENE_H_
#define EKOSCAPE_SCENES_MENU_PLAY_SCENE_H_

#include "cybel/common.h"

#include "cybel/scene/scene.h"
#include "cybel/util/rando.h"
#include "cybel/util/util.h"
#include "cybel/cybel_engine.h"

#include "map/map.h"
#include "assets.h"
#include "scene_action.h"

#include <algorithm>
#include <filesystem>
#include <functional>
#include <sstream>
#include <vector>

namespace ekoscape {

class MenuPlayScene : public Scene {
public:
  using MapSelector = std::function<void(const std::filesystem::path&,bool is_rand)>;

  explicit MenuPlayScene(CybelEngine& cybel_engine,Assets& assets,const std::filesystem::path& sel_map_file
      ,bool is_rand_map,const MapSelector& select_map);

  void on_key_down_event(SDL_Keycode key) override;
  int update_scene_logic(const FrameStep& step) override;
  void draw_scene(Renderer& ren) override;

private:
  class MapOption {
  public:
    tiny_utf8::string group{};
    std::filesystem::path file{};
    tiny_utf8::string title{};
    tiny_utf8::string text{};

    explicit MapOption() noexcept = default;
    explicit MapOption(const tiny_utf8::string& text);
  };

  static const int kUpDownArrowIndent = 2;
  static const int kMaxMapOpts = 10;
  // If kMaxMapOpts is even, no exact middle; bias drawing of selected option towards top half,
  //     since this is easier on the eyes.
  static const int kMapOptsHalf1 = std::max((kMaxMapOpts >> 1) - 1,1);
  static const int kMapOptsHalf2 = kMaxMapOpts >> 1;
  static const int kMinMapOptsHalf = std::min(kMapOptsHalf1,kMapOptsHalf2);

  CybelEngine& cybel_engine_;
  Assets& assets_;
  MapSelector select_map_{};
  int scene_action_ = SceneAction::kNil;

  int map_opt_index_ = 0;
  std::vector<MapOption> map_opts_{};

  void refresh_maps();
  void refresh_maps(const std::filesystem::path& sel_map_file,bool is_rand_map);
  void glob_maps();
  void prev_map_opt_group();
  void next_map_opt_group();
  void select_map_opt(bool force = false);
  void select_map_opt(int index,bool force = false);
};

} // Namespace.
#endif
