/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "color.h"

#include <charconv>

namespace cybel {

const Color4f Color4f::kNone{0.0f,0.0f};
const Color4f Color4f::kBlack = hex(0x000000);
const Color4f Color4f::kBlue = hex(0x0000ff);
const Color4f Color4f::kBrown = hex(0x964b00);
const Color4f Color4f::kCopper = hex(0xb87333);
const Color4f Color4f::kCyan = hex(0x00ffff);
const Color4f Color4f::kGreen = hex(0x00ff00);
const Color4f Color4f::kHotPink = hex(0xff69b4);
const Color4f Color4f::kPink = hex(0xffc0cb);
const Color4f Color4f::kPurple = hex(0xff00ff);
const Color4f Color4f::kRed = hex(0xff0000);
const Color4f Color4f::kWhite = hex(0xffffff);
const Color4f Color4f::kYellow = hex(0xffff00);

Color4f Color4f::hex(std::uint32_t rgb,std::uint8_t a) noexcept { return Color4f{}.set_hex(rgb,a); }

Color4f Color4f::hex(std::string_view str,const Color4f& fallback) {
  if(str.length() >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
    str.remove_prefix(2);
  }

  if(str.empty()) { return fallback; }

  // RGBA: `11223344`.
  if(str.length() >= 8) {
    std::uint64_t rgba = 0;
    const auto err = std::from_chars(str.data(),str.data() + str.size(),rgba,16).ec;

    if(err != std::errc{}) { return fallback; }

    return hex(static_cast<std::uint32_t>((rgba >> 8) & 0xffffff),static_cast<std::uint8_t>(rgba & 0xff));
  }

  // RGB: `112233`.
  std::uint32_t rgb = 0;
  const auto err = std::from_chars(str.data(),str.data() + str.size(),rgb,16).ec;

  if(err != std::errc{}) { return fallback; }

  return hex(rgb & 0xffffff);
}

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

std::uint8_t Color4f::byte_r() const {
  return static_cast<std::uint8_t>(std::clamp(std::round(r * 255.0f),0.0f,255.0f));
}

std::uint8_t Color4f::byte_g() const {
  return static_cast<std::uint8_t>(std::clamp(std::round(g * 255.0f),0.0f,255.0f));
}

std::uint8_t Color4f::byte_b() const {
  return static_cast<std::uint8_t>(std::clamp(std::round(b * 255.0f),0.0f,255.0f));
}

std::uint8_t Color4f::byte_a() const {
  return static_cast<std::uint8_t>(std::clamp(std::round(a * 255.0f),0.0f,255.0f));
}

} // namespace cybel
