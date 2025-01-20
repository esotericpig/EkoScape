/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_MENU_SCENE_H_
#define EKOSCAPE_SCENES_MENU_SCENE_H_

#include "common.h"

#include "cybel/scene/scene.h"

#include "scenes/scene_action.h"
#include "game_context.h"

#include <functional>
#include <vector>

namespace ekoscape {

class MenuScene : public Scene {
public:
  explicit MenuScene(GameContext& ctx);

  void on_input_event(int action,const ViewDimens& dimens) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren,const ViewDimens& dimens) override;

private:
  class Option {
  public:
    using OnSelect = std::function<void()>;
    using OnUpdate = std::function<void(Option&)>;

    struct CycleConfig {
      OnUpdate on_update{};
      OnSelect on_select{};
      OnSelect on_select_alt{};
    };

    std::string text{};

    static Option cycle(const CycleConfig& config);
    explicit Option(std::string_view text,const OnSelect& on_select);

    void select();
    void select_alt();

    bool is_cycle() const;

  private:
    bool is_cycle_ = false;
    OnSelect on_select_{};
    OnSelect on_select_alt_{};
    OnUpdate on_update_{};

    explicit Option() = default;
  };

  GameContext& ctx_;
  int scene_action_ = SceneAction::kNil;

  std::vector<Option> opts_{};
  int opt_index_ = 0;
};

} // Namespace.
#endif
