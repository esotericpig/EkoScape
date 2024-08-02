/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ekoscape_error.h"

namespace ekoscape {

EkoScapeError::EkoScapeError() noexcept {}

EkoScapeError::EkoScapeError(const std::string& msg) noexcept
    : msg_{msg} {}

const char* EkoScapeError::what() const noexcept { return msg_.c_str(); }

} // Namespace.
