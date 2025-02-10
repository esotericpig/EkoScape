/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_STUBS_GLEW_STUB_H_
#define CYBEL_STUBS_GLEW_STUB_H_

#include "cybel/common.h"

#if !defined(GLEW_VERSION)

inline constexpr bool GLEW_VERSION_2_1 = true;
inline constexpr bool GLEW_VERSION_3_0 = true;

inline constexpr GLenum GLEW_OK = 0;

GLenum glewInit();
const GLubyte* glewGetErrorString(GLenum error);

#endif // GLEW_VERSION.
#endif
