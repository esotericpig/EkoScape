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

#include "Dantares2.h"

#include<iomanip>
#include<utility>

Dantares2::Dantares2(float SquareSize, float FloorHeight, float CeilingHeight)
    : SqSize(SquareSize),
      Floor(FloorHeight),
      Ceiling(CeilingHeight),
      WalkSpeed(SquareSize / 15.0f)
{
}

Dantares2::Dantares2(Dantares2 &&Other) noexcept
{
    MoveFrom(std::move(Other));
}

Dantares2 &Dantares2::operator = (Dantares2 &&Other) noexcept
{
    if (this != &Other)
    {
        MoveFrom(std::move(Other));
    }

    return *this;
}

void Dantares2::MoveFrom(Dantares2 &&Other) noexcept
{
    CurrentMap = std::exchange(Other.CurrentMap, -1);
    NextMapID = std::exchange(Other.NextMapID, 0);
    SqSize = Other.SqSize;
    Floor = Other.Floor;
    Ceiling = Other.Ceiling;
    CameraX = std::exchange(Other.CameraX, 0);
    CameraY = std::exchange(Other.CameraY, 0);
    CameraFacing = std::exchange(Other.CameraFacing, 0);
    Walking = std::exchange(Other.Walking, -1);
    Turning = std::exchange(Other.Turning, 0);
    WalkSpeed = Other.WalkSpeed;
    TurnSpeed = Other.TurnSpeed;
    WalkOffset = std::exchange(Other.WalkOffset, 0.0f);
    TurnOffset = std::exchange(Other.TurnOffset, 0.0f);
    DegreesTurned = std::exchange(Other.DegreesTurned, 0.0f);

    for (int x = 0; x < MAXMAPS; x++)
    {
        Maps[x] = std::move(Other.Maps[x]);
    }
}

int Dantares2::AddMap(const void *Map, int SizeX, int SizeY)
{
    const int NewMapID = NextMapID;

    if (NextMapID >= MAXMAPS)                                            //All map slots are taken.
    {
        return -1;
    }

    Maps[NewMapID] = std::make_unique<MapClass>(*this, SizeX, SizeY);    //Generate new map.
    NextMapID = MAXMAPS;

    for (int x = 0; x < MAXMAPS; x++)                                    //Find the next map ID.
    {
        if (!Maps[x])
        {
            NextMapID = x;
            break;
        }
    }

    for (int x = 0, y = 0, z = 0; z < (SizeX * SizeY); z++)              //Insert map info.
    {
        const int Space = *(static_cast<const int*>(Map) + z);

        Maps[NewMapID]->AddSpaceIfAbsent(Space);
        Maps[NewMapID]->MapArray[x][y] = Space;
        Maps[NewMapID]->WalkArray[x][y] = (Space == 0);

        if (++y >= SizeY)
        {
            y = 0;
            x++;
        }
    }

    return NewMapID;                                                     //Return the new map ID.
}

int Dantares2::AddMap(const int* const *Map, int SizeX, int SizeY)
{
    const int NewMapID = NextMapID;

    if (NextMapID >= MAXMAPS)                                            //All map slots are taken.
    {
        return -1;
    }

    Maps[NewMapID] = std::make_unique<MapClass>(*this, SizeX, SizeY);    //Generate new map.
    NextMapID = MAXMAPS;

    for (int x = 0; x < MAXMAPS; x++)                                    //Find the next map ID.
    {
        if (!Maps[x])
        {
            NextMapID = x;
            break;
        }
    }

    for (int x = 0; x < SizeX; x++)                                      //Insert map info.
    {
        for (int y = 0; y < SizeY; y++)
        {
            Maps[NewMapID]->AddSpaceIfAbsent(Map[x][y]);
            Maps[NewMapID]->MapArray[x][y] = Map[x][y];
            Maps[NewMapID]->WalkArray[x][y] = (Map[x][y] == 0);
        }
    }

    return NewMapID;                                                     //Return the new map ID.
}

bool Dantares2::DeleteMap(int MapID)
{
    if (Maps[MapID])
    {
        Maps[MapID].reset();                                             //Delete the map.
    }
    else
    {
        return false;                                                    //Map doesn't exist.
    }

    if (NextMapID >= MAXMAPS)
    {
        NextMapID = MapID;
    }

    if (MapID == CurrentMap)
    {
        CurrentMap = -1;
    }

    return true;
}

bool Dantares2::IsMap(int MapID) const
{
    if (MapID < 0 || MapID >= MAXMAPS)                                   //MapID out of range.
    {
        return false;
    }

    return Maps[MapID] ? true : false;
}

