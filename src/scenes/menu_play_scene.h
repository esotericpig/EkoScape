/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_MENU_PLAY_SCENE_H_
#define EKOSCAPE_SCENES_MENU_PLAY_SCENE_H_

#include "common.h"

#include "cybel/scene/scene.h"
#include "cybel/util/rando.h"
#include "cybel/util/util.h"

#include "map/map.h"
#include "game_context.h"
#include "scene_action.h"

#include <algorithm>
#include <filesystem>
#include <vector>

namespace ekoscape {

class MenuPlayScene : public Scene {
public:
  struct State {
    std::filesystem::path map_file{};
    bool is_rand_map = true;
  };

  explicit MenuPlayScene(GameContext& ctx,State& state);

  void on_key_down_event(const KeyEvent& event,const ViewDimens& dimens) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren,const ViewDimens& dimens) override;

private:
  class MapOption {
  public:
    StrUtf8 group{};
    std::filesystem::path file{};
    StrUtf8 title{};
    StrUtf8 text{};

    explicit MapOption() noexcept = default;
    explicit MapOption(const StrUtf8& text);
  };

  /**
   * Order of core groups.
   */
  static inline std::unordered_map<StrUtf8,int> kCoreGroupToPriority{
    {"user",1},
    {"fanmade",2},
    {"neo",3},
    {"classic",4},
  };

  static inline const int kUpDownArrowIndent = 2;
  static inline const int kMaxMapOpts = 10;
  // If kMaxMapOpts is even, no exact middle, so bias drawing of selected option towards top half,
  //     since this is easier on the eyes.
  static inline const int kMapOptsHalf1 = std::max((kMaxMapOpts >> 1) - 1,1);
  static inline const int kMapOptsHalf2 = kMaxMapOpts >> 1;
  static inline const int kMinMapOptsHalf = std::min(kMapOptsHalf1,kMapOptsHalf2);

  GameContext& ctx_;
  State& state_;
  int scene_action_ = SceneAction::kNil;

  int map_opt_index_ = 0;
  std::vector<MapOption> map_opts_{};

  void refresh_maps();
  void glob_maps();
  void prev_map_opt_group();
  void next_map_opt_group();
  void sync_map_opt();
  void select_map_opt(int index,bool wrap,bool force = false);
};

} // Namespace.
#endif
