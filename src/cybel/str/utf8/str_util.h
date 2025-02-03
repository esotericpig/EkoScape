/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_STR_UTF8_STR_UTIL_H_
#define CYBEL_STR_UTF8_STR_UTIL_H_

#include "cybel/common.h"

namespace cybel::utf8 {

namespace StrUtil {
  int casecmp_ascii(std::string_view str1,std::string_view str2);
  std::size_t count_runes(std::string_view str);
  std::string ellipsize(std::string_view str,std::size_t max_len);
  std::string ljust(std::string_view str,std::size_t new_len);
  std::string strip(std::string_view str);
}

} // Namespace.
#endif
