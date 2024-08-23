/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_SCENE_SCENE_BAG_H_
#define CYBEL_SCENE_SCENE_BAG_H_

#include "cybel/common.h"

#include "scene.h"

namespace cybel {

class SceneBag {
public:
  int type = -1;
  std::shared_ptr<Scene> scene{};
  bool persist = false;

  explicit SceneBag();
  explicit SceneBag(int type,std::shared_ptr<Scene> scene = nullptr,bool persist = false);

  explicit operator bool() const;
  Scene* operator->() const;
  Scene& operator*() const;
};

} // Namespace.
#endif
