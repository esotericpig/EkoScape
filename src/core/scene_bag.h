/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_SCENE_BAG_H_
#define EKOSCAPE_CORE_SCENE_BAG_H_

#include "common.h"

#include "scene.h"

namespace cybel {

class SceneBag {
public:
  static const int kEmptyType = 0;
  static const SceneBag kEmpty;

  int type = -1;
  std::shared_ptr<Scene> scene{};
  bool persist = false;

  SceneBag();
  explicit SceneBag(int type,std::shared_ptr<Scene> scene = nullptr,bool persist = false);

  operator bool() const;
  Scene* operator->() const;
  Scene& operator*() const;
};

} // Namespace.
#endif
