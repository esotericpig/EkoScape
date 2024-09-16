/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_SCENE_SCENE_MAN_H_
#define CYBEL_SCENE_SCENE_MAN_H_

#include "cybel/common.h"

#include "cybel/util/cybel_error.h"
#include "scene.h"
#include "scene_bag.h"

#include <functional>

namespace cybel {

class SceneMan {
public:
  using SceneBuilder = std::function<SceneBag(int type)>;
  using SceneIniter = std::function<void(Scene&)>;

  static inline const SceneBag kEmptySceneBag{
    Scene::kNilType,
    std::make_shared<Scene>(), // Current scene should never be null.
    false,
  };

  explicit SceneMan(const SceneBuilder& build_scene,const SceneIniter& init_scene);

  bool push_scene(int type);
  bool pop_scene();
  void pop_all_scenes();

  Scene& curr_scene() const;
  int curr_scene_type() const;

private:
  SceneBuilder build_scene_{};
  SceneIniter init_scene_{};

  SceneBag curr_scene_bag_ = kEmptySceneBag;
  std::vector<SceneBag> prev_scene_bags_{};

  void set_scene(const SceneBag& scene_bag);
};

} // Namespace.
#endif
