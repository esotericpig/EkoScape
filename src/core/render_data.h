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
  GLfloat r = 0.0f;
  GLfloat g = 0.0f;
  GLfloat b = 0.0f;
  GLfloat a = 1.0f;

  Color4f();
  Color4f(GLfloat r,GLfloat g,GLfloat b,GLfloat a = 1.0f);
  // `int` so that it's not ambiguous (unlike `uint8_t`).
  Color4f(int r,int g,int b,int a = 255);

  Color4f& set_f(GLfloat r,GLfloat g,GLfloat b,GLfloat a = 1.0f);
  Color4f& set_i(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a = 255);
};

struct Pos2f {
  GLfloat x = 0.0f;
  GLfloat y = 0.0f;
};

struct Pos2i {
  int x = 0;
  int y = 0;
};

struct Pos4f {
  GLfloat x1 = 0.0f;
  GLfloat y1 = 0.0f;
  GLfloat x2 = 0.0f;
  GLfloat y2 = 0.0f;
};

struct Size2i {
  int w = 0;
  int h = 0;
};

struct ViewDimens {
  Size2i size{};
  Size2i target_size{};
  Size2i init_size{};
  float scale = 1.0f;
};

} // Namespace.
#endif
