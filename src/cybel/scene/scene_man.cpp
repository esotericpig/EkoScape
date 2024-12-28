/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "scene_man.h"

namespace cybel {

SceneMan::SceneMan(const SceneBuilder& build_scene,const SceneIniter& init_scene)
    : build_scene_(build_scene),init_scene_(init_scene) {}

bool SceneMan::push_scene(int type) {
  SceneBag scene = build_scene_(type);
  if(!scene.scene) { return false; }

  SceneBag prev = curr_scene_bag_;
  set_scene(scene);

  if(!prev.persist) { prev.scene = nullptr; }
  prev_scene_bags_.push_back(prev);

  return true;
}

bool SceneMan::pop_scene() {
  // Avoid setting scene to kEmptySceneBag over & over.
  if(prev_scene_bags_.empty()) { return false; }

  do {
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
  } while(!prev_scene_bags_.empty());

  set_scene(kEmptySceneBag);
  return false;
}

void SceneMan::pop_all_scenes() {
  if(prev_scene_bags_.empty()) { return; }

  prev_scene_bags_.clear();
  set_scene(kEmptySceneBag);
}

bool SceneMan::restart_scene() {
  SceneBag scene_bag = build_scene_(curr_scene_bag_.type);
  if(!scene_bag.scene) { return false; }

  set_scene(scene_bag);
  return true;
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
