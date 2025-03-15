/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rune_util.h"

namespace cybel::utf8 {

char32_t RuneUtil::next_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count) {
  // Always set to 1 to avoid infinite loops.
  byte_count = 1;

  if(index >= str.size()) { return kInvalidRune; }

  const auto octet1 = static_cast<Octet>(str[index]);

  // UTF8-1 = 0x00-7F.
  if(octet1 <= kMaxAsciiOctet) { return octet1; }

  if((++index) >= str.size()) { return kInvalidRune; }

  return _unpack_seq(str,index,byte_count,octet1,_count_seq(octet1));
}

char32_t RuneUtil::prev_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count) {
  // Always set to 1 to avoid infinite loops.
  byte_count = 1;

  if(index >= str.size()) { return kInvalidRune; }

  auto octet = static_cast<Octet>(str[index]);

  // UTF8-1 = 0x00-7F.
  if(octet <= kMaxAsciiOctet) { return octet; }

  for(std::uint8_t octet_count = 2; index > 0 && octet_count <= 4; --index,++octet_count) {
    octet = static_cast<Octet>(str[index - 1]);
    const auto type = octet & 0b1100'0000;

    // Head/Lead octet?
    if(type == 0b1100'0000) {
      // Double check sequence count.
      if(_count_seq(octet) != octet_count) { break; }

      return _unpack_seq(str,index,byte_count,octet,octet_count);
    }

    // Not a continuation octet? (0b1000'0000)
    if(type != kMinOctetTail) { break; }
  }

  return kInvalidRune;
}

