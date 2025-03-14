/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rune_util.h"

namespace cybel::utf8 {

std::string RuneUtil::pack(char32_t rune) {
  std::string result{};

  // UTF8-1.
  if(rune <= kMaxAsciiOctet) {
    result += static_cast<char>(rune);
  }
  // UTF8-2.
  else if(rune >= 0x0080 && rune <= 0x07FF) {
    const auto octet1 = static_cast<char>(0b1100'0000 | ((rune >> 6) & 0b0001'1111));
    const auto octet2 = static_cast<char>(kOctetBodyType | (rune & kOctetBodyMask));

    result += octet1;
    result += octet2;
  }
  // UTF8-3.
  else if((rune >= 0x0800 && rune <= 0xD7FF) ||
          (rune >= 0xE000 && rune <= 0xFFFF)) {
    const auto octet1 = static_cast<char>(0b1110'0000 | ((rune >> 12) & 0b0000'1111));
    const auto octet2 = static_cast<char>(kOctetBodyType | ((rune >> 6) & kOctetBodyMask));
    const auto octet3 = static_cast<char>(kOctetBodyType | (rune & kOctetBodyMask));

    result += octet1;
    result += octet2;
    result += octet3;
  }
  // UTF8-4.
  else if(rune >= 0x01'0000 && rune <= 0x10'FFFF) {
    const auto octet1 = static_cast<char>(0b1111'0000 | ((rune >> 18) & 0b0000'0111));
    const auto octet2 = static_cast<char>(kOctetBodyType | ((rune >> 12) & kOctetBodyMask));
    const auto octet3 = static_cast<char>(kOctetBodyType | ((rune >> 6) & kOctetBodyMask));
    const auto octet4 = static_cast<char>(kOctetBodyType | (rune & kOctetBodyMask));

    result += octet1;
    result += octet2;
    result += octet3;
    result += octet4;
  } else {
    return kInvalidPackedRune;
  }

  return result;
}

bool RuneUtil::is_whitespace(char32_t rune) {
  switch(rune) {
    case 0x0020: // SPACE.
    case 0x0085: // <control-0085>.
    case 0x00A0: // NO-BREAK SPACE.
    case 0x1680: // OGHAM SPACE MARK.
    case 0x2028: // LINE SEPARATOR.
    case 0x2029: // PARAGRAPH SEPARATOR.
    case 0x202F: // NARROW NO-BREAK SPACE.
    case 0x205F: // MEDIUM MATHEMATICAL SPACE.
    case 0x3000: // IDEOGRAPHIC SPACE.
      return true;
  }

  return (rune >= 0x0009 && rune <= 0x000D) || // <control-0009>..<control-000D>.
         (rune >= 0x2000 && rune <= 0x200A);   // EN QUAD..HAIR SPACE.
}

} // namespace cybel::utf8
