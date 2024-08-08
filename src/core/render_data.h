/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_RENDER_DATA_H_
#define EKOSCAPE_CORE_RENDER_DATA_H_

#include "common.h"

namespace ekoscape {

/**
 * Clamped float to [0,1].
 */
struct Color4f {
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  float a = 1.0f;

  Color4f();
  Color4f(float r,float g,float b,float a = 1.0f);
  // `int` so that it's not ambiguous (unlike `uint8_t`).
  Color4f(int r,int g,int b,int a = 255);

  Color4f& set_f(float r,float g,float b,float a = 1.0f);
  Color4f& set_i(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a = 255);
};

struct Point2i {
  int x = 0;
  int y = 0;
};

struct Size2i {
  int w = 1;
  int h = 1;
};

struct ViewDimens {
  Size2i size{};
  Size2i target_size{};
  Size2i init_size{};
  float scale = 1.0f;
};

} // Namespace.
#endif
