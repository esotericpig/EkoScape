/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_STR_UTF8_RUNE_UTIL_H_
#define CYBEL_STR_UTF8_RUNE_UTIL_H_

#include "cybel/common.h"

namespace cybel::utf8 {

using Octet = unsigned char;

/**
 * UTF-8 links:
 * - https://datatracker.ietf.org/doc/html/rfc3629#autoid-3
 * - https://datatracker.ietf.org/doc/html/rfc3629#autoid-4
 * - https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G31703
 * - https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G48534
 * - https://www.unicode.org/Public/16.0.0/ucd/PropList.txt
 */
namespace RuneUtil {
  inline const char32_t kInvalidRune = U'�'; // \uFFFD.
  inline const std::string kInvalidPackedRune = "�"; // \xEF\xBF\xBD.

  inline const Octet kMaxAsciiOctet = 0x7F; // 0b0111'1111.
  inline const Octet kMinOctetTail = 0x80; // 0b1000'0000.
  inline const Octet kMaxOctetTail = 0xBF; // 0b1011'1111.
  inline const Octet kOctetTailMask = 0b0011'1111; // 0b10xxxxxx.

  char32_t next_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count);
  char32_t prev_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count);

  std::string pack(char32_t rune);

  bool is_whitespace(char32_t rune);

  std::uint8_t _count_seq(Octet octet1);
  char32_t _unpack_seq(std::string_view str,std::size_t index,std::uint8_t& byte_count,Octet octet1,
                       std::uint8_t octet_count);
  char32_t _unpack_seq2(std::string_view str,std::size_t index,std::uint8_t& byte_count,Octet octet1);
  char32_t _unpack_seq3(std::string_view str,std::size_t index,std::uint8_t& byte_count,Octet octet1);
  char32_t _unpack_seq4(std::string_view str,std::size_t index,std::uint8_t& byte_count,Octet octet1);
}

} // namespace cybel::utf8
#endif
