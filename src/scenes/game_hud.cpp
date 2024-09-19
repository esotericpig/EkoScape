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
  mini_map_eko_color_.set_hex(0xff0000,kAlpha);
  mini_map_end_color_.set_hex(0xb87333,kAlpha); // Copper.
  mini_map_non_walkable_color_.set_hex(0x00ff00,kAlpha);
  mini_map_portal_color_.set_hex(0x00ffff,kAlpha); // Cyan.
  mini_map_robot_color_.set_bytes(214,kAlpha);
  mini_map_walkable_color_.set_bytes(0,kAlpha);

  if(assets.is_weird()) {
    std::swap(mini_map_eko_color_.r,mini_map_eko_color_.b);
    std::swap(mini_map_end_color_.r,mini_map_end_color_.b);
    std::swap(mini_map_portal_color_.r,mini_map_portal_color_.b);
  }
}

void GameHud::draw(Renderer& ren,const DantaresMap& map,bool show_mini_map,bool player_hit_end) {
  ren.begin_auto_anchor_scale({0.0f,1.0f}); // Anchor HUD to bottom left.

  const int total_h = kMiniMapBlockSize.h + (show_mini_map ? kMiniMapSize.h : 0);
  Pos3i pos{10,ren.dimens().target_size.h - 10 - total_h};

  ren.wrap_color(mini_map_walkable_color_,[&]() {
    ren.draw_quad(pos,{kMiniMapSize.w,kMiniMapBlockSize.h});
  });
  assets_.font_renderer().wrap(ren,pos,0.33f,[&](auto& font) {
    font.print();

    const Color4f font_color = font.font_color;

    font.font_color = (map.total_rescues() < map.total_cells()) ? mini_map_eko_color_ : mini_map_end_color_;
    font.print(std::to_string(map.total_rescues()));

    font.font_color = font_color;
    font.print(Util::build_str('/',map.total_cells()," ekos"));
  });

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