bool Dantares2::SetWallTexture(int SpaceID, int TextureID, bool Delete)
{
    if (CurrentMap == -1)
    {
        return false;                                                    //No active map.
    }

    auto &Space = Maps[CurrentMap]->AddSpaceIfAbsent(SpaceID);

    if (Delete)
    {
        TextureID = -1;
    }

    Space.WallTexture = TextureID;

    return true;
}

bool Dantares2::SetFloorTexture(int SpaceID, int TextureID, bool Delete)
{
    if (CurrentMap == -1)
    {
        return false;                                                    //No active map.
    }

    auto &Space = Maps[CurrentMap]->AddSpaceIfAbsent(SpaceID);

    if (Delete)
    {
        TextureID = -1;
    }

    Space.FloorTexture = TextureID;

    return true;
}

bool Dantares2::SetCeilingTexture(int SpaceID, int TextureID, bool Delete)
{
    if (CurrentMap == -1)
    {
        return false;                                                    //No active map.
    }

    auto &Space = Maps[CurrentMap]->AddSpaceIfAbsent(SpaceID);

    if (Delete)
    {
        TextureID = -1;
    }

    Space.CeilingTexture = TextureID;

    return true;
}

bool Dantares2::SetMasterFloorTexture(int TextureID, bool Delete)
{
    return SetFloorTexture(0, TextureID, Delete);
}

bool Dantares2::SetMasterCeilingTexture(int TextureID, bool Delete)
{
    return SetCeilingTexture(0, TextureID, Delete);
}

bool Dantares2::SetCurrentMap(int MapID)
{
    if (MapID >= MAXMAPS || MapID < 0 || !Maps[MapID])                   //MapID is out of range, or
    {                                                                    //map doesn't exist.
        return false;
    }

    CurrentMap = MapID;

    return true;
}

int Dantares2::GetCurrentMap() const
{
    return CurrentMap;
}

bool Dantares2::ChangeSquare(int XCoord, int YCoord, int NewType)
{
    if (CurrentMap == -1)
    {
        return false;                                                    //No active map.
    }

    if (XCoord < 0 || YCoord < 0 ||                                      //Space out of range.
        XCoord > Maps[CurrentMap]->XSize ||
        YCoord > Maps[CurrentMap]->YSize)
    {
        return false;
    }

    Maps[CurrentMap]->AddSpaceIfAbsent(NewType);
    Maps[CurrentMap]->MapArray[XCoord][YCoord] = NewType;
    Maps[CurrentMap]->WalkArray[XCoord][YCoord] = (NewType == 0);

    return true;
}

bool Dantares2::MakeSpaceNonWalkable(int XCoord, int YCoord)
{
    if (CurrentMap == -1)
    {
        return false;                                                    //No active map.
    }

    if (XCoord < 0 || YCoord < 0 ||                                      //Space out of range.
        XCoord > Maps[CurrentMap]->XSize ||
        YCoord > Maps[CurrentMap]->YSize)
    {
        return false;
    }

    Maps[CurrentMap]->WalkArray[XCoord][YCoord] = false;

    return true;
}

bool Dantares2::MakeSpaceWalkable(int XCoord, int YCoord)
{
    if (CurrentMap == -1)
    {
        return false;                                                    //No active map.
    }

    if (XCoord < 0 || YCoord < 0 ||                                      //Space out of range.
        XCoord > Maps[CurrentMap]->XSize ||
        YCoord > Maps[CurrentMap]->YSize)
    {
        return false;
    }

    Maps[CurrentMap]->WalkArray[XCoord][YCoord] = true;

    return true;
}

bool Dantares2::SetPlayerPosition(int XCoord, int YCoord)
{
    return SetPlayerPosition(XCoord, YCoord, CameraFacing);
}

bool Dantares2::SetPlayerPosition(int XCoord, int YCoord, int Facing)
{
    if (CurrentMap == -1)
    {
        return false;                                                    //No active map.
    }

    if (XCoord < 0 || YCoord < 0 ||                                      //Space out of range,
        XCoord >= Maps[CurrentMap]->XSize ||                             //or illegal direction.
        YCoord >= Maps[CurrentMap]->YSize ||
        Facing < 0 || Facing > 3)
    {
        return false;
    }

    CameraX = XCoord;
    CameraY = YCoord;
    CameraFacing = Facing;
    Walking = -1;
    WalkOffset = 0.0f;
    Turning = 0;
    TurnOffset = 0.0f;

    return true;
}

