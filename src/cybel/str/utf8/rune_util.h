/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_STR_UTF8_RUNE_UTIL_H_
#define CYBEL_STR_UTF8_RUNE_UTIL_H_

namespace cybel::utf8 {

/**
 * UTF-8 rune links:
 * - https://www.unicode.org/Public/16.0.0/ucd/PropList.txt
 */
namespace RuneUtil {
  inline const char32_t kInvalidRune = U'�'; // \uFFFD.

  inline const unsigned char kMaxAsciiOctet = 0x7F;
  inline const unsigned char kMinOctetTail = 0x80; // 0b1000'0000.
  inline const unsigned char kMaxOctetTail = 0xBF; // 0b1011'1111.
  inline const unsigned char kOctetBodyMask = 0b0011'1111; // 0b10xxxxxx.

  bool is_whitespace(char32_t rune);
}

} // Namespace.
#endif
