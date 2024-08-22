/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UTIL_CYBEL_ERROR_H_
#define CYBEL_UTIL_CYBEL_ERROR_H_

#include "cybel/common.h"

namespace cybel {

class CybelError : public std::exception {
public:
  CybelError() noexcept;
  explicit CybelError(const std::string& msg) noexcept;

  const char* what() const noexcept override;

private:
    std::string msg_{};
};

} // Namespace.
#endif