bool Dantares2::GenerateMap()
{
    if (CurrentMap == -1)                                                //No active map.
    {
        return false;
    }

    const float Offset = SqSize / 2.0f;

    for (auto &[_Type, Seeker]: Maps[CurrentMap]->SpaceInfo)
    {
        Seeker->ResetDisplayList();

        if (Seeker->CeilingTexture != -1)
        {
            glNewList(Seeker->DisplayList + 5, GL_COMPILE);
                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, Seeker->CeilingTexture);

                glBegin(GL_QUADS);
                    glNormal3f(0.0f, -1.0f, 0.0f);
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex3f(-Offset, Ceiling, -Offset);
                    glTexCoord2f(1.0f, 0.0f);
                    glVertex3f(Offset, Ceiling, -Offset);
                    glTexCoord2f(1.0f, 1.0f);
                    glVertex3f(Offset, Ceiling, Offset);
                    glTexCoord2f(0.0f, 1.0f);
                    glVertex3f(-Offset, Ceiling, Offset);
                glEnd();
            glEndList();
        }

        if (Seeker->FloorTexture != -1)
        {
            glNewList(Seeker->DisplayList + 4, GL_COMPILE);
                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, Seeker->FloorTexture);

                glBegin(GL_QUADS);
                    glNormal3f(0.0f, 1.0f, 0.0f);
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex3f(-Offset, Floor, -Offset);
                    glTexCoord2f(1.0f, 0.0f);
                    glVertex3f(Offset, Floor, -Offset);
                    glTexCoord2f(1.0f, 1.0f);
                    glVertex3f(Offset, Floor, Offset);
                    glTexCoord2f(0.0f, 1.0f);
                    glVertex3f(-Offset, Floor, Offset);
                glEnd();
            glEndList();
        }

        if (Seeker->WallTexture != -1)
        {
            glNewList(Seeker->DisplayList + 2, GL_COMPILE);
                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, Seeker->WallTexture);

                glBegin(GL_QUADS);
                    glNormal3f(0.0f, 0.0f, 1.0f);
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex3f(-Offset, Floor, Offset);
                    glTexCoord2f(1.0f, 0.0f);
                    glVertex3f(Offset, Floor, Offset);
                    glTexCoord2f(1.0f, 1.0f);
                    glVertex3f(Offset, Ceiling, Offset);
                    glTexCoord2f(0.0f, 1.0f);
                    glVertex3f(-Offset, Ceiling, Offset);
                glEnd();
            glEndList();

            glNewList(Seeker->DisplayList + 1, GL_COMPILE);
                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, Seeker->WallTexture);

                glBegin(GL_QUADS);
                    glNormal3f(1.0f, 0.0f, 0.0f);
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex3f(Offset, Floor, Offset);
                    glTexCoord2f(1.0f, 0.0f);
                    glVertex3f(Offset, Floor, -Offset);
                    glTexCoord2f(1.0f, 1.0f);
                    glVertex3f(Offset, Ceiling, -Offset);
                    glTexCoord2f(0.0f, 1.0f);
                    glVertex3f(Offset, Ceiling, Offset);
                glEnd();
            glEndList();

            glNewList(Seeker->DisplayList, GL_COMPILE);
                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, Seeker->WallTexture);

                glBegin(GL_QUADS);
                    glNormal3f(0.0f, 0.0f, -1.0f);
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex3f(Offset, Floor, -Offset);
                    glTexCoord2f(1.0f, 0.0f);
                    glVertex3f(-Offset, Floor, -Offset);
                    glTexCoord2f(1.0f, 1.0f);
                    glVertex3f(-Offset, Ceiling, -Offset);
                    glTexCoord2f(0.0f, 1.0f);
                    glVertex3f(Offset, Ceiling, -Offset);
                glEnd();
            glEndList();

            glNewList(Seeker->DisplayList + 3, GL_COMPILE);
                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, Seeker->WallTexture);

                glBegin(GL_QUADS);
                    glNormal3f(-1.0f, 0.0f, 0.0f);
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex3f(-Offset, Floor, -Offset);
                    glTexCoord2f(1.0f, 0.0f);
                    glVertex3f(-Offset, Floor, Offset);
                    glTexCoord2f(1.0f, 1.0f);
                    glVertex3f(-Offset, Ceiling, Offset);
                    glTexCoord2f(0.0f, 1.0f);
                    glVertex3f(-Offset, Ceiling, -Offset);
                glEnd();
            glEndList();
        }
    }

    return true;
}

