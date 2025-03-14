/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "menu_scene.h"

#include "input/input_action.h"

namespace ekoscape {

MenuScene::Option MenuScene::Option::cycle(const CycleConfig& config) {
  Option opt{};
  opt.is_cycle_ = true;
  opt.on_select_ = config.on_select;
  opt.on_select_alt_ = config.on_select_alt;
  opt.on_update_ = config.on_update;

  if(opt.on_update_) { opt.on_update_(opt); } // Init.

  return opt;
}

MenuScene::Option::Option(std::string_view text,const OnSelect& on_select)
  : text(text),on_select_(on_select) {}

void MenuScene::Option::select() {
  if(on_select_) { on_select_(); }
  if(on_update_) { on_update_(*this); }
}

void MenuScene::Option::select_alt() {
  if(on_select_alt_) {
    on_select_alt_();
  } else if(on_select_) {
    on_select_();
  }
  if(on_update_) { on_update_(*this); }
}

bool MenuScene::Option::is_cycle() const { return is_cycle_; }

MenuScene::MenuScene(GameContext& ctx)
  : ctx_(ctx) {
  auto gfx_opt = Option::cycle({
    .on_update = [&](auto& opt) { opt.text = "gfx: " + ctx_.assets.tex_style(); },
    .on_select = [&] { ctx_.assets.next_tex_style(); },
    .on_select_alt = [&] { ctx_.assets.prev_tex_style(); },
  });
  auto vsync_opt = Option::cycle({
    .on_update = [&](auto& opt) {
      opt.text = "vsync: ";
      opt.text += (ctx_.cybel_engine.is_vsync() ? "on" : "off");
    },
    .on_select = [&] {
      ctx_.cybel_engine.set_vsync(!ctx_.cybel_engine.is_vsync());
    },
  });

  opts_ = {
    Option{"play",[&] { scene_action_ = SceneAction::kGoToMenuPlay; }},
    gfx_opt,
    vsync_opt,
    Option{"credits",[&] { scene_action_ = SceneAction::kGoToMenuCredits; }},
    #if !defined(__EMSCRIPTEN__)
      Option{"quit",[&] { scene_action_ = SceneAction::kQuit; }},
    #endif
  };
}

void MenuScene::on_input_event(int action,const ViewDimens& /*dimens*/) {
  Option& sel_opt = opts_.at(opt_index_);

  switch(action) {
    case InputAction::kSelect:
      sel_opt.select();
      break;

    case InputAction::kUp:
      if(opt_index_ > 0) {
        --opt_index_;
      } else if(!opts_.empty()) {
        opt_index_ = static_cast<int>(opts_.size()) - 1; // Wrap to bottom.
      }
      break;

    case InputAction::kDown:
      if(opt_index_ < (static_cast<int>(opts_.size()) - 1)) {
        ++opt_index_;
      } else {
        opt_index_ = 0; // Wrap to top.
      }
      break;

    case InputAction::kLeft:
      if(sel_opt.is_cycle()) { sel_opt.select_alt(); }
      break;

    case InputAction::kRight:
      if(sel_opt.is_cycle()) { sel_opt.select(); }
      break;
  }
}

int MenuScene::update_scene_logic(const FrameStep& /*step*/,const ViewDimens& /*dimens*/) {
  return std::exchange(scene_action_,SceneAction::kNil);
}

void MenuScene::draw_scene(Renderer& ren,const ViewDimens& /*dimens*/) {
  ren.begin_2d_scene()
     .begin_auto_center_scale()
     .begin_add_blend();

  ren.wrap_sprite(ctx_.assets.logo_sprite(),[&](auto& s) {
    s.draw_quad(Pos3i{150,10,0},Size2i{1300,300});
  });

  ctx_.assets.font_renderer().wrap(ren,Pos3i{395,330,0},[&](auto& font) {
    for(std::size_t i = 0; i < opts_.size(); ++i) {
      const Option& opt = opts_[i];
      int styles = 0;

      if(static_cast<int>(i) == opt_index_) {
        if(opt.is_cycle()) {
          styles |= FontRenderer::kMenuStyleCycle;
        } else {
          styles |= FontRenderer::kMenuStyleSelected;
        }
      }

      font.draw_menu_opt(opt.text,styles);
    }
  });

  ren.wrap_tex(ctx_.assets.robot_tex(),[&](auto& tex) {
    tex.draw_quad(Pos3i{10,368,0},Size2i{300,256});
  });
  ren.wrap_tex(ctx_.assets.cell_tex(),[&](auto& tex) {
    tex.draw_quad(Pos3i{10,634,0},Size2i{300,256});
  });
  ren.wrap_sprite(ctx_.assets.keys_sprite(),[&](auto& s) {
    s.draw_quad(Pos3i{804,560,0},Size2i{s.sprite.size().w / 2,s.sprite.size().h / 2});
  });

  ren.end_blend()
     .end_scale();
}

} // namespace ekoscape
