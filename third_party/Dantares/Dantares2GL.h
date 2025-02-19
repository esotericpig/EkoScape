/*
 Copyright (C) 2006 Ryan Witmer
 Copyright (C) 2024-2025 Bradley Whited

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation; either version 2 of
 the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef DANTARES2_GL_H
#define DANTARES2_GL_H

#include "Dantares2.h"

#if defined(DANTARES_RENDERER_GL)

#if defined(DANTARES_PLATFORM_MACOS)
    #ifndef GL_SILENCE_DEPRECATION
    #define GL_SILENCE_DEPRECATION
    #endif

    //Mac OS X OpenGL headers.
    #include<OpenGL/gl.h>
#elif defined(DANTARES_PLATFORM_WINDOWS)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif

    //Windows OpenGL headers.
    #include<windows.h>
    #include<GL/gl.h>
#else
    //X11 OpenGL headers.
    #include<GL/gl.h>
#endif

class Dantares2GLRenderer : public Dantares2::RendererClass
{
public:
    void BeginDraw() override;
    void EndDraw() override;

    void TranslateModelMatrix(float X, float Y, float Z) override;
    void RotateModelMatrix(float Angle, float X, float Y, float Z) override;
    void UpdateModelMatrix() override;
    void PushModelMatrix() override;
    void PopModelMatrix() override;

    GLuint GenerateQuadLists(int Count) override;
    void DeleteQuadLists(GLuint ID, int Count) override;
    void CompileQuadList(GLuint ID, int Index, const QuadListData &Data) override;
    void DrawQuadList(GLuint ID, int Index) override;
};

#endif //DANTARES_RENDERER_GL.
#endif
