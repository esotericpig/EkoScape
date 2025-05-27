/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_hud.h"

#include "scenes/scene_action.h"

#include <sstream>

namespace ekoscape {

GameHud::GameHud(GameContext& ctx,const Map& map)
  : ctx_(ctx),map_(map) {
  mini_map_eko_color_ = ctx_.assets.eko_color().with_a(kAlpha);
  mini_map_end_color_ = ctx_.assets.end_color().with_a(kAlpha);
  mini_map_fruit_color_ = ctx_.assets.fruit_color().with_a(kAlpha);
  mini_map_non_walkable_color_ = ctx_.assets.wall_color().with_a(kAlpha);
  mini_map_portal_color_ = ctx_.assets.portal_color().with_a(kAlpha);
  mini_map_robot_color_ = ctx_.assets.robot_color().with_a(kAlpha);
  mini_map_walkable_color_.set(0.0f,kAlpha);

  update_speedrun_time_str();
}

void GameHud::update_state(const State& state) { state_ = state; }

int GameHud::update_scene_logic(const FrameStep& step,const ViewDimens& /*dimens*/) {
  // Update the speedrun time str on Game Over or at an interval.
  if(state_.speedrun_time != last_speedrun_time_ &&
     (state_.is_game_over || (last_updated_speedrun_time_ += step.dpf).millis() >= 100.0)) {
    last_updated_speedrun_time_.set_to_zero();
    last_speedrun_time_ = state_.speedrun_time;
    update_speedrun_time_str();
  }

  return SceneAction::kNil;
}

void GameHud::update_speedrun_time_str() {
  // Round to a precision of 2.
  const auto total_secs = std::round(state_.speedrun_time.secs() * 100.0) / 100.0;
  const auto total_whole_secs = static_cast<int>(total_secs);

  const auto whole_millis = static_cast<int>((total_secs - total_whole_secs) * 100.0);
  const auto whole_secs = total_whole_secs % 60;
  const auto whole_mins = total_whole_secs / 60;

  std::ostringstream buffer{};

  if(whole_mins > 0) {
    if(whole_mins < 10) { buffer << '0'; }
    buffer << whole_mins << ':';
  }

  if(whole_secs < 10) { buffer << '0'; }
  buffer << whole_secs << '.';

  if(whole_millis < 10) { buffer << '0'; }
  buffer << whole_millis;

  speedrun_time_str_ = buffer.str();
}

void GameHud::draw_scene(Renderer& ren,const ViewDimens& dimens) {
  draw_map_mod(ren,dimens);

  // Always show the speedrun time on Game Over.
  if(state_.show_speedrun || state_.is_game_over) { draw_speedrun_mod(ren,dimens); }
}

void GameHud::draw_map_mod(Renderer& ren,const ViewDimens& dimens) {
  ren.begin_auto_anchor_scale(Pos2f{0.0f,1.0f}); // Anchor to bottom left.

  const int total_h = kMiniMapBlockSize.h + (state_.show_mini_map ? kMiniMapSize.h : 0);
  const Pos3i pos{0,dimens.target_size.h - total_h,0};

  ren.wrap_color(mini_map_walkable_color_,[&] {
    ren.draw_quad(pos,Size2i{kMiniMapSize.w,kMiniMapBlockSize.h});
  });
  ctx_.assets.font_renderer().wrap(ren,pos,kTextScale,[&](auto& font) {
    const Color4f font_color = font.font_color;

    font.print();
    font.font_color = (map_.total_rescues() < map_.total_cells())
                      ? mini_map_eko_color_ : mini_map_end_color_;
    font.print(std::to_string(map_.total_rescues()));
    font.font_color = font_color;
    font.print(Util::build_str('/',map_.total_cells()," ekos"));
  });
  if(state_.player_fruit_time > Duration::kZero) {
    const Pos3i fruit_pos{pos.x + kMiniMapSize.w,pos.y,pos.z};

    ctx_.assets.font_renderer().wrap(ren,fruit_pos,kTextScale,[&](auto& font) {
      const auto fruit_text = std::to_string(state_.player_fruit_time.round_secs());

      font.set_bg_padding(Size2i{5,0});
      font.draw_bg(mini_map_walkable_color_,Size2i{static_cast<int>(fruit_text.length()),1});
      font.font_color = mini_map_fruit_color_.with_a(1.0f);
      font.print(fruit_text);
    });
  }

  if(state_.show_mini_map) { draw_mini_map(ren,pos); }

  ren.end_scale();
}

void GameHud::draw_mini_map(Renderer& ren,Pos3i pos) {
  pos.y += kMiniMapBlockSize.h;

  const Pos3i player_pos = map_.player_pos();
  Pos3i block_pos = pos;

  for(int y = -kMiniMapHoodRadius.h; y <= kMiniMapHoodRadius.h; ++y,block_pos.y += kMiniMapBlockSize.h) {
    for(int x = -kMiniMapHoodRadius.w; x <= kMiniMapHoodRadius.w; ++x,block_pos.x += kMiniMapBlockSize.w) {
      Pos3i map_pos = player_pos;

      // "Rotate" the mini map according to the direction the player is facing.
      // - Remember that the grid is flipped vertically in Map for the Y calculations.
      switch(map_.player_facing()) {
        case Facing::kNorth:
          map_pos.x += x;
          map_pos.y -= y;
          break;

        case Facing::kSouth:
          map_pos.x -= x;
          map_pos.y += y;
          break;

        case Facing::kEast:
          map_pos.x -= y;
          map_pos.y -= x;
          break;

        case Facing::kWest:
          map_pos.x += y;
          map_pos.y += x;
          break;
      }

      const Space* space = map_.space(map_pos);
      const SpaceType type = (space != nullptr) ? space->type() : SpaceType::kNil;
      const Color4f* color = &mini_map_walkable_color_;

      switch(type) {
        case SpaceType::kCell:
          color = &mini_map_eko_color_;
          break;

        case SpaceType::kEnd:
          color = &mini_map_end_color_;
          break;

        case SpaceType::kFruit:
          color = &mini_map_fruit_color_;
          break;

        default:
          if(SpaceTypes::is_robot(type)) {
            color = &mini_map_robot_color_;
          } else if(SpaceTypes::is_portal(type)) {
            color = &mini_map_portal_color_;
          } else if(SpaceTypes::is_non_walkable(type)) {
            color = &mini_map_non_walkable_color_;
          }
          break;
      }

      ren.begin_color(*color);
      ren.draw_quad(block_pos,kMiniMapBlockSize);

      if(!state_.player_hit_end && (x == 0 && y == 0)) { // Player block?
        ren.begin_color(mini_map_eko_color_);
        ren.wrap_font_atlas(
          ctx_.assets.font_atlas(),block_pos,kMiniMapBlockSize,Size2i{0,0},
          [&](auto& font) { font.print("↑"); }
        );
      }
    }

    block_pos.x = pos.x;
  }
  ren.end_color();
}

void GameHud::draw_speedrun_mod(Renderer& ren,const ViewDimens& dimens) {
  ren.begin_auto_anchor_scale(Pos2f{1.0f,1.0f}); // Anchor to bottom right.

  ctx_.assets.font_renderer().wrap(ren,Pos3i{},kTextScale,[&](auto& font) {
    font.set_bg_padding(Size2i{10,5});

    const Size2i str_size{static_cast<int>(speedrun_time_str_.length()),1};
    const auto total_size = font.font.calc_total_size(str_size);

    font.font.pos.x += (dimens.target_size.w - total_size.w);
    font.font.pos.y += (dimens.target_size.h - total_size.h);

    font.draw_bg(mini_map_walkable_color_,str_size);
    font.print(speedrun_time_str_);
  });

  ren.end_scale();
}

} // namespace ekoscape