bool Dantares2::Draw(int Distance, bool MovePlayer)
{
    if (CurrentMap == -1)                                                //No active map.
    {
        return false;
    }

    const int XBound = Maps[CurrentMap]->XSize;
    const int YBound = Maps[CurrentMap]->YSize;
    const int HalfDistance = Distance / 2;
    const auto CameraXf = static_cast<float>(CameraX);
    const auto CameraYf = static_cast<float>(CameraY);

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);

    glTranslatef(0.0f, 0.0f, -(SqSize / 2.0f));
    glRotatef(static_cast<float>(CameraFacing) * 90.0f + TurnOffset, 0.0f, 1.0f, 0.0f);

    switch (CameraFacing)
    {
        case 0:
        case 2:
            if (Walking == 1 || Walking == 3)
            {
                glTranslatef(-(CameraXf * SqSize + WalkOffset), 0.0f, CameraYf * SqSize);
            }
            else
            {
                glTranslatef(-(CameraXf * SqSize), 0.0f, CameraYf * SqSize + WalkOffset);
            }
            break;

        case 1:
        case 3:
            if (Walking == 0 || Walking == 2)
            {
                glTranslatef(-(CameraXf * SqSize), 0.0f, CameraYf * SqSize + WalkOffset);
            }
            else
            {
                glTranslatef(-(CameraXf * SqSize + WalkOffset), 0.0f, CameraYf * SqSize);
            }
            break;
    }

    switch (CameraFacing)
    {
        case 0:
            for (int x = (CameraX > Distance) ? (CameraX - Distance) : 0;
                 x < XBound && x < (CameraX + Distance); x++)
            {
                glPushMatrix();
                glTranslatef(static_cast<float>(x) * SqSize, 0.0f, SqSize);

                for (int y = (CameraY > HalfDistance) ? (CameraY - HalfDistance) : 0;
                     y < YBound && y < (CameraY + Distance); y++)
                {
                    if (y == (CameraY - HalfDistance))
                    {
                        glTranslatef(0.0f, 0.0f, -(SqSize * static_cast<float>(y)));
                    }

                    glTranslatef(0.0f, 0.0f, -SqSize);

                    const SpaceClass *Seeker = Maps[CurrentMap]->FindSpace(Maps[CurrentMap]->MapArray[x][y]);

                    if (Seeker->WallTexture != -1)
                    {
                        if (y >= CameraY)
                        {
                            glCallList(Seeker->DisplayList + 2);
                        }

                        if (x < CameraX)
                        {
                            glCallList(Seeker->DisplayList + 1);
                        }
                        else if (x > CameraX)
                        {
                            glCallList(Seeker->DisplayList + 3);
                        }
                    }

                    if (Seeker->FloorTexture != -1)
                    {
                        glCallList(Seeker->DisplayList + 4);
                    }

                    if (Seeker->CeilingTexture != -1)
                    {
                        glCallList(Seeker->DisplayList + 5);
                    }
                }

                glPopMatrix();
            }
            break;

        case 1:
            for (int x = (CameraX > HalfDistance) ? (CameraX - HalfDistance) : 0;
                 x < XBound && x < (CameraX + Distance); x++)
            {
                glPushMatrix();
                glTranslatef(static_cast<float>(x) * SqSize, 0.0f, SqSize);

                for (int y = (CameraY > Distance) ? (CameraY - Distance) : 0;
                     y < YBound && y < (CameraY + Distance); y++)
                {
                    if (y == (CameraY - Distance))
                    {
                        glTranslatef(0.0f, 0.0f, -(SqSize * static_cast<float>(y)));
                    }

                    glTranslatef(0.0f, 0.0f, -SqSize);

                    const SpaceClass *Seeker = Maps[CurrentMap]->FindSpace(Maps[CurrentMap]->MapArray[x][y]);

                    if (Seeker->WallTexture != -1)
                    {
                        if (x >= CameraX)
                        {
                            glCallList(Seeker->DisplayList + 3);
                        }

                        if (y > CameraY)
                        {
                            glCallList(Seeker->DisplayList + 2);
                        }
                        else if (y < CameraY)
                        {
                            glCallList(Seeker->DisplayList);
                        }
                    }

                    if (Seeker->FloorTexture != -1)
                    {
                        glCallList(Seeker->DisplayList + 4);
                    }

                    if (Seeker->CeilingTexture != -1)
                    {
                        glCallList(Seeker->DisplayList + 5);
                    }
                }

                glPopMatrix();
            }
            break;

        case 2:
            for (int x = (CameraX > Distance) ? (CameraX - Distance) : 0;
                 x < XBound && x < (CameraX + Distance); x++)
            {
                glPushMatrix();
                glTranslatef(static_cast<float>(x) * SqSize, 0.0f, SqSize);

                for (int y = (CameraY > Distance) ? (CameraY - Distance) : 0;
                     y < YBound && y < (CameraY + HalfDistance); y++)
                {
                    if (y == (CameraY - Distance))
                    {
                        glTranslatef(0.0f, 0.0f, -(SqSize * static_cast<float>(y)));
                    }

                    glTranslatef(0.0f, 0.0f, -SqSize);

                    const SpaceClass *Seeker = Maps[CurrentMap]->FindSpace(Maps[CurrentMap]->MapArray[x][y]);

                    if (Seeker->WallTexture != -1)
                    {
                        if (y <= CameraY)
                        {
                            glCallList(Seeker->DisplayList + 0);
                        }

                        if (x > CameraX)
                        {
                            glCallList(Seeker->DisplayList + 3);
                        }
                        else if (x < CameraX)
                        {
                            glCallList(Seeker->DisplayList + 1);
                        }
                    }

                    if (Seeker->FloorTexture != -1)
                    {
                        glCallList(Seeker->DisplayList + 4);
                    }

                    if (Seeker->CeilingTexture != -1)
                    {
                        glCallList(Seeker->DisplayList + 5);
                    }
                }

                glPopMatrix();
            }
            break;

        case 3:
            for (int x = (CameraX > Distance) ? (CameraX - Distance) : 0;
                 x < XBound && x < (CameraX + HalfDistance); x++)
            {
                glPushMatrix();
                glTranslatef(static_cast<float>(x) * SqSize, 0.0f, SqSize);

                for (int y = (CameraY > Distance) ? (CameraY - Distance) : 0;
                     y < YBound && y < (CameraY + Distance); y++)
                {
                    if (y == (CameraY - Distance))
                    {
                        glTranslatef(0.0f, 0.0f, -(SqSize * static_cast<float>(y)));
                    }

                    glTranslatef(0.0f, 0.0f, -SqSize);

                    const SpaceClass *Seeker = Maps[CurrentMap]->FindSpace(Maps[CurrentMap]->MapArray[x][y]);

                    if (Seeker->WallTexture != -1)
                    {
                        if (x <= CameraX)
                        {
                            glCallList(Seeker->DisplayList + 1);
                        }

                        if (y < CameraY)
                        {
                            glCallList(Seeker->DisplayList);
                        }
                        else if (y > CameraY)
                        {
                            glCallList(Seeker->DisplayList + 2);
                        }
                    }

                    if (Seeker->FloorTexture != -1)
                    {
                        glCallList(Seeker->DisplayList + 4);
                    }

                    if (Seeker->CeilingTexture != -1)
                    {
                        glCallList(Seeker->DisplayList + 5);
                    }
                }

                glPopMatrix();
            }
            break;
    }

    if (MovePlayer)
    {
        if (WalkOffset >= SqSize)
        {
            WalkOffset = 0;

            if (Walking == 0)
            {
                CameraY++;
            }
            else
            {
                CameraX++;
            }

            Walking = -1;
        }
        else if (WalkOffset <= -SqSize)
        {
            WalkOffset = 0;

            if (Walking == 2)
            {
                CameraY--;
            }
            else
            {
                CameraX--;
            }

            Walking = -1;
        }
        else if (WalkOffset > 0.0f)
        {
            WalkOffset = WalkOffset + WalkSpeed;

            if (WalkOffset > SqSize)
            {
                WalkOffset = SqSize;
            }
        }
        else if (WalkOffset < 0.0f)
        {
            WalkOffset = WalkOffset - WalkSpeed;

            if (WalkOffset < -SqSize)
            {
                WalkOffset = -SqSize;
            }
        }

        if (Turning > 0)
        {
            TurnOffset = TurnOffset + TurnSpeed;
            DegreesTurned = DegreesTurned + TurnSpeed;
        }
        else if (Turning < 0)
        {
            TurnOffset = TurnOffset - TurnSpeed;
            DegreesTurned = DegreesTurned + TurnSpeed;
        }

        if (TurnOffset > 45.0f && Turning == 1)
        {
            Turning = 2;
            CameraFacing = (CameraFacing + 1) % 4;
            TurnOffset = -90.0f + TurnOffset;
        }

        if (TurnOffset < -45.0f && Turning == -1)
        {
            Turning = -2;

            if (--CameraFacing == -1)
            {
                CameraFacing = 3;
            }

            TurnOffset = 90.0f + TurnOffset;
        }

        if (DegreesTurned >= 90.0f)
        {
            Turning = 0;
            TurnOffset = 0.0f;
            DegreesTurned = 0.0f;
        }
    }

    glPopAttrib();

    return true;
}

