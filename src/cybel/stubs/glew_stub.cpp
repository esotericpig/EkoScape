/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "glew_stub.h"

#if !defined(GLEW_VERSION)

GLenum glewInit() { return GLEW_OK; }

const GLubyte* glewGetErrorString(GLenum /*error*/) { return reinterpret_cast<const GLubyte*>(""); }

#endif // GLEW_VERSION
