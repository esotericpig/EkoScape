/*
 Copyright (C) 2025 Bradley Whited

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

#include "Dantares2GLES.h"

#if defined(DANTARES_RENDER_GLES)

#include<memory>
#include<utility>

Dantares2GLES::Dantares2GLES(float SquareSize, float FloorHeight, float CeilingHeight)
    : Dantares2(SquareSize, FloorHeight, CeilingHeight)
{
}

std::unique_ptr<Dantares2GLES::SpaceClass> Dantares2GLES::BuildSpace(int SpaceID)
{
    return std::make_unique<SpaceClassGLES>(SpaceID);
}

void Dantares2GLES::BeginDraw()
{
}

void Dantares2GLES::EndDraw()
{
}

void Dantares2GLES::TranslateModelMatrix(float /*X*/, float /*Y*/, float /*Z*/)
{
}

void Dantares2GLES::RotateModelMatrix(float /*Angle*/, float /*X*/, float /*Y*/, float /*Z*/)
{
}

void Dantares2GLES::UpdateModelMatrix()
{
}

void Dantares2GLES::PushModelMatrix()
{
}

void Dantares2GLES::PopModelMatrix()
{
}

Dantares2GLES::SpaceClassGLES::SpaceClassGLES(int Type) noexcept
    : SpaceClass(Type)
{
}

Dantares2GLES::SpaceClassGLES::SpaceClassGLES(SpaceClassGLES &&Other) noexcept
    : SpaceClass(std::move(Other))
{
    MoveFrom(std::move(Other));
}

Dantares2GLES::SpaceClassGLES &Dantares2GLES::SpaceClassGLES::operator = (SpaceClassGLES &&Other) noexcept
{
    if (this != &Other)
    {
        SpaceClass::operator = (std::move(Other));
        MoveFrom(std::move(Other));
    }

    return *this;
}

void Dantares2GLES::SpaceClassGLES::MoveFrom(SpaceClassGLES &&/*Other*/) noexcept
{
}

Dantares2GLES::SpaceClassGLES::~SpaceClassGLES() noexcept
{
}

void Dantares2GLES::SpaceClassGLES::GenerateFaces(float /*SquareOffset*/, float /*FloorHeight*/, float /*CeilingHeight*/)
{
}

void Dantares2GLES::SpaceClassGLES::DrawFace(int /*FaceIndex*/)
{
}

#endif //DANTARES_RENDER_GLES.
