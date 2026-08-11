/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_GAME_SCENE_GAME_SCENE_H_
#define EKOSCAPE_GAME_SCENE_GAME_SCENE_H_

#include "ekoscape/global.h"

#include "ekoscape/core/game_session.h"
#include "ekoscape/game_scene/game_hud.h"
#include "ekoscape/game_scene/game_overlay.h"
#include "ekoscape/maps/map.h"
#include "ekoscape/maps/space_type.h"
#include "ekoscape/world/robot.h"

#include <cybel/assets/asset_types.h>
#include <cybel/chrono/duration.h>
#include <cybel/chrono/ticker.h>
#include <cybel/chrono/timer.h>
#include <cybel/math/pos.h>
#include <cybel/scenes/scene.h>
#include <cybel/scenes/scene_context.h>
#include <Dantares/Dantares2.h>

#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace ekoscape {

class GameScene final : public Scene {
public:
  explicit GameScene(GameSession& sesh,Renderer& ren,AssetMan& assets,const std::filesystem::path& map_file);

  void on_scene_enter(SceneContext& ctx) override;
  void on_scene_exit(SceneContext& ctx) override;

  void on_scene_gpu_context_restore(SceneContext& ctx) override;

  void on_scene_input_event(input_id_t input_id,SceneContext& ctx) override;
  void handle_scene_input(InputMan& input,SceneContext& ctx) override;

  void update_scene_logic(const FrameDelta& delta,SceneContext& ctx) override;
  void draw_scene(Renderer& ren,SceneContext& ctx) override;

private:
  enum class GamePhase {
    kShowMapInfo,
    kPlay,
    kGameOver,
  };

  struct StoredInputs {
    bool is_down = false;
    bool is_left = false;
    bool is_right = false;
  };

  using MoveChecker = std::function<bool(const Pos3i&)>;

  static constexpr int kDantaresDist = 24; // Must be 2+.
  static constexpr Duration kMapInfoDuration = Duration::from_millis(2'500);
  static constexpr Duration kInitExtraRobotDelay = Duration::from_millis(1'000);
  static constexpr std::uint32_t kFruitWarnSecs = 2;

  GameSession& sesh_;
  std::unique_ptr<Dantares2::RendererClass> dantares_renderer_{};
  std::unique_ptr<Dantares2> dantares_{};
  std::unique_ptr<Map> map_{};

  GamePhase game_phase_ = GamePhase::kShowMapInfo;
  Timer map_info_timer_{};

  StoredInputs stored_inputs_{};
  bool player_hit_end_ = false;
  bool player_already_warped_ = false; /// Prevents infinite warping.
  ChronoTicker player_warp_ticker_{Duration::from_millis(750),TickerFlag::kFireAsap};
  /// Starts at end so that time left is 0.
  ChronoTicker player_fruit_ticker_{Duration::from_secs(7.0f),TickerFlag::kFireAsap};
  std::uint32_t last_player_fruit_warn_secs_{};
  Timer speedrun_timer_{};

  std::vector<Robot> robots_{};
  Duration robot_move_time_{};
  std::unique_ptr<Robot::MoveData> robot_move_data_{};
  std::unordered_map<SpaceType,std::vector<Pos3i>> portal_to_pos_bag_{};

  std::unique_ptr<GameHud> hud_{};
  std::unique_ptr<GameOverlay> overlay_{};

  void init_map(const std::filesystem::path& map_file);
  SpaceType init_map_space(const Pos3i& pos,SpaceType type,std::vector<Pos3i>& cells);
  void init_map_default_empty(const Pos3i& pos,SpaceType type);
  void make_map_weird(std::vector<Pos3i>& cells);
  void init_map_textures(AssetMan& assets);

  void update_player(const FrameDelta& delta,const SceneContext& ctx);
  void game_over(bool player_hit_end,const SceneContext& ctx);

  void update_robots(const FrameDelta& delta);
  void move_robots(const FrameDelta& delta);
  void remove_robots_at(const Pos3i& pos);
  std::optional<Pos3i> fetch_portal_bro(const Pos3i& pos,SpaceType portal,const MoveChecker& can_move_to);

  void update_mods(const FrameDelta& delta,const SceneContext& ctx);

  void set_space_textures(AssetMan& assets,SpaceType type,asset_id_t id);
  void set_space_textures(AssetMan& assets,SpaceType type,std::optional<asset_id_t> ceiling_id,
                          std::optional<asset_id_t> wall_id,std::optional<asset_id_t> floor_id);
};

} // namespace ekoscape
#endif
