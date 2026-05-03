/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "menu_scene.h"

#include "core/input_action.h"
#include "scenes/scene_action.h"

namespace ekoscape {

MenuScene::MenuScene(GameSession& sesh,SceneContext& ctx)
  : sesh_{sesh} {
  auto gfx_opt = Option::cycle(ctx,{
    .on_update = [this](auto& opt,auto&) { opt.text = "gfx: " + sesh_.assets.tex_style(); },
    .on_select = [this](auto&) { sesh_.assets.next_tex_style(); },
    .on_select_alt = [this](auto&) { sesh_.assets.prev_tex_style(); },
  });
  auto vsync_opt = Option::cycle(ctx,{
    .on_update = [](auto& opt,auto& opt_ctx) {
      opt.text = "vsync: ";
      opt.text += (opt_ctx.engine.is_vsync() ? "on" : "off");
    },
    .on_select = [](auto& opt_ctx) {
      opt_ctx.engine.set_vsync(!opt_ctx.engine.is_vsync());
    },
  });

  opts_ = {
    Option{"play",[](auto& opt_ctx) { opt_ctx.scene_man.push_scene(SceneAction::kGoToMenuPlay); }},
    gfx_opt,
    vsync_opt,
    Option{"credits",[](auto& opt_ctx) { opt_ctx.scene_man.push_scene(SceneAction::kGoToMenuCredits); }},
#if !defined(__EMSCRIPTEN__)
    Option{"quit",[](auto& opt_ctx) { opt_ctx.scene_man.push_scene(SceneAction::kQuit); }},
#endif
  };
}

void MenuScene::on_scene_input_event(input_id_t input_id,SceneContext& ctx) {
  if(opts_.empty()) { return; }

  Option& sel_opt = opts_.at(opt_index_);

  switch(input_id) {
    case InputAction::kSelect:
      sel_opt.select(ctx);
      break;

    case InputAction::kUp:
      if(opt_index_ >= 1) {
        --opt_index_;
      } else {
        opt_index_ = opts_.size() - 1; // Wrap to bottom.
      }
      break;

    case InputAction::kDown:
      if((opt_index_ + 1) < opts_.size()) {
        ++opt_index_;
      } else {
        opt_index_ = 0; // Wrap to top.
      }
      break;

    case InputAction::kLeft:
      if(sel_opt.is_cycle()) { sel_opt.select_alt(ctx); }
      break;

    case InputAction::kRight:
      if(sel_opt.is_cycle()) { sel_opt.select(ctx); }
      break;
  }
}

void MenuScene::draw_scene(Renderer& ren,SceneContext& ctx) {
  ren.begin_2d_scene()
     .begin_auto_center_scale()
     .begin_add_blend();

  ren.wrap_sprite(*sesh_.assets.sprite(SpriteId::kEkoScapeLogo),[&](auto& s) {
    s.draw_quad(Pos3i{150,10,0},Size2i{1300,300});
  });

  sesh_.assets.font_renderer().wrap(ren,Pos3i{395,330,0},[&](auto& font) {
    for(std::size_t i = 0; i < opts_.size(); ++i) {
      const Option& opt = opts_[i];
      int styles = 0;

      if(i == opt_index_) {
        if(opt.is_cycle()) {
          styles |= FontRenderer::kMenuStyleCycle;
        } else {
          styles |= FontRenderer::kMenuStyleSelected;
        }
      }

      font.draw_menu_opt(opt.text,styles);
    }
  });

  ren.wrap_tex(*sesh_.assets.styled_tex(StyledTexId::kRobot),[&](auto& tex) {
    tex.draw_quad(Pos3i{10,368,0},Size2i{300,256});
  });
  ren.wrap_tex(*sesh_.assets.styled_tex(StyledTexId::kCell),[&](auto& tex) {
    tex.draw_quad(Pos3i{10,634,0},Size2i{300,256});
  });
  ren.wrap_sprite(*sesh_.assets.sprite(SpriteId::kKeys),[&](auto& s) {
    constexpr int padding = 10;
    const Size2i size{s.sprite.size().w / 2,s.sprite.size().h / 2};
    const Pos3i pos{
      ctx.dimens.target_size.w - size.w - padding,
      ctx.dimens.target_size.h - size.h - padding,
      0
    };

    s.draw_quad(pos,size);
  });

  ren.end_blend()
     .end_scale();
}

MenuScene::Option MenuScene::Option::cycle(SceneContext& ctx,const CycleConfig& config) {
  Option opt{};
  opt.is_cycle_ = true;
  opt.on_update_ = config.on_update;
  opt.on_select_ = config.on_select;
  opt.on_select_alt_ = config.on_select_alt;

  if(opt.on_update_) { opt.on_update_(opt,ctx); } // Init.

  return opt;
}

MenuScene::Option::Option(std::string_view text,const OnSelect& on_select)
  : text{text},on_select_{on_select} {}

void MenuScene::Option::select(SceneContext& ctx) {
  if(on_select_) { on_select_(ctx); }
  if(on_update_) { on_update_(*this,ctx); }
}

void MenuScene::Option::select_alt(SceneContext& ctx) {
  if(on_select_alt_) {
    on_select_alt_(ctx);
  } else if(on_select_) {
    on_select_(ctx);
  }
  if(on_update_) { on_update_(*this,ctx); }
}

bool MenuScene::Option::is_cycle() const { return is_cycle_; }

} // namespace ekoscape
