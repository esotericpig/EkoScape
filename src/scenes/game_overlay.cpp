/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_overlay.h"

namespace ekoscape {

const Color4f GameOverlay::kTextBgColor{0.0f,0.5f};
const Size2i GameOverlay::kTextBgPadding{15,10};
const Duration GameOverlay::kMapInfoDuration = Duration::from_millis(3'000);
const float GameOverlay::kGameOverLifespan = 3.0f; // Seconds.

GameOverlay::GameOverlay(const Assets& assets)
    : assets_(assets) {}

void GameOverlay::init_scene() {
  map_info_timer_.start();
}

bool GameOverlay::update_map_info() {
  return (map_info_timer_.end().duration() >= kMapInfoDuration);
}

void GameOverlay::update_game_over(const FrameStep& step) {
  if(game_over_age_ >= 1.0f) { return; }

  game_over_age_ += (static_cast<float>(step.delta_time) / kGameOverLifespan);
  if(game_over_age_ > 1.0f) { game_over_age_ = 1.0f; }
}

void GameOverlay::draw_map_info(Renderer& ren,const DantaresMap& map) {
  assets_.font_renderer().wrap(ren,{395,395},[&](auto& font) {
    const tiny_utf8::string title = map.title();
    const tiny_utf8::string author = "  by " + map.author();
    const auto bg_w = static_cast<int>(std::max(title.length(),author.length()));

    font.draw_bg(kTextBgColor,{bg_w,2},kTextBgPadding);
    font.puts(title);
    font.puts(author);
  });
}

void GameOverlay::draw_game_over(Renderer& ren,const DantaresMap& map,bool player_hit_end) {
  Color4f bg_color = kTextBgColor;
  const int total_rescues = map.total_rescues();
  const int total_cells = map.total_cells();
  const bool freed_all = (total_rescues >= total_cells);

  bg_color.a *= game_over_age_;

  ren.wrap_sprite(player_hit_end ? assets_.corngrits_sprite() : assets_.goodnight_sprite(),[&](auto& s) {
    ren.wrap_color({1.0f,game_over_age_},[&]() {
      s.draw_quad({10,10},{1200,450});
    });
  });
  assets_.font_renderer().wrap(ren,{460,460},0.60f,[&](auto& font) {
    font.font_color.a *= game_over_age_;

    const auto font_color = font.font_color;
    const Color4f miss_color{1.0f,0.0f,0.0f,font_color.a};
    const Color4f goal_color{0.0f,1.0f,0.0f,font_color.a};

    font.draw_bg(bg_color,{37,5},kTextBgPadding);
    font.puts(player_hit_end ? "Congrats!" : "You're dead!");

    font.print("You freed ");
    font.font_color = freed_all ? goal_color : miss_color;
    font.print(std::to_string(total_rescues));
    font.font_color = font_color;
    font.print(" eko");
    if(total_rescues != 1) { font.print('s'); }
    font.print(" out of ");
    font.font_color = goal_color;
    font.print(std::to_string(total_cells));
    font.font_color = font_color;
    font.print(" eko");
    if(total_cells != 1) { font.print('s'); }
    font.print('.');

    if(player_hit_end && freed_all) {
      font.puts_blanks(2);
      font.puts("You unlocked a secret!");
      font.print("Press ");
      font.font_color.set(1.0f,1.0f,0.0f,font_color.a);
      font.print("F");
      font.font_color = font_color;
      font.print(" key in the Credits scene.");
    }
  });

  assets_.font_renderer().wrap(ren,{580,790},[&](auto& font) {
    font.draw_bg(bg_color,{9,1},kTextBgPadding);
    font.font_color.a *= game_over_age_;
    font.draw_menu_opt("go back",FontRenderer::kMenuStyleSelected);
  });
}

} // Namespace.
