/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_TYPES_COLOR_H_
#define CYBEL_TYPES_COLOR_H_

#include "cybel/common.h"

namespace cybel {

/**
 * Clamped floats to [0,1].
 */
class Color4f {
public:
  static const Color4f kNone;
  static const Color4f kBlack;
  static const Color4f kBlue;
  static const Color4f kBrown;
  static const Color4f kCopper;
  static const Color4f kCyan;
  static const Color4f kGreen;
  static const Color4f kHotPink;
  static const Color4f kPink;
  static const Color4f kPurple;
  static const Color4f kRed;
  static const Color4f kWhite;
  static const Color4f kYellow;

  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  float a = 1.0f;

  static Color4f hex(std::uint32_t rgb,std::uint8_t a = 255) noexcept;
  static Color4f hex(std::string_view str,const Color4f& fallback = kWhite);
  static Color4f bytes(std::uint8_t rgb,std::uint8_t a = 255) noexcept;
  static Color4f bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a = 255) noexcept;

  explicit Color4f() noexcept = default;
  explicit Color4f(float rgb,float a = 1.0f) noexcept;
  explicit Color4f(float r,float g,float b,float a = 1.0f) noexcept;

  auto operator<=>(const Color4f& other) const = default;

  Color4f& set(float rgb);
  Color4f& set(float rgb,float a);
  Color4f& set(float r,float g,float b);
  Color4f& set(float r,float g,float b,float a);
  Color4f& set_hex(std::uint32_t rgb);
  Color4f& set_hex(std::uint32_t rgb,std::uint8_t a);
  Color4f& set_bytes(std::uint8_t rgb);
  Color4f& set_bytes(std::uint8_t rgb,std::uint8_t a);
  Color4f& set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b);
  Color4f& set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a);

  Color4f with_r(float r) const;
  Color4f with_g(float g) const;
  Color4f with_b(float b) const;
  Color4f with_a(float a) const;
  Color4f with_byte_r(std::uint8_t r) const;
  Color4f with_byte_g(std::uint8_t g) const;
  Color4f with_byte_b(std::uint8_t b) const;
  Color4f with_byte_a(std::uint8_t a) const;

  std::uint8_t byte_r() const;
  std::uint8_t byte_g() const;
  std::uint8_t byte_b() const;
  std::uint8_t byte_a() const;
};

} // namespace cybel
#endif
