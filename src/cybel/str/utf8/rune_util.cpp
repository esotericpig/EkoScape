/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rune_util.h"

namespace cybel::utf8 {

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

} // Namespace.
