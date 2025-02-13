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

#if defined(DANTARES_RENDER_GL)

#include<memory>
#include<utility>

Dantares2GL::Dantares2GL(float SquareSize, float FloorHeight, float CeilingHeight)
    : Dantares2(SquareSize, FloorHeight, CeilingHeight)
{
}

std::unique_ptr<Dantares2GL::SpaceClass> Dantares2GL::BuildSpace(int SpaceID)
{
    return std::make_unique<SpaceClassGL>(SpaceID);
}

void Dantares2GL::BeginDraw()
{
}

void Dantares2GL::EndDraw()
{
}

void Dantares2GL::TranslateModelMatrix(float X, float Y, float Z)
{
    glTranslatef(X, Y, Z);
}

void Dantares2GL::RotateModelMatrix(float Angle, float X, float Y, float Z)
{
    glRotatef(Angle, X, Y, Z);
}

void Dantares2GL::UpdateModelMatrix()
{
}

void Dantares2GL::PushModelMatrix()
{
    glPushMatrix();
}

void Dantares2GL::PopModelMatrix()
{
    glPopMatrix();
}

Dantares2GL::SpaceClassGL::SpaceClassGL(int Type) noexcept
    : SpaceClass(Type)
{
}

Dantares2GL::SpaceClassGL::SpaceClassGL(SpaceClassGL &&Other) noexcept
    : SpaceClass(std::move(Other))
{
    MoveFrom(std::move(Other));
}

Dantares2GL::SpaceClassGL &Dantares2GL::SpaceClassGL::operator = (SpaceClassGL &&Other) noexcept
{
    if (this != &Other)
    {
        SpaceClass::operator = (std::move(Other));
        MoveFrom(std::move(Other));
    }

    return *this;
}

void Dantares2GL::SpaceClassGL::MoveFrom(SpaceClassGL &&Other) noexcept
{
    DisplayList = std::exchange(Other.DisplayList, 0);
}

Dantares2GL::SpaceClassGL::~SpaceClassGL() noexcept
{
    DeleteDisplayList();
}

void Dantares2GL::SpaceClassGL::DeleteDisplayList() noexcept
{
    if (DisplayList != 0)
    {
        glDeleteLists(DisplayList, FACE_COUNT);
        DisplayList = 0;
    }
}

void Dantares2GL::SpaceClassGL::GenerateFaces(float SquareOffset, float FloorHeight, float CeilingHeight)
{
    DeleteDisplayList();
    DisplayList = glGenLists(FACE_COUNT);

    if (CeilingTexture != 0)
    {
        glNewList(DisplayList + FACE_CEILING, GL_COMPILE);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, CeilingTexture);

            glBegin(GL_QUADS);
                glNormal3f(0.0f, -1.0f, 0.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-SquareOffset, CeilingHeight, -SquareOffset);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(SquareOffset, CeilingHeight, -SquareOffset);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(SquareOffset, CeilingHeight, SquareOffset);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(-SquareOffset, CeilingHeight, SquareOffset);
            glEnd();
        glEndList();
    }

    if (FloorTexture != 0)
    {
        glNewList(DisplayList + FACE_FLOOR, GL_COMPILE);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, FloorTexture);

            glBegin(GL_QUADS);
                glNormal3f(0.0f, 1.0f, 0.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-SquareOffset, FloorHeight, -SquareOffset);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(SquareOffset, FloorHeight, -SquareOffset);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(SquareOffset, FloorHeight, SquareOffset);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(-SquareOffset, FloorHeight, SquareOffset);
            glEnd();
        glEndList();
    }

    if (WallTexture != 0)
    {
        glNewList(DisplayList + FACE_WALL_NEAR, GL_COMPILE);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, WallTexture);

            glBegin(GL_QUADS);
                glNormal3f(0.0f, 0.0f, 1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-SquareOffset, FloorHeight, SquareOffset);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(SquareOffset, FloorHeight, SquareOffset);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(SquareOffset, CeilingHeight, SquareOffset);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(-SquareOffset, CeilingHeight, SquareOffset);
            glEnd();
        glEndList();

        glNewList(DisplayList + FACE_WALL_RIGHT, GL_COMPILE);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, WallTexture);

            glBegin(GL_QUADS);
                glNormal3f(1.0f, 0.0f, 0.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(SquareOffset, FloorHeight, SquareOffset);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(SquareOffset, FloorHeight, -SquareOffset);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(SquareOffset, CeilingHeight, -SquareOffset);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(SquareOffset, CeilingHeight, SquareOffset);
            glEnd();
        glEndList();

        glNewList(DisplayList + FACE_WALL_FAR, GL_COMPILE);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, WallTexture);

            glBegin(GL_QUADS);
                glNormal3f(0.0f, 0.0f, -1.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(SquareOffset, FloorHeight, -SquareOffset);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(-SquareOffset, FloorHeight, -SquareOffset);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(-SquareOffset, CeilingHeight, -SquareOffset);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(SquareOffset, CeilingHeight, -SquareOffset);
            glEnd();
        glEndList();

        glNewList(DisplayList + FACE_WALL_LEFT, GL_COMPILE);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, WallTexture);

            glBegin(GL_QUADS);
                glNormal3f(-1.0f, 0.0f, 0.0f);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-SquareOffset, FloorHeight, -SquareOffset);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(-SquareOffset, FloorHeight, SquareOffset);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(-SquareOffset, CeilingHeight, SquareOffset);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(-SquareOffset, CeilingHeight, -SquareOffset);
            glEnd();
        glEndList();
    }
}

void Dantares2GL::SpaceClassGL::DrawFace(int FaceIndex)
{
    glCallList(DisplayList + FaceIndex);
}

#endif //DANTARES_RENDER_GL.