bool Dantares2::StepForward(bool Force)
{
    if (Walking > -1 || Turning != 0 || CurrentMap == -1)
    {
        return false;
    }

    switch (CameraFacing)
    {
        case 0:
            if ((CameraY + 1) < Maps[CurrentMap]->YSize &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX][CameraY + 1]))
            {
                Walking = 0;
                WalkOffset = WalkOffset + WalkSpeed;

                return true;
            }
            return false;

        case 1:
            if ((CameraX + 1) < Maps[CurrentMap]->XSize &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX + 1][CameraY]))
            {
                Walking = 1;
                WalkOffset = WalkOffset + WalkSpeed;

                return true;
            }
            return false;

        case 2:
            if (CameraY > 0 &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX][CameraY - 1]))
            {
                Walking = 2;
                WalkOffset = WalkOffset - WalkSpeed;

                return true;
            }
            return false;

        case 3:
            if (CameraX > 0 &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX - 1][CameraY]))
            {
                Walking = 3;
                WalkOffset = WalkOffset - WalkSpeed;

                return true;
            }
            return false;
    }

    return false;
}

bool Dantares2::StepBackward(bool Force)
{
    if (Walking > -1 || Turning != 0 || CurrentMap == -1)
    {
        return false;
    }

    switch (CameraFacing)
    {
        case 0:
            if (CameraY > 0 &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX][CameraY - 1]))
            {
                Walking = 2;
                WalkOffset = WalkOffset - WalkSpeed;

                return true;
            }
            return false;

        case 1:
            if (CameraX > 0 &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX - 1][CameraY]))
            {
                Walking = 3;
                WalkOffset = WalkOffset - WalkSpeed;

                return true;
            }
            return false;

        case 2:
            if ((CameraY + 1) < Maps[CurrentMap]->YSize &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX][CameraY + 1]))
            {
                Walking = 0;
                WalkOffset = WalkOffset + WalkSpeed;

                return true;
            }
            return false;

        case 3:
            if ((CameraX + 1) < Maps[CurrentMap]->XSize &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX + 1][CameraY]))
            {
                Walking = 1;
                WalkOffset = WalkOffset + WalkSpeed;

                return true;
            }
            return false;
    }

    return false;
}

