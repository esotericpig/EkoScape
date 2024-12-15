/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_hud.h"

namespace ekoscape {

GameHud::GameHud(Assets& assets)
    : assets_(assets) {
  mini_map_eko_color_ = assets_.eko_color().with_a(kAlpha);
  mini_map_end_color_ = assets_.end_color().with_a(kAlpha);
  mini_map_fruit_color_ = assets_.fruit_color().with_a(kAlpha);
  mini_map_non_walkable_color_ = assets_.wall_color().with_a(kAlpha);
  mini_map_portal_color_ = assets_.portal_color().with_a(kAlpha);
  mini_map_robot_color_ = assets_.robot_color().with_a(kAlpha);
  mini_map_walkable_color_.set(0.0f,kAlpha);
}

void GameHud::draw(Renderer& ren,const ViewDimens& dimens,const Map& map,bool show_mini_map
    ,const Duration& player_fruit_time,bool player_hit_end) {
  ren.begin_auto_anchor_scale({0.0f,1.0f}); // Anchor HUD to bottom left.

  const int total_h = kMiniMapBlockSize.h + (show_mini_map ? kMiniMapSize.h : 0);
  Pos3i pos{0,dimens.target_size.h - total_h,0};
  const float text_scale = 0.33f;

  ren.wrap_color(mini_map_walkable_color_,[&]() {
    ren.draw_quad(pos,{kMiniMapSize.w,kMiniMapBlockSize.h});
  });
  assets_.font_renderer().wrap(ren,pos,text_scale,[&](auto& font) {
    const Color4f font_color = font.font_color;

    font.print();
    font.font_color = (map.total_rescues() < map.total_cells()) ? mini_map_eko_color_ : mini_map_end_color_;
    font.print(std::to_string(map.total_rescues()));
    font.font_color = font_color;
    font.print(Util::build_str('/',map.total_cells()," ekos"));
  });
  if(player_fruit_time > Duration::kZero) {
    const int fruit_padding_w = 5;
    const Pos3i fruit_pos{pos.x + kMiniMapSize.w + fruit_padding_w,pos.y,pos.z};

    assets_.font_renderer().wrap(ren,fruit_pos,text_scale,[&](auto& font) {
      const StrUtf8 fruit_text = std::to_string(player_fruit_time.round_secs());

      font.draw_bg(mini_map_walkable_color_,{static_cast<int>(fruit_text.length()),1},{fruit_padding_w,0});
      font.font_color = mini_map_fruit_color_.with_a(1.0f);
      font.print(fruit_text);
    });
  }

  if(!show_mini_map) {
    ren.end_scale();
    return;
  }

  pos.y += kMiniMapBlockSize.h;

  const Pos3i player_pos = map.player_pos();
  Pos3i block_pos = pos;

  for(int y = -kMiniMapHoodRadius.h; y <= kMiniMapHoodRadius.h; ++y,block_pos.y += kMiniMapBlockSize.h) {
    for(int x = -kMiniMapHoodRadius.w; x <= kMiniMapHoodRadius.w; ++x,block_pos.x += kMiniMapBlockSize.w) {
      Pos3i map_pos = player_pos;

      // "Rotate" the mini map according to the direction the player is facing.
      // - Remember that the grid is flipped vertically in Map for the Y calculations.
      switch(map.player_facing()) {
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

      const Space* space = map.space(map_pos);
      const SpaceType type = (space != nullptr) ? space->type() : SpaceType::kNil;
      Color4f* color = &mini_map_walkable_color_;

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

      if(!player_hit_end && (x == 0 && y == 0)) { // Player block?
        ren.begin_color(mini_map_eko_color_);
        ren.wrap_font_atlas(assets_.font_atlas(),block_pos,kMiniMapBlockSize,{},[&](auto& font) {
          font.print("↑");
        });
      }
    }

    block_pos.x = pos.x;
  }
  ren.end_color();

  ren.end_scale();
}

} // Namespace.