std::uint8_t RuneUtil::_count_seq(Octet octet1) {
  // UTF8-2 = 0b110xxxxx.
  if((octet1 & 0b1110'0000) == 0b1100'0000) { return 2; }

  // UTF8-3 = 0b1110xxxx.
  if((octet1 & 0b1111'0000) == 0b1110'0000) { return 3; }

  // UTF8-4 = 0b11110xxx.
  if((octet1 & 0b1111'1000) == 0b1111'0000) { return 4; }

  return 0;
}

char32_t RuneUtil::_unpack_seq(std::string_view str,std::size_t index,std::uint8_t& byte_count,Octet octet1,
                               std::uint8_t octet_count) {
  // The _unpack_seq*() funcs update `byte_count` instead of us updating it here to `octet_count` if the
  //     resulting rune is valid, because a trickster could use the literal `kInvalidRune` in the text,
  //     in which case the `byte_count` should be 3, not 1.
  switch(octet_count) {
    case 2: return _unpack_seq2(str,index,byte_count,octet1);
    case 3: return _unpack_seq3(str,index,byte_count,octet1);
    case 4: return _unpack_seq4(str,index,byte_count,octet1);
  }

  return kInvalidRune;
}

char32_t RuneUtil::_unpack_seq2(std::string_view str,std::size_t index,std::uint8_t& byte_count,
                                Octet octet1) {
  if(index >= str.size()) { return kInvalidRune; }

  // UTF8-2 = 0xC2-DF UTF8-tail.
  if(octet1 < 0xC2 || octet1 > 0xDF) { return kInvalidRune; }

  const auto octet2 = static_cast<Octet>(str[index]);

  if(octet2 < kMinOctetTail || octet2 > kMaxOctetTail) {
    return kInvalidRune;
  }

  // UTF8-2 = 0b110xxxxx.
  auto rune = static_cast<char32_t>(octet1 & 0b0001'1111);

  rune = (rune << 6) | (octet2 & kOctetTailMask);
  byte_count = 2;

  return rune;
}

char32_t RuneUtil::_unpack_seq3(std::string_view str,std::size_t index,std::uint8_t& byte_count,
                                Octet octet1) {
  if((index + 1) >= str.size()) { return kInvalidRune; }

  const auto octet2 = static_cast<Octet>(str[index]);
  const auto octet3 = static_cast<Octet>(str[++index]);

  // UTF8-3 = 0xE0    0xA0-BF   UTF8-tail.
  // UTF8-3 = 0xE1-EC UTF8-tail UTF8-tail.
  // UTF8-3 = 0xED    0x80-9F   UTF8-tail.
  // UTF8-3 = 0xEE-EF UTF8-tail UTF8-tail.
  if(octet1 == 0xE0) {
    if(octet2 < 0xA0 || octet2 > 0xBF) { return kInvalidRune; }
  } else if((octet1 >= 0xE1 && octet1 <= 0xEC) ||
            (octet1 >= 0xEE && octet1 <= 0xEF)) {
    if(octet2 < kMinOctetTail || octet2 > kMaxOctetTail) { return kInvalidRune; }
  } else if(octet1 == 0xED) {
    if(octet2 < 0x80 || octet2 > 0x9F) { return kInvalidRune; }
  } else {
    return kInvalidRune;
  }

  if(octet3 < kMinOctetTail || octet3 > kMaxOctetTail) {
    return kInvalidRune;
  }

  // UTF8-3 = 0b1110xxxx.
  auto rune = static_cast<char32_t>(octet1 & 0b0000'1111);

  rune = (rune << 6) | (octet2 & kOctetTailMask);
  rune = (rune << 6) | (octet3 & kOctetTailMask);
  byte_count = 3;

  return rune;
}

char32_t RuneUtil::_unpack_seq4(std::string_view str,std::size_t index,std::uint8_t& byte_count,
                                Octet octet1) {
  if((index + 2) >= str.size()) { return kInvalidRune; }

  const auto octet2 = static_cast<Octet>(str[index]);
  const auto octet3 = static_cast<Octet>(str[++index]);
  const auto octet4 = static_cast<Octet>(str[++index]);

  // UTF8-4 = 0xF0    0x90-BF   UTF8-tail UTF8-tail.
  // UTF8-4 = 0xF1-F3 UTF8-tail UTF8-tail UTF8-tail.
  // UTF8-4 = 0xF4    0x80-8F   UTF8-tail UTF8-tail.
  if(octet1 == 0xF0) {
    if(octet2 < 0x90 || octet2 > 0xBF) { return kInvalidRune; }
  } else if(octet1 >= 0xF1 && octet1 <= 0xF3) {
    if(octet2 < kMinOctetTail || octet2 > kMaxOctetTail) { return kInvalidRune; }
  } else if(octet1 == 0xF4) {
    if(octet2 < 0x80 || octet2 > 0x8F) { return kInvalidRune; }
  } else {
    return kInvalidRune;
  }

  if((octet3 < kMinOctetTail || octet3 > kMaxOctetTail) ||
     (octet4 < kMinOctetTail || octet4 > kMaxOctetTail)) {
    return kInvalidRune;
  }

  // UTF8-4 = 0b11110xxx.
  auto rune = static_cast<char32_t>(octet1 & 0b0000'0111);

  rune = (rune << 6) | (octet2 & kOctetTailMask);
  rune = (rune << 6) | (octet3 & kOctetTailMask);
  rune = (rune << 6) | (octet4 & kOctetTailMask);
  byte_count = 4;

  return rune;
}

std::string RuneUtil::pack(char32_t rune) {
  std::string result{};

  // UTF8-1.
  if(rune <= kMaxAsciiOctet) {
    result += static_cast<char>(rune);
  }
  // UTF8-2.
  else if(rune >= 0x0080 && rune <= 0x07FF) {
    // 0b110xxxxx.
    result += static_cast<char>(0b1100'0000   | ((rune >> 6) & 0b0001'1111));
    result += static_cast<char>(kMinOctetTail | ( rune       & kOctetTailMask));
  }
  // UTF8-3.
  else if((rune >= 0x0800 && rune <= 0xD7FF) ||
          (rune >= 0xE000 && rune <= 0xFFFF)) {
    // 0b1110xxxx.
    result += static_cast<char>(0b1110'0000   | ((rune >> 12) & 0b0000'1111));
    result += static_cast<char>(kMinOctetTail | ((rune >>  6) & kOctetTailMask));
    result += static_cast<char>(kMinOctetTail | ( rune        & kOctetTailMask));
  }
  // UTF8-4.
  else if(rune >= 0x01'0000 && rune <= 0x10'FFFF) {
    // 0b11110xxx.
    result += static_cast<char>(0b1111'0000   | ((rune >> 18) & 0b0000'0111));
    result += static_cast<char>(kMinOctetTail | ((rune >> 12) & kOctetTailMask));
    result += static_cast<char>(kMinOctetTail | ((rune >>  6) & kOctetTailMask));
    result += static_cast<char>(kMinOctetTail | ( rune        & kOctetTailMask));
  } else {
    return kInvalidPackedRune;
  }

  return result;
}

// See: https://www.unicode.org/Public/16.0.0/ucd/PropList.txt
bool RuneUtil::is_whitespace(char32_t rune) {
  switch(rune) {
    case 0x0020: // SPACE
    case 0x0085: // <control-0085>
    case 0x00A0: // NO-BREAK SPACE
    case 0x1680: // OGHAM SPACE MARK
    case 0x2028: // LINE SEPARATOR
    case 0x2029: // PARAGRAPH SEPARATOR
    case 0x202F: // NARROW NO-BREAK SPACE
    case 0x205F: // MEDIUM MATHEMATICAL SPACE
    case 0x3000: // IDEOGRAPHIC SPACE
      return true;
  }

  return (rune >= 0x0009 && rune <= 0x000D) || // <control-0009>..<control-000D>
         (rune >= 0x2000 && rune <= 0x200A);   // EN QUAD..HAIR SPACE
}

} // namespace cybel::utf8
