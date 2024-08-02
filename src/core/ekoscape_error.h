/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_EKOSCAPE_ERROR_H_
#define EKOSCAPE_CORE_EKOSCAPE_ERROR_H_

#include "common.h"

namespace ekoscape {

class EkoScapeError : public std::exception {
public:
  EkoScapeError() noexcept;
  explicit EkoScapeError(const std::string& msg) noexcept;

  const char* what() const noexcept override;

private:
    std::string msg_{};
};

} // Namespace.
#endif
