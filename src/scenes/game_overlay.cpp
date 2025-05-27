/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "game_overlay.h"

#include "cybel/str/utf8/str_util.h"

#include "core/input_action.h"
#include "scenes/scene_action.h"

namespace ekoscape {

GameOverlay::Option::Option(OptionType type,std::string_view text)
  : type(type),text(text) {}

GameOverlay::GameOverlay(GameContext& ctx,const Map& map)
  : ctx_(ctx),map_(map) {
  const std::string& title = map_.title();
  const std::string author = "  by " + map_.author();

  map_info_ = title + "\n" + author;
  map_info_str_size_.w = static_cast<int>(
    std::max(utf8::StrUtil::count_runes(title),utf8::StrUtil::count_runes(author))
  );
  map_info_str_size_.h = 2;
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

void GameOverlay::game_over(bool player_hit_end) {
  state_.player_hit_end = player_hit_end;

  if(game_over_age_ >= 0.0f) { return; }

  const bool perfect = (map_.total_rescues() >= map_.total_cells()) && state_.player_hit_end;

  game_over_age_ = 0.0f;
  game_over_opt_index_ = 0;

  if(!perfect) {
    game_over_opts_.emplace_back(OptionType::kPlayAgain,"play again");
    if(state_.player_hit_end) { game_over_opt_index_ = 1; } // Auto-select 'go back'.
  }

  game_over_opts_.emplace_back(OptionType::kGoBack,"go back");
}

void GameOverlay::update_state(const State& state) { state_ = state; }

void GameOverlay::on_scene_input_event(input_id_t input_id,const ViewDimens& /*dimens*/) {
  if(game_over_opts_.empty()) { return; }

  const Option& sel_opt = game_over_opts_.at(game_over_opt_index_);

  switch(input_id) {
    case InputAction::kSelect:
      switch(sel_opt.type) {
        case OptionType::kPlayAgain:
          scene_action_ = SceneAction::kRestart;
          break;
        case OptionType::kGoBack:
          scene_action_ = SceneAction::kGoBack;
          break;
      }
      break;

    case InputAction::kUp:
      if(game_over_opt_index_ >= 1) {
        --game_over_opt_index_;
      } else {
        game_over_opt_index_ = game_over_opts_.size() - 1; // Wrap to bottom.
      }
      break;

    case InputAction::kDown:
      if((game_over_opt_index_ + 1) < game_over_opts_.size()) {
        ++game_over_opt_index_;
      } else {
        game_over_opt_index_ = 0; // Wrap to top.
      }
      break;
  }
}

int GameOverlay::update_scene_logic(const FrameStep& step,const ViewDimens& dimens) {
  if(scene_action_ != SceneAction::kNil) {
    return std::exchange(scene_action_,SceneAction::kNil);
  }

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

  if(game_over_age_ >= 0.0f) {
    if(game_over_age_ < 1.0f) {
      game_over_age_ += static_cast<float>(step.delta_time / kGameOverDuration.secs());
      if(game_over_age_ > 1.0f) { game_over_age_ = 1.0f; }
    }

    if(state_.player_hit_end) {
      if(star_sys_.is_empty()) {
        star_sys_.init(dimens,true);
      } else {
        star_sys_.update(step);
      }
    }
  }

  return SceneAction::kNil;
}

void GameOverlay::draw_scene(Renderer& ren,const ViewDimens& dimens) {
  if(flash_age_ >= 0.0f) {
    flash_color_.a = kAlpha * flash_age_;

    ren.wrap_color(flash_color_,[&] {
      ren.draw_quad(Pos3i{0,0,0},dimens.size);
    });
  }
  if(fade_age_ >= 0.0f) {
    fade_color_.a = kAlpha * fade_age_;

    ren.wrap_color(fade_color_,[&] {
      ren.draw_quad(Pos3i{0,0,0},dimens.size);
    });
  }

  draw_map_info(ren);
  draw_game_over(ren);
}

void GameOverlay::draw_map_info(Renderer& ren) {
  if(!state_.is_map_info) { return; }

  ren.begin_auto_center_scale();

  ctx_.assets.font_renderer().wrap(ren,Pos3i{},[&](auto& font) {
    font.set_bg_padding(kTextBgPadding);

    const auto true_size = font.font.calc_total_size(map_info_str_size_);
    const Pos3i pos{
      std::max((ren.dimens().target_size.w - true_size.w) >> 1,0),
      std::max((ren.dimens().target_size.h - true_size.h) >> 1,0),
      0
    };

    font.font.init_pos = pos;
    font.font.pos = pos;

    font.draw_bg(kTextBgColor,map_info_str_size_);
    font.puts(map_info_);
  });

  ren.end_scale();
}

void GameOverlay::draw_game_over(Renderer& ren) {
  if(game_over_age_ < 0.0f) { return; }

  ren.begin_auto_center_scale();

  const auto bg_color = kTextBgColor.with_a(kTextBgColor.a * game_over_age_);
  const int total_rescues = map_.total_rescues();
  const int total_cells = map_.total_cells();
  const bool freed_all = (total_rescues >= total_cells);
  const bool perfect = freed_all && state_.player_hit_end;
  const auto* game_over_sprite = ctx_.assets.sprite(
    state_.player_hit_end ? SpriteId::kCorngrits : SpriteId::kGoodnight
  );

  ren.wrap_sprite(*game_over_sprite,[&](auto& s) {
    ren.wrap_color(Color4f{1.0f,game_over_age_},[&] {
      s.draw_quad(Pos3i{10,10,0},Size2i{1200,450});
    });
  });
  ctx_.assets.font_renderer().wrap(ren,Pos3i{445,450,0},0.60f,[&](auto& font) {
    font.set_bg_padding(kTextBgPadding);
    font.font_color.a *= game_over_age_;

    const auto font_color = font.font_color;
    const auto miss_color = ctx_.assets.font_renderer().cycle_arrow_color().with_a(font_color.a);
    const auto goal_color = ctx_.assets.font_renderer().arrow_color().with_a(font_color.a);

    font.draw_bg(bg_color,Size2i{37,perfect ? 5 : 2});
    font.puts(state_.player_hit_end ? "Congrats!" : "You're dead!");

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

  ctx_.assets.font_renderer().wrap(ren,Pos3i{565,perfect ? 780 : 680,0},[&](auto& font) {
    font.set_bg_padding(kTextBgPadding);
    font.draw_bg(bg_color,Size2i{12,static_cast<int>(game_over_opts_.size())});
    font.font_color.a *= game_over_age_;

    for(std::size_t i = 0; i < game_over_opts_.size(); ++i) {
      Option& opt = game_over_opts_[i];
      int styles = 0;

      if(i == game_over_opt_index_) { styles |= FontRenderer::kMenuStyleSelected; }

      font.draw_menu_opt(opt.text,styles);
    }
  });

  ren.end_scale()
     .begin_auto_scale()
     .begin_add_blend();
  star_sys_.draw(ren,ctx_.assets.star_tex());
  ren.end_blend()
     .end_scale();
}

float GameOverlay::game_over_age() const { return game_over_age_; }

} // namespace ekoscape
