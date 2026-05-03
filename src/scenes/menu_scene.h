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
#include "cybel/scene/scene_context.h"

#include "core/game_session.h"

#include <functional>
#include <vector>

namespace ekoscape {

class MenuScene final : public Scene {
public:
  explicit MenuScene(GameSession& sesh,SceneContext& ctx);

  void on_scene_input_event(input_id_t input_id,SceneContext& ctx) override;

  void draw_scene(Renderer& ren,SceneContext& ctx) override;

private:
  class Option {
  public:
    using OnUpdate = std::function<void(Option&,SceneContext&)>;
    using OnSelect = std::function<void(SceneContext&)>;

    struct CycleConfig {
      OnUpdate on_update{};
      OnSelect on_select{};
      OnSelect on_select_alt{};
    };

    std::string text{};

    static Option cycle(SceneContext&,const CycleConfig& config);
    explicit Option(std::string_view text,const OnSelect& on_select);

    void select(SceneContext& ctx);
    void select_alt(SceneContext& ctx);

    bool is_cycle() const;

  private:
    bool is_cycle_ = false;
    OnUpdate on_update_{};
    OnSelect on_select_{};
    OnSelect on_select_alt_{};

    explicit Option() = default;
  };

  GameSession& sesh_;

  std::vector<Option> opts_{};
};

} // namespace ekoscape
#endif