bool Dantares2::StepLeft(bool Force)
{
    if (Walking > -1 || Turning != 0 || CurrentMap == -1)
    {
        return false;
    }

    switch (CameraFacing)
    {
        case 0:
            if (CameraX > 0 &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX - 1][CameraY]))
            {
                Walking = 3;
                WalkOffset = WalkOffset - WalkSpeed;

                return true;
            }
            return false;

        case 1:
            if ((CameraY + 1) < Maps[CurrentMap]->YSize &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX][CameraY + 1]))
            {
                Walking = 0;
                WalkOffset = WalkOffset + WalkSpeed;

                return true;
            }
            return false;

        case 2:
            if ((CameraX + 1) < Maps[CurrentMap]->XSize &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX + 1][CameraY]))
            {
                Walking = 1;
                WalkOffset = WalkOffset + WalkSpeed;

                return true;
            }
            return false;

        case 3:
            if (CameraY > 0 &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX][CameraY - 1]))
            {
                Walking = 2;
                WalkOffset = WalkOffset - WalkSpeed;

                return true;
            }
            return false;
    }

    return false;
}

bool Dantares2::StepRight(bool Force)
{
    if (Walking > -1 || Turning != 0 || CurrentMap == -1)
    {
        return false;
    }

    switch (CameraFacing)
    {
        case 0:
            if ((CameraX + 1) < Maps[CurrentMap]->XSize &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX + 1][CameraY]))
            {
                Walking = 1;
                WalkOffset = WalkOffset + WalkSpeed;

                return true;
            }
            return false;

        case 1:
            if (CameraY > 0 &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX][CameraY - 1]))
            {
                Walking = 2;
                WalkOffset = WalkOffset - WalkSpeed;

                return true;
            }
            return false;

        case 2:
            if (CameraX > 0 &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX - 1][CameraY]))
            {
                Walking = 3;
                WalkOffset = WalkOffset - WalkSpeed;

                return true;
            }
            return false;

        case 3:
            if ((CameraY + 1) < Maps[CurrentMap]->YSize &&
                (Force || Maps[CurrentMap]->WalkArray[CameraX][CameraY + 1]))
            {
                Walking = 0;
                WalkOffset = WalkOffset + WalkSpeed;

                return true;
            }
            return false;
    }

    return false;
}

bool Dantares2::TurnLeft()
{
    if (Walking > -1 || Turning != 0 || CurrentMap == -1)
    {
        return false;
    }

    Turning = -1;
    TurnOffset = TurnOffset - TurnSpeed;
    DegreesTurned = DegreesTurned + TurnSpeed;

    return true;
}

