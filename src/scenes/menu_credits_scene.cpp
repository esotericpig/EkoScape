/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "menu_credits_scene.h"

#include "cybel/util/rando.h"

#include "input/input_action.h"

namespace ekoscape {

Color4f MenuCreditsScene::rand_color() {
  auto& r = Rando::it();

  return Color4f{r.rand_float(),r.rand_float(),r.rand_float()};
}

MenuCreditsScene::MenuCreditsScene(GameContext& ctx)
    : ctx_(ctx),wtfs_(75,WtfParticle{}) {}

void MenuCreditsScene::on_input_event(int action,const ViewDimens& dimens) {
  switch(action) {
    case InputAction::kSelect:
      scene_action_ = SceneAction::kGoBack;
      break;

    // Shhh... Don't Tell.
    case InputAction::kMakeWeird:
      ctx_.assets.make_weird();
      init_wtfs(dimens);
      break;
  }
}

int MenuCreditsScene::update_scene_logic(const FrameStep& step,const ViewDimens& dimens) {
  update_wtfs(step,dimens);

  return std::exchange(scene_action_,SceneAction::kNil);
}

void MenuCreditsScene::draw_scene(Renderer& ren,const ViewDimens& /*dimens*/) {
  ren.begin_2d_scene()
     .begin_auto_center_scale()
     .begin_add_blend();

  int x = 15;
  int y = 10;
  int right_x = x + 800;

  ren.wrap_sprite(ctx_.assets.logo_sprite(),[&](auto& s) {
    s.draw_quad(Pos3i{x,y,0},Size2i{780,180}); // 1300x300.
  });
  ren.wrap_sprite(ctx_.assets.dantares_sprite(),[&](auto& s) {
    s.draw_quad(Pos3i{right_x,y,0},Size2i{780,156}); // 600x120.
  });
  x += 35;
  y += 190;

  ctx_.assets.font_renderer().wrap(ren,Pos3i{x,y,0},0.75f,[&](auto& font) {
    font.print("by Bradley Whited");
    font.font.pos.x = right_x;
    font.print("by Ryan Witmer");

    font.puts();
    y = font.font.pos.y;
  });
  ctx_.assets.font_renderer().wrap(ren,Pos3i{x,y,0},0.45f,[&](auto& font) {
    font.print("github.com/esotericpig/EkoScape");
    font.font.pos.x = right_x;
    font.print("https://phasercat.com");

    font.puts_blanks(2);
    y = font.font.pos.y;
  });

  ctx_.assets.font_renderer().wrap(ren,Pos3i{x,y,0},0.60f,[&](auto& font) {
    font.puts("Monogram font by datagoblin.itch.io");
    font.puts("Star textures by Kronbits.itch.io");
  });

  ctx_.assets.font_renderer().wrap(ren,Pos3i{395,615,0},[&](auto& font) {
    font.draw_menu_opt("go back",FontRenderer::kMenuStyleSelected);
  });

  ren.end_blend() // Else, the WTFs are too bright.
     .end_scale();

  ren.begin_auto_scale();
  draw_wtfs(ren);
  ren.end_scale();
}

void MenuCreditsScene::init_wtfs(const ViewDimens& dimens) {
  auto& r = Rando::it();
  int max_births = (active_wtf_count_ <= 20) ? 25 : 5;
  const auto init_x = static_cast<float>(dimens.target_size.w) / 2.0f;
  const auto init_y = static_cast<float>(dimens.target_size.h) / 2.0f;
  const auto init_w = static_cast<float>(ctx_.assets.font_renderer().font_size().w);
  const auto init_h = static_cast<float>(ctx_.assets.font_renderer().font_size().h);
  const auto size = static_cast<int>(wtfs_.size());

  for(int i = active_wtf_count_; i < size; ++i,++active_wtf_count_) {
    WtfParticle& wtf = wtfs_[i];

    wtf.lifespan = 3.0f;
    wtf.age = 0.0f;
    wtf.past_lives = 0;

    wtf.pos.x = init_x;
    wtf.pos.y = init_y;
    wtf.pos_vel.x = r.rand_float_vel(75.0f) * 3.0f;
    wtf.pos_vel.y = r.rand_float_vel(75.0f) * 2.0f;
    wtf.spin_angle = r.rand_float_vel(30.0f);
    wtf.spin_vel = r.rand_float_vel(30.0f);

    const float size_scale = r.rand_float(0.1f,2.5f);

    wtf.baby_size.w = init_w * size_scale;
    wtf.baby_size.h = init_h * size_scale;
    wtf.elder_size.w = wtf.baby_size.w * 1.2f;
    wtf.elder_size.h = wtf.baby_size.h * 1.2f;
    wtf.baby_color = rand_color();
    wtf.elder_color = rand_color();

    wtf.birth();
    if((--max_births) <= 0) { break; }
  }
}

void MenuCreditsScene::update_wtfs(const FrameStep& step,const ViewDimens& dimens) {
  const auto text_len = static_cast<float>(kWtfText.length());
  const Size2f font_spacing = ctx_.assets.font_renderer().font_spacing().to_size2<float>();
  const float total_spacing_w = font_spacing.w * (text_len - 1);

  for(int i = 0; i < active_wtf_count_; ++i) {
    WtfParticle& wtf = wtfs_.at(i);

    if(wtf.is_dead() && wtf.past_lives >= 1) {
      // Set this index to last active element.
      --active_wtf_count_;
      wtfs_[i] = wtfs_.at(active_wtf_count_);
      --i; // Reprocess this index, since now an active element.
      continue;
    }

    wtf.age_by(static_cast<float>(step.delta_time));

    if(wtf.is_dead()) {
      wtf.fade();
      wtf.elder_color.a = 0.0f;
      wtf.rebirth();
    }

    // Adjust pos & size for number of chars in text,
    //     since wtf.size is just for a single char [see init_wtfs()].
    wtf.true_size.w = (wtf.size.w * text_len) + total_spacing_w;
    wtf.true_size.h = wtf.size.h;
    wtf.true_pos.x = wtf.pos.x - (wtf.true_size.w / 2.0f);
    wtf.true_pos.y = wtf.render_pos.y;

    // Because of rotation, use max for both width & height for in_bounds().
    const auto s = static_cast<int>(std::max(wtf.true_size.w,wtf.true_size.h));

    if(!dimens.target_size.in_bounds(wtf.true_pos.to_pos2<int>(),Size2i{s,s})) {
      wtf.die().past_lives = 1;
      --i; // Reprocess this index to actually remove it from active count.
    }
  }
}

void MenuCreditsScene::draw_wtfs(Renderer& ren) {
  ctx_.assets.font_renderer().wrap(ren,Pos3i{},[&](auto& font) {
    for(int i = 0; i < active_wtf_count_; ++i) {
      WtfParticle& wtf = wtfs_[i];

      font.font.pos = wtf.true_pos.to_pos3<int>();
      font.font.char_size = wtf.size.to_size2<int>();
      font.font_color = wtf.color;

      ren.wrap_rotate(wtf.pos.to_pos3<int>(),wtf.spin_angle,[&]() {
        font.print(kWtfText);
      });
    }
  });
}

} // Namespace.
