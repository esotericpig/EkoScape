/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "render_data.h"

namespace ekoscape {

Color4f::Color4f() {}

Color4f::Color4f(float r,float g,float b,float a) { set_f(r,g,b,a); }

Color4f::Color4f(int r,int g,int b,int a) {
  set_i(
    static_cast<std::uint8_t>(r)
    ,static_cast<std::uint8_t>(g)
    ,static_cast<std::uint8_t>(b)
    ,static_cast<std::uint8_t>(a)
  );
}

Color4f& Color4f::set_f(float r,float g,float b,float a) {
  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;

  return *this;
}

Color4f& Color4f::set_i(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a) {
  this->r = r / 255.0f;
  this->g = g / 255.0f;
  this->b = b / 255.0f;
  this->a = a / 255.0f;

  return *this;
}

} // Namespace.
