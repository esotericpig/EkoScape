/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_overlay.h"

namespace ekoscape {

GameOverlay::GameOverlay(Assets& assets)
    : assets_(assets) {}

void GameOverlay::init(const ViewDimens& dimens) {
  view_dimens_ = dimens;

  map_info_timer_.resume();
}

void GameOverlay::flash(const Color4f& color) {
  flash_color_ = color;
  flash_age_ = 0.0f;
  flash_age_dir_ = 1.0f;
}

void GameOverlay::fade_to(const Color4f& color) {
  fade_color_ = color;
  fade_age_ = 0.0f;
}

void GameOverlay::game_over(const DantaresMap& map,bool player_hit_end) {
  if(game_over_age_ >= 0.0f) { return; }

  const bool perfect = (map.total_rescues() >= map.total_cells()) && player_hit_end;

  game_over_age_ = 0.0f;
  game_over_opt_index_ = 0;

  if(!perfect) {
    game_over_opts_.emplace_back(OptionType::kPlayAgain,"play again");
    if(player_hit_end) { game_over_opt_index_ = 1; } // Auto-select 'go back'.
  }

  game_over_opts_.emplace_back(OptionType::kGoBack,"go back");
}

int GameOverlay::on_key_down_event(SDL_Keycode key) {
  const int game_over_opt_count = static_cast<int>(game_over_opts_.size());

  if(game_over_opt_index_ < 0 || game_over_opt_index_ >= game_over_opt_count) {
    return SceneAction::kNil;
  }

  const Option sel_opt = game_over_opts_.at(game_over_opt_index_);

  switch(key) {
    case SDLK_RETURN:
    case SDLK_SPACE:
    case SDLK_KP_ENTER:
      switch(sel_opt.type) {
        case OptionType::kPlayAgain: return SceneAction::kRestart;
        case OptionType::kGoBack: return SceneAction::kGoBack;
      }
      break;

    case SDLK_UP:
    case SDLK_w:
      if(game_over_opt_index_ > 0) {
        --game_over_opt_index_;
      } else if(game_over_opt_count > 0) {
        game_over_opt_index_ = game_over_opt_count - 1; // Wrap to bottom.
      }
      break;

    case SDLK_DOWN:
    case SDLK_s:
      if(game_over_opt_index_ < (game_over_opt_count - 1)) {
        ++game_over_opt_index_;
      } else {
        game_over_opt_index_ = 0; // Wrap to top.
      }
      break;
  }

  return SceneAction::kNil;
}

void GameOverlay::update(const FrameStep& step) {
  if(flash_age_ >= 0.0f) {
    flash_age_ += (static_cast<float>(step.delta_time / kFlashDuration.secs()) * flash_age_dir_);

    if(flash_age_ > 1.0f) {
      flash_age_ = 1.0f;
      flash_age_dir_ = -flash_age_dir_;
    } else if(flash_age_ < 0.0f) {
      flash_age_ = -1.0f;
      flash_age_dir_ = 0.0f;
    }
  }
  if(fade_age_ >= 0.0f && fade_age_ < 1.0f) {
    fade_age_ += static_cast<float>(step.delta_time / kFadeDuration.secs());
    if(fade_age_ > 1.0f) { fade_age_ = 1.0f; }
  }
}

bool GameOverlay::update_map_info() {
  return (map_info_timer_.end().duration() >= kMapInfoDuration);
}

void GameOverlay::update_game_over(const FrameStep& step,bool player_hit_end) {
  if(game_over_age_ < 1.0f) {
    game_over_age_ += static_cast<float>(step.delta_time / kGameOverDuration.secs());
    if(game_over_age_ > 1.0f) { game_over_age_ = 1.0f; }
  }

  if(player_hit_end) {
    if(star_sys_.is_empty()) { star_sys_.init(view_dimens_,true); }
    star_sys_.update(step);
  }
}

void GameOverlay::draw(Renderer& ren) {
  if(flash_age_ >= 0.0f) {
    flash_color_.a = kAlpha * flash_age_;

    ren.wrap_color(flash_color_,[&]() {
      ren.draw_quad({0,0,0},ren.dimens().size);
    });
  }
  if(fade_age_ >= 0.0f) {
    fade_color_.a = kAlpha * fade_age_;

    ren.wrap_color(fade_color_,[&]() {
      ren.draw_quad({0,0,0},ren.dimens().size);
    });
  }
}

void GameOverlay::draw_map_info(Renderer& ren,const DantaresMap& map) {
  ren.begin_auto_center_scale();

  assets_.font_renderer().wrap(ren,{395,395,0},[&](auto& font) {
    const CybelStrUtf8 title = map.title();
    const CybelStrUtf8 author = "  by " + map.author();
    const auto bg_w = static_cast<int>(std::max(title.length(),author.length()));

    font.draw_bg(kTextBgColor,{bg_w,2},kTextBgPadding);
    font.puts(title);
    font.puts(author);
  });

  ren.end_scale();
}

void GameOverlay::draw_game_over(Renderer& ren,const DantaresMap& map,bool player_hit_end) {
  ren.begin_auto_center_scale();

  const Color4f bg_color = kTextBgColor.with_a(kTextBgColor.a * game_over_age_);
  const int total_rescues = map.total_rescues();
  const int total_cells = map.total_cells();
  const bool freed_all = (total_rescues >= total_cells);
  const bool perfect = freed_all && player_hit_end;

  ren.wrap_sprite(player_hit_end ? assets_.corngrits_sprite() : assets_.goodnight_sprite(),[&](auto& s) {
    ren.wrap_color({1.0f,game_over_age_},[&]() {
      s.draw_quad({10,10,0},{1200,450});
    });
  });
  assets_.font_renderer().wrap(ren,{460,460,0},0.60f,[&](auto& font) {
    font.font_color.a *= game_over_age_;

    const Color4f font_color = font.font_color;
    const Color4f miss_color = assets_.font_renderer().cycle_arrow_color().with_a(font_color.a);
    const Color4f goal_color = assets_.font_renderer().arrow_color().with_a(font_color.a);

    font.draw_bg(bg_color,{37,perfect ? 5 : 2},kTextBgPadding);
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

    if(perfect) {
      font.puts_blanks(2);
      font.puts("You've unlocked a secret!");
      font.print("Press ");
      font.font_color.set(1.0f,1.0f,0.0f,font_color.a);
      font.print("F");
      font.font_color = font_color;
      font.print(" in Credits to pay respects.");
    }
  });

  assets_.font_renderer().wrap(ren,{580,perfect ? 790 : 690,0},[&](auto& font) {
    const int opt_count = static_cast<int>(game_over_opts_.size());

    font.draw_bg(bg_color,{12,opt_count},kTextBgPadding);
    font.font_color.a *= game_over_age_;

    for(int i = 0; i < opt_count; ++i) {
      Option& opt = game_over_opts_[i];
      int styles = 0;

      if(i == game_over_opt_index_) { styles |= FontRenderer::kMenuStyleSelected; }

      font.draw_menu_opt(opt.text,styles);
    }
  });

  ren.end_scale()
     .begin_auto_scale()
     .begin_add_blend();
  star_sys_.draw(ren,assets_.star_texture());
  ren.end_blend()
     .end_scale();
}

float GameOverlay::game_over_age() const { return game_over_age_; }

} // Namespace.
