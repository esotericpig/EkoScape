/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cybel_error.h"

namespace cybel {

CybelError::CybelError(const std::string& msg) noexcept
    : msg_{msg} {}

const char* CybelError::what() const noexcept { return msg_.c_str(); }

} // Namespace.
