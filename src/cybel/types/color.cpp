/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "color.h"

namespace cybel {

Color4f Color4f::hex(std::uint32_t rgb,std::uint8_t a) noexcept { return Color4f{}.set_hex(rgb,a); }

Color4f Color4f::bytes(std::uint8_t rgb,std::uint8_t a) noexcept { return Color4f{}.set_bytes(rgb,a); }

Color4f Color4f::bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a) noexcept {
  return Color4f{}.set_bytes(r,g,b,a);
}

Color4f::Color4f(float rgb,float a) noexcept { set(rgb,a); }

Color4f::Color4f(float r,float g,float b,float a) noexcept { set(r,g,b,a); }

Color4f& Color4f::set(float rgb) { return set(rgb,a); }

Color4f& Color4f::set(float rgb,float a) { return set(rgb,rgb,rgb,a); }

Color4f& Color4f::set(float r,float g,float b) { return set(r,g,b,a); }

Color4f& Color4f::set(float r,float g,float b,float a) {
  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;

  return *this;
}

Color4f& Color4f::set_hex(std::uint32_t rgb) {
  return set_bytes(
    static_cast<std::uint8_t>((rgb >> 16) & 0xff),
    static_cast<std::uint8_t>((rgb >>  8) & 0xff),
    static_cast<std::uint8_t>((rgb      ) & 0xff)
  );
}

Color4f& Color4f::set_hex(std::uint32_t rgb,std::uint8_t a) {
  return set_bytes(
    static_cast<std::uint8_t>((rgb >> 16) & 0xff),
    static_cast<std::uint8_t>((rgb >>  8) & 0xff),
    static_cast<std::uint8_t>((rgb      ) & 0xff),
    a
  );
}

Color4f& Color4f::set_bytes(std::uint8_t rgb) { return set_bytes(rgb,rgb,rgb); }

Color4f& Color4f::set_bytes(std::uint8_t rgb,std::uint8_t a) { return set_bytes(rgb,rgb,rgb,a); }

Color4f& Color4f::set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b) {
  return set(
    static_cast<float>(r) / 255.0f,
    static_cast<float>(g) / 255.0f,
    static_cast<float>(b) / 255.0f
  );
}

Color4f& Color4f::set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a) {
  return set(
    static_cast<float>(r) / 255.0f,
    static_cast<float>(g) / 255.0f,
    static_cast<float>(b) / 255.0f,
    static_cast<float>(a) / 255.0f
  );
}

Color4f Color4f::with_r(float r) const { return Color4f{r,g,b,a}; }

Color4f Color4f::with_g(float g) const { return Color4f{r,g,b,a}; }

Color4f Color4f::with_b(float b) const { return Color4f{r,g,b,a}; }

Color4f Color4f::with_a(float a) const { return Color4f{r,g,b,a}; }

Color4f Color4f::with_byte_r(std::uint8_t r) const { return Color4f{static_cast<float>(r) / 255.0f,g,b,a}; }

Color4f Color4f::with_byte_g(std::uint8_t g) const { return Color4f{r,static_cast<float>(g) / 255.0f,b,a}; }

Color4f Color4f::with_byte_b(std::uint8_t b) const { return Color4f{r,g,static_cast<float>(b) / 255.0f,a}; }

Color4f Color4f::with_byte_a(std::uint8_t a) const { return Color4f{r,g,b,static_cast<float>(a) / 255.0f}; }

} // namespace cybel
