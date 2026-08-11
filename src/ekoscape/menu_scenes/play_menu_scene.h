/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MENU_SCENES_PLAY_MENU_SCENE_H_
#define EKOSCAPE_MENU_SCENES_PLAY_MENU_SCENE_H_

#include "ekoscape/global.h"

#include "ekoscape/core/game_session.h"

#include <cybel/scenes/scene.h>
#include <cybel/scenes/scene_context.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ekoscape {

class PlayMenuScene final : public Scene {
public:
  explicit PlayMenuScene(GameSession& sesh,const AssetMan& assets);

  void on_scene_input_event(input_id_t input_id,SceneContext& ctx) override;

  void draw_scene(Renderer& ren,SceneContext& ctx) override;

private:
  class MapOption {
  public:
    std::string group{};
    std::filesystem::path file{};
    std::string title{};
    std::string text{};

    explicit MapOption() = default;
    explicit MapOption(std::string_view text);
  };

  /// Order of core groups.
  static inline std::unordered_map<std::string,int> kCoreGroupToPriority{
    {"user",1},
    {"fanmade",2},
    {"neo",3},
    {"classic",4},
  };

  static constexpr int kUpDownArrowIndent = 2;
  static constexpr std::uint8_t kNonMapOptCount = 2;
  static constexpr std::uint8_t kMaxMapOpts = 10;
  static constexpr std::uint8_t kMapOptsHalf2 = kMaxMapOpts >> 1;
  // If even number of options, no exact middle, so bias drawing of selected option towards top half,
  // since this is easier on the eyes.
  static constexpr std::uint8_t kMapOptsHalf1 = ((kMaxMapOpts % 2) == 0)
                                                ? (std::max<std::uint8_t>(kMapOptsHalf2,1) - 1)
                                                : kMapOptsHalf2;
  static constexpr std::uint8_t kMinMapOptsHalf = std::min(kMapOptsHalf1,kMapOptsHalf2);

  GameSession& sesh_;

  int map_opt_index_ = 0;
  std::vector<MapOption> map_opts_{};

  void glob_maps(const AssetMan& assets);
  void prev_map_opt_group();
  void next_map_opt_group();
  void select_map_opt(int index,bool wrap);
  void select_map();
};

} // namespace ekoscape
#endif
