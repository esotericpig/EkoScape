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

class Dantares2GL : public Dantares2
{
public:
    explicit Dantares2GL(float SquareSize, float FloorHeight, float CeilingHeight);

    std::unique_ptr<SpaceClass> BuildSpace(int SpaceID) override;

    void BeginDraw() override;
    void EndDraw() override;
    void TranslateModelMatrix(float X, float Y, float Z) override;
    void RotateModelMatrix(float Angle, float X, float Y, float Z) override;
    void UpdateModelMatrix() override;
    void PushModelMatrix() override;
    void PopModelMatrix() override;

private:
    class SpaceClassGL : public SpaceClass
    {
    public:
        explicit SpaceClassGL(int Type) noexcept;

        SpaceClassGL(SpaceClassGL &&Other) noexcept;
        SpaceClassGL &operator = (SpaceClassGL &&Other) noexcept;
        ~SpaceClassGL() noexcept override;

        void GenerateFaces(float SquareOffset, float FloorHeight, float CeilingHeight) override;
        void DrawFace(int FaceIndex) override;

        GLuint DisplayList = 0;

    private:
        void MoveFrom(SpaceClassGL &&Other) noexcept;
        void DeleteDisplayList() noexcept;
    };
};

#endif
