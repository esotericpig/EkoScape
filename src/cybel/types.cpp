/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "types.h"

namespace cybel {

Color4f Color4f::hex(std::uint32_t rgb,std::uint8_t a) { return Color4f{}.set_hex(rgb,a); }

Color4f Color4f::bytes(std::uint8_t rgb,std::uint8_t a) { return Color4f{}.set_bytes(rgb,a); }

Color4f Color4f::bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a) {
  return Color4f{}.set_bytes(r,g,b,a);
}

Color4f::Color4f() {}

Color4f::Color4f(float rgb,float a) { set(rgb,a); }

Color4f::Color4f(float r,float g,float b,float a) { set(r,g,b,a); }

Color4f& Color4f::set(float rgb,float a) { return set(rgb,rgb,rgb,a); }

Color4f& Color4f::set(float r,float g,float b,float a) {
  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;
  return *this;
}

Color4f& Color4f::set_hex(std::uint32_t rgb,std::uint8_t a) {
  return set_bytes(
     static_cast<std::uint8_t>((rgb >> 16) & 0xff)
    ,static_cast<std::uint8_t>((rgb >>  8) & 0xff)
    ,static_cast<std::uint8_t>((rgb      ) & 0xff)
    ,a
  );
}

Color4f& Color4f::set_bytes(std::uint8_t rgb,std::uint8_t a) { return set_bytes(rgb,rgb,rgb,a); }

Color4f& Color4f::set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a) {
  return set(
     r / 255.0f
    ,g / 255.0f
    ,b / 255.0f
    ,a / 255.0f
  );
}

bool Range2i::in_range(int value) const { return value >= min && value <= max; }

} // Namespace.