bool Dantares2::TurnRight()
{
    if (Walking > -1 || Turning != 0 || CurrentMap == -1)
    {
        return false;
    }

    Turning = 1;
    TurnOffset = TurnOffset + TurnSpeed;
    DegreesTurned = DegreesTurned + TurnSpeed;

    return true;
}

bool Dantares2::SetWalkingSpeed(float WSpeed)
{
    if (WSpeed == 0.0f)
    {
        WalkSpeed = SqSize / 15.0f;

        return true;
    }

    if (WSpeed < 0.0f)
    {
        return false;
    }

    WalkSpeed = SqSize / WSpeed;

    return true;
}

bool Dantares2::SetTurningSpeed(float TSpeed)
{
    if (TSpeed == 0.0f)
    {
        TurnSpeed = 5.0f;

        return true;
    }

    if (TSpeed < 0.0f || TSpeed > 90.0f)
    {
        return false;
    }

    TurnSpeed = TSpeed;

    return true;
}

int Dantares2::IsWalking() const
{
    return Walking;
}

float Dantares2::GetWalkOffset() const
{
    return WalkOffset;
}

int Dantares2::IsTurning() const
{
    if (Turning < 0)
    {
        return -1;
    }

    if (Turning > 0)
    {
        return 1;
    }

    return 0;
}

float Dantares2::GetTurnOffset() const
{
    return TurnOffset;
}

int Dantares2::GetPlayerX() const
{
    if (CurrentMap == -1)
    {
        return -1;
    }

    return CameraX;
}

int Dantares2::GetPlayerY() const
{
    if (CurrentMap == -1)
    {
        return -1;
    }

    return CameraY;
}

int Dantares2::GetPlayerFacing() const
{
    if (CurrentMap == -1)
    {
        return -1;
    }

    return CameraFacing;
}

int Dantares2::GetCurrentSpace() const
{
    if (CurrentMap == -1)
    {
        return -1;
    }

    return Maps[CurrentMap]->MapArray[CameraX][CameraY];
}

int Dantares2::GetSpace(int XCoord, int YCoord) const
{
    if (CurrentMap == -1 || XCoord < 0 || YCoord < 0 ||
        XCoord >= Maps[CurrentMap]->XSize ||
        YCoord >= Maps[CurrentMap]->YSize)
    {
        return -1;
    }

    return Maps[CurrentMap]->MapArray[XCoord][YCoord];
}

bool Dantares2::SpaceIsWalkable(int XCoord, int YCoord) const
{
    if (CurrentMap == -1 || XCoord < 0 || YCoord < 0 ||
        XCoord >= Maps[CurrentMap]->XSize ||
        YCoord >= Maps[CurrentMap]->YSize)
    {
        return false;
    }

    return Maps[CurrentMap]->WalkArray[XCoord][YCoord];
}

void Dantares2::PrintDebugInfo(std::ostream &Out) const
{
    constexpr int Indent = 4;

    Out << "{Dantares2}"
        << "\nCurrentMap:       " << CurrentMap
        << "\nNextMapID:        " << NextMapID
        << "\nSqSize:           " << SqSize
        << "\nFloor:            " << Floor
        << "\nCeiling:          " << Ceiling
        << "\nCameraX:          " << CameraX
        << "\nCameraY:          " << CameraY
        << "\nCameraFacing:     " << CameraFacing
        << "\nWalking:          " << Walking
        << "\nTurning:          " << Turning
        << "\nWalkSpeed:        " << WalkSpeed
        << "\nTurnSpeed:        " << TurnSpeed
        << "\nWalkOffset:       " << WalkOffset
        << "\nTurnOffset:       " << TurnOffset
        << "\nDegreesTurned:    " << DegreesTurned
        ;

    const std::string Indl = '\n' + std::string(Indent, ' ');

    std::cout << "\nMaps:";

    for (int i = 0; i < MAXMAPS; i++)
    {
        Out << Indl << "{Map[" << i << "]} = ";

        if(Maps[i])
        {
            Out << Maps[i].get() << '\n';
            Maps[i]->PrintDebugInfo(Out, Indent);
        }
        else
        {
            Out << "nullptr";
        }
    }

    Out << std::endl;
}

Dantares2::SpaceClass::SpaceClass(int Type)
    : SpaceType(Type),
      DisplayList(static_cast<int>(glGenLists(DISPLAY_LIST_RANGE)))
{
}

Dantares2::SpaceClass::SpaceClass(SpaceClass &&Other) noexcept
{
    MoveFrom(std::move(Other));
}

