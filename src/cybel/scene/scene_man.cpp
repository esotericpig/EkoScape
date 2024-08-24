/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "scene_man.h"

namespace cybel {

const SceneBag SceneMan::kEmptySceneBag{
  Scene::kNilType,
  std::make_shared<Scene>(), // Current scene should never be null.
  false,
};

SceneMan::SceneMan(SceneBuilder build_scene,SceneIniter init_scene)
    : build_scene_(build_scene),init_scene_(init_scene) {}

bool SceneMan::push_scene(int type) {
  SceneBag scene_bag = build_scene_(type);
  if(!scene_bag.scene) { return false; }

  SceneBag prev = curr_scene_bag_;
  set_scene(scene_bag);

  if(!prev.persist) { prev.scene = nullptr; }
  prev_scene_bags_.emplace_back(prev);

  return true;
}

bool SceneMan::pop_scene() {
  while(!prev_scene_bags_.empty()) {
    SceneBag prev = prev_scene_bags_.back();
    prev_scene_bags_.pop_back();

    if(prev.type == Scene::kNilType) { continue; }

    // Not persisted? (i.e., need to recreate)
    if(!prev.scene) {
      prev.scene = build_scene_(prev.type).scene;
      if(!prev.scene) { continue; }
    }

    set_scene(prev);
    return true;
  }

  set_scene(kEmptySceneBag);
  return false;
}

void SceneMan::pop_all_scenes() {
  prev_scene_bags_.clear();
  set_scene(kEmptySceneBag);
}

void SceneMan::set_scene(const SceneBag& scene_bag) {
  if(!scene_bag.scene) { throw CybelError{"Scene is null."}; }

  curr_scene_bag_->on_scene_exit();

  curr_scene_bag_ = scene_bag;
  init_scene_(*curr_scene_bag_.scene);
}

Scene& SceneMan::curr_scene() const { return *curr_scene_bag_.scene; }

int SceneMan::curr_scene_type() const { return curr_scene_bag_.type; }

} // Namespace.