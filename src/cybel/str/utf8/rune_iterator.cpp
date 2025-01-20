/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rune_iterator.h"

#include <bit>

namespace cybel::utf8 {

char32_t RuneIterator::next_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count) {
  if(index >= str.size()) {
    byte_count = 0;
    return RuneUtil::kInvalidRune;
  }

  byte_count = 1;
  const auto octet1 = static_cast<unsigned char>(str[index]);

  // UTF8-1 = 0x00-7F.
  if(octet1 <= RuneUtil::kMaxAsciiOctet) { return octet1; }

  if((++index) >= str.size()) { return RuneUtil::kInvalidRune; }

  const int octet_count = std::countl_one(octet1);

  // The unpack_seq*() funcs update `byte_count` instead of us updating it here to `octet_count` if the
  //     resulting rune is valid, because a trickster could use the literal `kInvalidRune` in the text,
  //     in which case the `byte_count` should be 3, not 1.
  // This is the same in prev_rune().
  switch(octet_count) {
    case 2: return unpack_seq2(str,index,byte_count,octet1);
    case 3: return unpack_seq3(str,index,byte_count,octet1);
    case 4: return unpack_seq4(str,index,byte_count,octet1);
  }

  return RuneUtil::kInvalidRune;
}

char32_t RuneIterator::prev_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count) {
  if(index >= str.size()) {
    byte_count = 0;
    return RuneUtil::kInvalidRune;
  }

  byte_count = 1;
  auto octet = static_cast<unsigned char>(str[index]);

  // UTF8-1 = 0x00-7F.
  if(octet <= RuneUtil::kMaxAsciiOctet) { return octet; }

  for(int octet_count = 2; index > 0 && octet_count <= 4; --index,++octet_count) {
    octet = static_cast<unsigned char>(str[index - 1]);
    const auto head_bits2 = octet & 0b1100'0000;

    // Head/Lead octet?
    if(head_bits2 == 0b1100'0000) {
      // Double check sequence count.
      if(std::countl_one(octet) != octet_count) { break; }

      switch(octet_count) {
        case 2: return unpack_seq2(str,index,byte_count,octet);
        case 3: return unpack_seq3(str,index,byte_count,octet);
        case 4: return unpack_seq4(str,index,byte_count,octet);
      }

      break;
    }

    // Not a body/continuation octet?
    if(head_bits2 != 0b1000'0000) { break; }
  }

  return RuneUtil::kInvalidRune;
}

char32_t RuneIterator::unpack_seq2(std::string_view str,std::size_t index,std::uint8_t& byte_count,
                                   unsigned char octet1) {
  if(index >= str.size()) { return RuneUtil::kInvalidRune; }

  // UTF8-2 = 0xC2-DF UTF8-tail.
  if(octet1 < 0xC2 || octet1 > 0xDF) { return RuneUtil::kInvalidRune; }

  const auto octet2 = static_cast<unsigned char>(str[index]);

  if(octet2 < RuneUtil::kMinOctetTail || octet2 > RuneUtil::kMaxOctetTail) {
    return RuneUtil::kInvalidRune;
  }

  // UTF8-2 = 0b110xxxxx.
  auto rune = static_cast<char32_t>(octet1 & 0b0001'1111);

  rune = (rune << 6) | (octet2 & RuneUtil::kOctetBodyMask);
  byte_count = 2;

  return rune;
}

char32_t RuneIterator::unpack_seq3(std::string_view str,std::size_t index,std::uint8_t& byte_count,
                                   unsigned char octet1) {
  if((index + 1) >= str.size()) { return RuneUtil::kInvalidRune; }

  const auto octet2 = static_cast<unsigned char>(str[index]);
  const auto octet3 = static_cast<unsigned char>(str[++index]);

  // UTF8-3 = 0xE0    0xA0-BF   UTF8-tail.
  // UTF8-3 = 0xE1-EC UTF8-tail UTF8-tail.
  // UTF8-3 = 0xED    0x80-9F   UTF8-tail.
  // UTF8-3 = 0xEE-EF UTF8-tail UTF8-tail.
  if(octet1 == 0xE0) {
    if(octet2 < 0xA0 || octet2 > RuneUtil::kMaxOctetTail) {
      return RuneUtil::kInvalidRune;
    }
  } else if((octet1 >= 0xE1 && octet1 <= 0xEC) || (octet1 >= 0xEE && octet1 <= 0xEF)) {
    if(octet2 < RuneUtil::kMinOctetTail || octet2 > RuneUtil::kMaxOctetTail) {
      return RuneUtil::kInvalidRune;
    }
  } else if(octet1 == 0xED) {
    if(octet2 < RuneUtil::kMinOctetTail || octet2 > 0x9F) {
      return RuneUtil::kInvalidRune;
    }
  } else {
    return RuneUtil::kInvalidRune;
  }

  if(octet3 < RuneUtil::kMinOctetTail || octet3 > RuneUtil::kMaxOctetTail) {
    return RuneUtil::kInvalidRune;
  }

  // UTF8-3 = 0b1110xxxx.
  auto rune = static_cast<char32_t>(octet1 & 0b0000'1111);

  rune = (rune << 6) | (octet2 & RuneUtil::kOctetBodyMask);
  rune = (rune << 6) | (octet3 & RuneUtil::kOctetBodyMask);
  byte_count = 3;

  return rune;
}

char32_t RuneIterator::unpack_seq4(std::string_view str,std::size_t index,std::uint8_t& byte_count,
                                   unsigned char octet1) {
  if((index + 2) >= str.size()) { return RuneUtil::kInvalidRune; }

  const auto octet2 = static_cast<unsigned char>(str[index]);
  const auto octet3 = static_cast<unsigned char>(str[++index]);
  const auto octet4 = static_cast<unsigned char>(str[++index]);

  // UTF8-4 = 0xF0    0x90-BF   UTF8-tail UTF8-tail.
  // UTF8-4 = 0xF1-F3 UTF8-tail UTF8-tail UTF8-tail.
  // UTF8-4 = 0xF4    0x80-8F   UTF8-tail UTF8-tail.
  if(octet1 == 0xF0) {
    if(octet2 < 0x90 || octet2 > RuneUtil::kMaxOctetTail) {
      return RuneUtil::kInvalidRune;
    }
  } else if(octet1 >= 0xF1 && octet1 <= 0xF3) {
    if(octet2 < RuneUtil::kMinOctetTail || octet2 > RuneUtil::kMaxOctetTail) {
      return RuneUtil::kInvalidRune;
    }
  } else if(octet1 == 0xF4) {
    if(octet2 < RuneUtil::kMinOctetTail || octet2 > 0x8F) {
      return RuneUtil::kInvalidRune;
    }
  } else {
    return RuneUtil::kInvalidRune;
  }

  if((octet3 < RuneUtil::kMinOctetTail || octet3 > RuneUtil::kMaxOctetTail) ||
     (octet4 < RuneUtil::kMinOctetTail || octet4 > RuneUtil::kMaxOctetTail)) {
    return RuneUtil::kInvalidRune;
  }

  // UTF8-4 = 0b11110xxx.
  auto rune = static_cast<char32_t>(octet1 & 0b0000'0111);

  rune = (rune << 6) | (octet2 & RuneUtil::kOctetBodyMask);
  rune = (rune << 6) | (octet3 & RuneUtil::kOctetBodyMask);
  rune = (rune << 6) | (octet4 & RuneUtil::kOctetBodyMask);
  byte_count = 4;

  return rune;
}

RuneIterator RuneIterator::begin(std::string_view str,std::size_t index) {
  return RuneIterator{str,(index <= str.size()) ? index : str.size()};
}

RuneIterator RuneIterator::end(std::string_view str,std::size_t relative_count) {
  return RuneIterator{str,(relative_count <= str.size()) ? (str.size() - relative_count) : 0};
}

RuneIterator::RuneIterator(std::string_view str,std::size_t index)
    : str_(str),index_(index) {
  next_rune();
}

bool RuneIterator::operator!=(const RuneIterator& other) const {
  return str_ != other.str_ || index_ != other.index_;
}

bool RuneIterator::operator==(const RuneIterator& other) const {
  return !(*this != other);
}

std::strong_ordering RuneIterator::operator<=>(const RuneIterator& other) const {
  return (str_ == other.str_) ? (index_ <=> other.index_) : (index_ <=> str_.size());
}

char32_t RuneIterator::operator*() const { return rune_; }

RuneIterator& RuneIterator::operator++() {
  next_rune();

  return *this;
}

RuneIterator RuneIterator::operator++(int) {
  RuneIterator temp = *this;

  next_rune();

  return temp;
}

RuneIterator& RuneIterator::operator--() {
  prev_rune();

  return *this;
}

RuneIterator RuneIterator::operator--(int) {
  RuneIterator temp = *this;

  prev_rune();

  return temp;
}

void RuneIterator::next_rune() {
  index_ = std::min(index_ + byte_count_,str_.size());
  rune_ = next_rune(str_,index_,byte_count_);

  // Shouldn't happen, but Justin Case.
  if(byte_count_ == 0) { index_ = str_.size(); }
}

void RuneIterator::prev_rune() {
  if(index_ >= str_.size() && !str_.empty()) {
    index_ = str_.size() - 1;
  }

  rune_ = prev_rune(str_,index_,byte_count_);

  // Shouldn't happen, but Justin Case.
  if(byte_count_ == 0) {
    index_ = 0;
  } else if(byte_count_ <= index_) {
    index_ -= byte_count_;
  }
}

std::string_view RuneIterator::str() const { return str_; }

std::size_t RuneIterator::index() const { return index_; }

std::uint8_t RuneIterator::byte_count() const { return byte_count_; }

char32_t RuneIterator::rune() const { return rune_; }

} // Namespace.