Dantares2::SpaceClass &Dantares2::SpaceClass::operator = (SpaceClass &&Other) noexcept
{
    if (this != &Other)
    {
        MoveFrom(std::move(Other));
    }

    return *this;
}

void Dantares2::SpaceClass::MoveFrom(SpaceClass &&Other) noexcept
{
    DeleteDisplayList();

    SpaceType = std::exchange(Other.SpaceType, 0);
    FloorTexture = std::exchange(Other.FloorTexture, -1);
    CeilingTexture = std::exchange(Other.CeilingTexture, -1);
    WallTexture = std::exchange(Other.WallTexture, -1);
    DisplayList = std::exchange(Other.DisplayList, -1);
}

Dantares2::SpaceClass::~SpaceClass() noexcept
{
    DeleteDisplayList();
}

void Dantares2::SpaceClass::DeleteDisplayList() noexcept
{
    if (DisplayList != -1)
    {
        glDeleteLists(DisplayList, DISPLAY_LIST_RANGE);
        DisplayList = -1;
    }
}

void Dantares2::SpaceClass::ResetDisplayList()
{
    DeleteDisplayList();
    DisplayList = static_cast<int>(glGenLists(DISPLAY_LIST_RANGE));
}

void Dantares2::SpaceClass::PrintDebugInfo(std::ostream &Out, int Indent) const
{
    const std::string Ind(Indent, ' ');
    const std::string Indl = '\n' + Ind;

    Out << Ind  << "SpaceType:         " << SpaceType
        << Indl << "FloorTexture:      " << FloorTexture
        << Indl << "CeilingTexture:    " << CeilingTexture
        << Indl << "WallTexture:       " << WallTexture
        << Indl << "DisplayList:       " << DisplayList
        ;
    Out.flush();
}

Dantares2::MapClass::MapClass(Dantares2 &Dan,int MaxX, int MaxY)
    : Parent(Dan),
      MapArray(MaxX, std::vector(MaxY, 0)),
      WalkArray(MaxX, std::vector(MaxY, true)),
      XSize(MaxX),
      YSize(MaxY)
{
}

Dantares2::MapClass::MapClass(MapClass &&Other) noexcept
    : Parent(Other.Parent)
{
    MoveFrom(std::move(Other));
}

Dantares2::MapClass &Dantares2::MapClass::operator = (MapClass &&Other) noexcept
{
    if (this != &Other)
    {
        MoveFrom(std::move(Other));
    }

    return *this;
}

void Dantares2::MapClass::MoveFrom(MapClass &&Other) noexcept
{
    MapArray = std::move(Other.MapArray);
    WalkArray = std::move(Other.WalkArray);
    SpaceInfo = std::move(Other.SpaceInfo);
    XSize = std::exchange(Other.XSize, 0);
    YSize = std::exchange(Other.YSize, 0);
}

Dantares2::SpaceClass &Dantares2::MapClass::AddSpaceIfAbsent(int SpaceID)
{
    auto [It, IsNew] = SpaceInfo.try_emplace(SpaceID);

    if(IsNew)
    {
        It->second = std::move(Parent.BuildSpace(SpaceID));
    }

    return *It->second;
}

Dantares2::SpaceClass *Dantares2::MapClass::FindSpace(int SpaceID)
{
    const auto It = SpaceInfo.find(SpaceID);

    if (It == SpaceInfo.end())
    {
        return nullptr;
    }

    return It->second.get();
}

void Dantares2::MapClass::PrintDebugInfo(std::ostream &Out, int Indent) const
{
    const std::string Ind(Indent, ' ');
    std::string Indl = '\n' + Ind;

    Out << Ind  << "XSize:    " << XSize
        << Indl << "YSize:    " << YSize
        ;

    Indent *= 2;
    Indl = '\n' + std::string(Indent, ' ');

    Out << Indl << "{MapArray} = " << MapArray.data();
    for (int y = 0; y < YSize; y++)
    {
        Out << Indl;
        for (int x = 0; x < XSize; x++)
        {
            Out << std::setw(4) << MapArray[x][y] << ' ';
        }
    }

    Out << Indl << "{WalkArray} = " << WalkArray.data();
    for (int y = 0; y < YSize; y++)
    {
        Out << Indl;
        for (int x = 0; x < XSize; x++)
        {
            Out << (WalkArray[x][y] ? ' ' : '1') << ' ';
        }
    }

    Out << Indl << "{SpaceInfo} = " << SpaceInfo.size() << " spaces";
    if (!SpaceInfo.empty())
    {
        const auto *Space = SpaceInfo.begin()->second.get();

        Out << Indl << "{Space} = " << Space
            << '\n';
        Space->PrintDebugInfo(Out, Indent);
    }

    Out.flush();
}
