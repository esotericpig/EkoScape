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

#include "Dantares2GL.h"

#if defined(DANTARES_RENDERER_GL)

void Dantares2GLRenderer::BeginDraw()
{
}

void Dantares2GLRenderer::EndDraw()
{
}

void Dantares2GLRenderer::TranslateModelMatrix(float X, float Y, float Z)
{
    glTranslatef(X, Y, Z);
}

void Dantares2GLRenderer::RotateModelMatrix(float Angle, float X, float Y, float Z)
{
    glRotatef(Angle, X, Y, Z);
}

void Dantares2GLRenderer::UpdateModelMatrix()
{
}

void Dantares2GLRenderer::PushModelMatrix()
{
    glPushMatrix();
}

void Dantares2GLRenderer::PopModelMatrix()
{
    glPopMatrix();
}

GLuint Dantares2GLRenderer::GenerateQuadLists(int Count)
{
    return glGenLists(Count);
}

void Dantares2GLRenderer::DeleteQuadLists(GLuint ID, int Count)
{
    glDeleteLists(ID, Count);
}

void Dantares2GLRenderer::CompileQuadList(GLuint ID, int Index,
                                          GLuint TextureID,
                                          const QuadNormalData &N,
                                          const QuadVertexData &V)
{
    glNewList(ID + Index, GL_COMPILE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextureID);

        glBegin(GL_QUADS);
            glNormal3f(N.X, N.Y, N.Z);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(V.X1, V.Y1, V.Z1);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(V.X2, V.Y2, V.Z2);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(V.X3, V.Y3, V.Z3);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(V.X4, V.Y4, V.Z4);
        glEnd();
    glEndList();
}

void Dantares2GLRenderer::DrawQuadList(GLuint ID, int Index)
{
    glCallList(ID + Index);
}

#endif //DANTARES_RENDERER_GL.
