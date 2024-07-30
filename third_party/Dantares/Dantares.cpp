/*
 Copyright (C) 2006 Ryan Witmer

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

/*  Implementation of The Dantares Engine v1.0
	All code written by Ryan Witmer in Apple Xcode
	ryan@averagesoftware.com
	www.averagesoftware.com
*/

#include "Dantares.h"

Dantares::Dantares(float SquareSize, float FloorHeight, float CeilingHeight)
{
	SqSize=SquareSize;
	Floor=FloorHeight;
	Ceiling=CeilingHeight;
	NextMapID=0;
	CurrentMap=-1;
	CameraX=0;
	CameraY=0;
	CameraFacing=0;
	Walking=-1;
	Turning=0;
	WalkSpeed=SquareSize/15.0;
	TurnSpeed=5.0;
	WalkOffset=0.0;
	TurnOffset=0.0;
	DegreesTurned=0.0;

	for (int x=0; x<MAXMAPS; x++)
	{
		Maps[x]=NULL;
	}
}

Dantares::Dantares(const Dantares &Copy)
{
	CurrentMap = Copy.CurrentMap;
	NextMapID = Copy.NextMapID;
	SqSize = Copy.SqSize;
	Floor = Copy.Floor;
	Ceiling = Copy.Ceiling;
	CameraX = Copy.CameraX;
	CameraY = Copy.CameraY;
	CameraFacing = Copy.CameraFacing;
	Walking = Copy.Walking;
	Turning = Copy.Turning;
	WalkSpeed = Copy.WalkSpeed;
	TurnSpeed = Copy.TurnSpeed;
	WalkOffset = Copy.WalkOffset;
	TurnOffset = Copy.TurnOffset;
	DegreesTurned = Copy.DegreesTurned;

	for (int x = 0; x < MAXMAPS; x++)
	{
		if (Copy.Maps[x] != NULL)
		{
			Maps[x] = new MapClass(*Copy.Maps[x]);
		}
		else
		{
			Maps[x] = NULL;
		}
	}
}

Dantares Dantares::operator = (const Dantares &r)
{
	CurrentMap = r.CurrentMap;
	NextMapID = r.NextMapID;
	SqSize = r.SqSize;
	Floor = r.Floor;
	Ceiling = r.Ceiling;
	CameraX = r.CameraX;
	CameraY = r.CameraY;
	CameraFacing = r.CameraFacing;
	Walking = r.Walking;
	Turning = r.Turning;
	WalkSpeed = r.WalkSpeed;
	TurnSpeed = r.TurnSpeed;
	WalkOffset = r.WalkOffset;
	TurnOffset = r.TurnOffset;
	DegreesTurned = r.DegreesTurned;

	for (int x = 0; x < MAXMAPS; x++)
	{
		delete Maps[x];

		if (r.Maps[x] != NULL)
		{
			Maps[x] = new MapClass(*r.Maps[x]);
		}
		else
		{
			Maps[x] = NULL;
		}
	}

	return *this;
}

Dantares::~Dantares()
{
	for (int x=0; x<MAXMAPS; x++)
	{
		delete Maps[x];
	}
}

int Dantares::AddMap(const void *Map, int SizeX, int SizeY)
{
	int **TempMap;
	int NewMapID=NextMapID;
	int x=0;
	int y=0;
	int z=0;

	if (NextMapID>=MAXMAPS)											//All map slots are taken.
	{
		return -1;
	}

	TempMap=new int*[SizeX];

	for (z=0; z<SizeX; z++)
	{
		TempMap[z]=new int[SizeY];
	}

	for (z=0; z<SizeX*SizeY; z++)
	{
		TempMap[x][y]=*(static_cast<const int*>(Map)+z);

		if (++y==SizeY)
		{
			y=0;
			x++;
		}
	}

	Maps[NewMapID]=new MapClass(SizeX, SizeY);					//Generate new map.

	if (Maps[NewMapID]==NULL)									//Out of memory.
	{
		return -1;
	}

	NextMapID=MAXMAPS;

	for (x=0; x<MAXMAPS; x++)									//Find the next map ID.
	{
		if (Maps[x]==NULL)
		{
			NextMapID=x;
		}
	}

	for (x=0; x<SizeX; x++)										//Insert map info.
	{
		for (y=0; y<SizeY; y++)
		{
			if (!Maps[NewMapID]->SpaceDefined(TempMap[x][y]))
			{
				Maps[NewMapID]->AddSpace(TempMap[x][y]);
			}

			Maps[NewMapID]->MapArray[x][y]=TempMap[x][y];
			Maps[NewMapID]->WalkArray[x][y]=TempMap[x][y]==0?true:false;
		}
	}

	for (x=0; x<SizeX; x++)
	{
		delete [] TempMap[x];
	}

	delete [] TempMap;

	return NewMapID;											//Return the new map ID.
}

int Dantares::AddMap(const int **Map, int SizeX, int SizeY)
{
	int NewMapID=NextMapID;
	int x=0;
	int y=0;

	if (NextMapID>=MAXMAPS)										//All map slots are taken.
	{
		return -1;
	}

	Maps[NewMapID]=new MapClass(SizeX, SizeY);					//Generate new map.

	if (Maps[NewMapID]==NULL)									//Out of memory.
	{
		return -1;
	}

	NextMapID=MAXMAPS;

	for (x=0; x<MAXMAPS; x++)									//Find the next map ID.
	{
		if (Maps[x]==NULL)
		{
			NextMapID=x;
		}
	}

	for (x=0; x<SizeX; x++)										//Insert map info.
	{
		for (y=0; y<SizeY; y++)
		{
			if (!Maps[NewMapID]->SpaceDefined(Map[x][y]))
			{
				Maps[NewMapID]->AddSpace(Map[x][y]);
			}

			Maps[NewMapID]->MapArray[x][y]=Map[x][y];
			Maps[NewMapID]->WalkArray[x][y]=Map[x][y]==0?true:false;
		}
	}

	return NewMapID;											//Return the new map ID.
}

bool Dantares::DeleteMap(int MapID)
{
	if (Maps[MapID]!=NULL)
	{
		delete Maps[MapID];										//Delete the map.
		Maps[MapID]=NULL;
	}
	else
	{
		return false;											//Map doesn't exist.
	}

	if (NextMapID>=MAXMAPS)
	{
		NextMapID=MapID;
	}

	if (MapID==CurrentMap)
	{
		CurrentMap=-1;
	}

	return true;
}

bool Dantares::IsMap(int MapID) const
{
	if (MapID<0 || MapID>=MAXMAPS)								//MapID out of range.
	{
		return false;
	}

	return !(Maps[MapID]==NULL);
}

bool Dantares::SetWallTexture(int SpaceID, int TextureID, bool Delete)
{
	if (CurrentMap==-1)
	{
		return false;											//No active map.
	}

	if (!Maps[CurrentMap]->SpaceDefined(SpaceID))
	{
		Maps[CurrentMap]->AddSpace(SpaceID);
	}

	if (Delete)
	{
		TextureID=-1;
	}

	Maps[CurrentMap]->FindSpace(SpaceID)->WallTexture=TextureID;

	return true;
}

bool Dantares::SetFloorTexture(int SpaceID, int TextureID, bool Delete)
{
	if (CurrentMap==-1)
	{
		return false;											//No active map.
	}

	if (!Maps[CurrentMap]->SpaceDefined(SpaceID))
	{
		Maps[CurrentMap]->AddSpace(SpaceID);
	}

	if (Delete)
	{
		TextureID=-1;
	}

	Maps[CurrentMap]->FindSpace(SpaceID)->FloorTexture=TextureID;

	return true;
}

bool Dantares::SetCeilingTexture(int SpaceID, int TextureID, bool Delete)
{
	if (CurrentMap==-1)
	{
		return false;											//No active map.
	}

	if (!Maps[CurrentMap]->SpaceDefined(SpaceID))
	{
		Maps[CurrentMap]->AddSpace(SpaceID);
	}

	if (Delete)
	{
		TextureID=-1;
	}

	Maps[CurrentMap]->FindSpace(SpaceID)->CeilingTexture=TextureID;

	return true;
}

bool Dantares::SetMasterFloorTexture(int TextureID, bool Delete)
{
	if (CurrentMap==-1)
	{
		return false;											//No active map.
	}

	if (!Maps[CurrentMap]->SpaceDefined(0))
	{
		Maps[CurrentMap]->AddSpace(0);
	}

	if (Delete)
	{
		TextureID=-1;
	}

	Maps[CurrentMap]->FindSpace(0)->FloorTexture=TextureID;

	return true;
}

bool Dantares::SetMasterCeilingTexture(int TextureID, bool Delete)
{
	if (CurrentMap==-1)
	{
		return false;											//No active map.
	}

	if (!Maps[CurrentMap]->SpaceDefined(0))
	{
		Maps[CurrentMap]->AddSpace(0);
	}

	Maps[CurrentMap]->FindSpace(0)->CeilingTexture=TextureID;

	return true;
}

bool Dantares::SetCurrentMap(int MapID)
{
	if (MapID>=MAXMAPS || MapID<0 || Maps[MapID]==NULL)			//MapID is out of range, or
	{															//map doesn't exist.
		return false;
	}

	CurrentMap=MapID;

	return true;
}

int Dantares::GetCurrentMap() const
{
	return CurrentMap;
}

bool Dantares::ChangeSquare(int XCoord, int YCoord, int NewType)
{
	if (CurrentMap==-1)
	{
		return false;											//No active map.
	}

	if (XCoord<0 || YCoord<0 ||									//Space out of range.
		XCoord>Maps[CurrentMap]->XSize ||
		YCoord>Maps[CurrentMap]->YSize)
	{
		return false;
	}

	Maps[CurrentMap]->MapArray[XCoord][YCoord]=NewType;
	Maps[CurrentMap]->WalkArray[XCoord][YCoord]=NewType==0?true:false;

	if (!Maps[CurrentMap]->SpaceDefined(NewType))
	{
		Maps[CurrentMap]->AddSpace(NewType);
	}

	return true;
}

bool Dantares::MakeSpaceNonWalkable(int XCoord, int YCoord)
{
	if (CurrentMap==-1)
	{
		return false;											//No active map.
	}

	if (XCoord<0 || YCoord<0 ||									//Space out of range.
		XCoord>Maps[CurrentMap]->XSize ||
		YCoord>Maps[CurrentMap]->YSize)
	{
		return false;
	}

	Maps[CurrentMap]->WalkArray[XCoord][YCoord]=false;

	return true;
}

bool Dantares::MakeSpaceWalkable(int XCoord, int YCoord)
{
	if (CurrentMap==-1)
	{
		return false;											//No active map.
	}

	if (XCoord<0 || YCoord<0 ||									//Space out of range.
		XCoord>Maps[CurrentMap]->XSize ||
		YCoord>Maps[CurrentMap]->YSize)
	{
		return false;
	}

	Maps[CurrentMap]->WalkArray[XCoord][YCoord]=true;

	return true;
}

bool Dantares::SetPlayerPosition(int XCoord, int YCoord, int Facing)
{
	if (CurrentMap==-1)
	{
		return false;											//No active map.
	}

	if (XCoord<0 || YCoord<0 ||									//Space out of range,
		XCoord >= Maps[CurrentMap]->XSize ||					//or illegal direction.
		YCoord >= Maps[CurrentMap]->YSize ||
		Facing<0 || Facing>3)
	{
		return false;
	}

	CameraX=XCoord;
	CameraY=YCoord;
	CameraFacing=Facing;
	Walking=-1;
	WalkOffset=0.0;
	Turning=0;
	TurnOffset=0.0;

	return true;
}

bool Dantares::GenerateMap()
{
	if (CurrentMap==-1)											//No active map.
	{
		return false;
	}

	float Offset=SqSize/2.0f;

	SpaceClass *Seeker=Maps[CurrentMap]->SpaceInfo;

	while (Seeker!=NULL)
	{
		glDeleteLists(Seeker->DisplayList, 6);
		Seeker->DisplayList=glGenLists(6);

		if (Seeker->CeilingTexture!=-1)
		{
			glNewList(Seeker->DisplayList+5, GL_COMPILE);
				glEnable(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, Seeker->CeilingTexture);

				glBegin(GL_QUADS);
					glNormal3f(0.0, -1.0, 0.0);
					glTexCoord2f(0.0, 0.0);
					glVertex3f(-Offset, Ceiling, -Offset);
					glTexCoord2f(1.0, 0.0);
					glVertex3f(Offset, Ceiling, -Offset);
					glTexCoord2f(1.0, 1.0);
					glVertex3f(Offset, Ceiling, Offset);
					glTexCoord2f(0.0, 1.0);
					glVertex3f(-Offset, Ceiling, Offset);
				glEnd();
			glEndList();
		}

		if (Seeker->FloorTexture!=-1)
		{
			glNewList(Seeker->DisplayList+4, GL_COMPILE);
				glEnable(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, Seeker->FloorTexture);

				glBegin(GL_QUADS);
					glNormal3f(0.0, 1.0, 0.0);
					glTexCoord2f(0.0, 0.0);
					glVertex3f(-Offset, Floor, -Offset);
					glTexCoord2f(1.0, 0.0);
					glVertex3f(Offset, Floor, -Offset);
					glTexCoord2f(1.0, 1.0);
					glVertex3f(Offset, Floor, Offset);
					glTexCoord2f(0.0, 1.0);
					glVertex3f(-Offset, Floor, Offset);
				glEnd();
			glEndList();
		}

		if (Seeker->WallTexture!=-1)
		{
			glNewList(Seeker->DisplayList+2, GL_COMPILE);
				glEnable(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, Seeker->WallTexture);

				glBegin(GL_QUADS);
					glNormal3f(0.0, 0.0, 1.0);
					glTexCoord2f(0.0, 0.0);
					glVertex3f(-Offset, Floor, Offset);
					glTexCoord2f(1.0, 0.0);
					glVertex3f(Offset, Floor, Offset);
					glTexCoord2f(1.0, 1.0);
					glVertex3f(Offset, Ceiling, Offset);
					glTexCoord2f(0.0, 1.0);
					glVertex3f(-Offset, Ceiling, Offset);
				glEnd();
			glEndList();

			glNewList(Seeker->DisplayList+1, GL_COMPILE);
				glEnable(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, Seeker->WallTexture);

				glBegin(GL_QUADS);
					glNormal3f(1.0, 0.0, 0.0);
					glTexCoord2f(0.0, 0.0);
					glVertex3f(Offset, Floor, Offset);
					glTexCoord2f(1.0, 0.0);
					glVertex3f(Offset, Floor, -Offset);
					glTexCoord2f(1.0, 1.0);
					glVertex3f(Offset, Ceiling, -Offset);
					glTexCoord2f(0.0, 1.0);
					glVertex3f(Offset, Ceiling, Offset);
				glEnd();
			glEndList();

			glNewList(Seeker->DisplayList, GL_COMPILE);
				glEnable(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, Seeker->WallTexture);

				glBegin(GL_QUADS);
					glNormal3f(0.0, 0.0, -1.0);
					glTexCoord2f(0.0, 0.0);
					glVertex3f(Offset, Floor, -Offset);
					glTexCoord2f(1.0, 0.0);
					glVertex3f(-Offset, Floor, -Offset);
					glTexCoord2f(1.0, 1.0);
					glVertex3f(-Offset, Ceiling, -Offset);
					glTexCoord2f(0.0, 1.0);
					glVertex3f(Offset, Ceiling, -Offset);
				glEnd();
			glEndList();

			glNewList(Seeker->DisplayList+3, GL_COMPILE);
				glEnable(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, Seeker->WallTexture);

				glBegin(GL_QUADS);
					glNormal3f(-1.0, 0.0, 0.0);
					glTexCoord2f(0.0, 0.0);
					glVertex3f(-Offset, Floor, -Offset);
					glTexCoord2f(1.0, 0.0);
					glVertex3f(-Offset, Floor, Offset);
					glTexCoord2f(1.0, 1.0);
					glVertex3f(-Offset, Ceiling, Offset);
					glTexCoord2f(0.0, 1.0);
					glVertex3f(-Offset, Ceiling, -Offset);
				glEnd();
			glEndList();
		}

		Seeker=Seeker->Next;
	}

	return true;
}

bool Dantares::Draw(int Distance, bool MovePlayer)
{
	if (CurrentMap==-1)												//No active map.
	{
		return false;
	}

	int x;
	int y;
	int XBound=Maps[CurrentMap]->XSize;
	int YBound=Maps[CurrentMap]->YSize;
	int HalfDistance=Distance/2;

	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_TEXTURE_2D);

	glTranslatef(0.0, 0.0, -(SqSize/2.0));
	glRotatef(CameraFacing*90+TurnOffset, 0.0, 1.0, 0.0);

	switch (CameraFacing)
	{
		case 0:
		case 2:
			if (Walking==1 || Walking==3)
			{
				glTranslatef(-(CameraX*SqSize+WalkOffset), 0.0, CameraY*SqSize);
			}
			else
			{
				glTranslatef(-(CameraX*SqSize), 0.0, CameraY*SqSize+WalkOffset);
			}

			break;
		case 1:
		case 3:
			if (Walking==0 || Walking==2)
			{
				glTranslatef(-(CameraX*SqSize), 0.0, CameraY*SqSize+WalkOffset);
			}
			else
			{
				glTranslatef(-(CameraX*SqSize+WalkOffset), 0.0, CameraY*SqSize);
			}

			break;
	}

	switch (CameraFacing)
	{
		case 0:
			for (x=CameraX>Distance?CameraX-Distance:0; x<XBound && x<CameraX+Distance; x++)
			{
				glPushMatrix();
				glTranslatef(float(x*SqSize), 0.0, SqSize);

				for (y=CameraY>HalfDistance?CameraY-HalfDistance:0; y<YBound && y<CameraY+Distance; y++)
				{
					if (y==CameraY-HalfDistance)
					{
						glTranslatef(0.0, 0.0, -(float(SqSize*y)));
					}

					glTranslatef(0.0, 0.0, -SqSize);

					SpaceClass *Seeker=Maps[CurrentMap]->FindSpace(Maps[CurrentMap]->MapArray[x][y]);

					if (Seeker->WallTexture!=-1)
					{
						if (y>=CameraY)
						{
							glCallList(Seeker->DisplayList+2);
						}

						if (x<CameraX)
						{
							glCallList(Seeker->DisplayList+1);
						}
						else if (x>CameraX)
						{
							glCallList(Seeker->DisplayList+3);
						}
					}

					if (Seeker->FloorTexture!=-1)
					{
						glCallList(Seeker->DisplayList+4);
					}

					if (Seeker->CeilingTexture!=-1)
					{
						glCallList(Seeker->DisplayList+5);
					}
				}

				glPopMatrix();
			}

			break;
		case 1:
			for (x=CameraX>HalfDistance?CameraX-HalfDistance:0; x<XBound && x<CameraX+Distance; x++)
			{
				glPushMatrix();
				glTranslatef(float(x*SqSize), 0.0, SqSize);

				for (y=CameraY>Distance?CameraY-Distance:0; y<YBound && y<CameraY+Distance; y++)
				{
					if (y==CameraY-Distance)
					{
						glTranslatef(0.0, 0.0, -(float(SqSize*y)));
					}

					glTranslatef(0.0, 0.0, -SqSize);

					SpaceClass *Seeker=Maps[CurrentMap]->FindSpace(Maps[CurrentMap]->MapArray[x][y]);

					if (Seeker->WallTexture!=-1)
					{
						if (x>=CameraX)
						{
							glCallList(Seeker->DisplayList+3);
						}

						if (y>CameraY)
						{
							glCallList(Seeker->DisplayList+2);
						}
						else if (y<CameraY)
						{
							glCallList(Seeker->DisplayList);
						}
					}

					if (Seeker->FloorTexture!=-1)
					{
						glCallList(Seeker->DisplayList+4);
					}

					if (Seeker->CeilingTexture!=-1)
					{
						glCallList(Seeker->DisplayList+5);
					}
				}

				glPopMatrix();
			}

			break;
		case 2:
			for (x=CameraX>Distance?CameraX-Distance:0; x<XBound && x<CameraX+Distance; x++)
			{
				glPushMatrix();
				glTranslatef(float(x*SqSize), 0.0, SqSize);

				for (y=CameraY>Distance?CameraY-Distance:0; y<YBound && y<CameraY+HalfDistance; y++)
				{
					if (y==CameraY-Distance)
					{
						glTranslatef(0.0, 0.0, -(float(SqSize*y)));
					}

					glTranslatef(0.0, 0.0, -SqSize);

					SpaceClass *Seeker=Maps[CurrentMap]->FindSpace(Maps[CurrentMap]->MapArray[x][y]);

					if (Seeker->WallTexture!=-1)
					{
						if (y<=CameraY)
						{
							glCallList(Seeker->DisplayList+0);
						}

						if (x>CameraX)
						{
							glCallList(Seeker->DisplayList+3);
						}
						else if (x<CameraX)
						{
							glCallList(Seeker->DisplayList+1);
						}
					}

					if (Seeker->FloorTexture!=-1)
					{
						glCallList(Seeker->DisplayList+4);
					}

					if (Seeker->CeilingTexture!=-1)
					{
						glCallList(Seeker->DisplayList+5);
					}
				}

				glPopMatrix();
			}

			break;
		case 3:
			for (x=CameraX>Distance?CameraX-Distance:0; x<XBound && x<CameraX+HalfDistance; x++)
			{
				glPushMatrix();
				glTranslatef(float(x*SqSize), 0.0, SqSize);

				for (y=CameraY>Distance?CameraY-Distance:0; y<YBound && y<CameraY+Distance; y++)
				{
					if (y==CameraY-Distance)
					{
						glTranslatef(0.0, 0.0, -(float(SqSize*y)));
					}

					glTranslatef(0.0, 0.0, -SqSize);

					SpaceClass *Seeker=Maps[CurrentMap]->FindSpace(Maps[CurrentMap]->MapArray[x][y]);

					if (Seeker->WallTexture!=-1)
					{
						if (x<=CameraX)
						{
							glCallList(Seeker->DisplayList+1);
						}

						if (y<CameraY)
						{
							glCallList(Seeker->DisplayList);
						}
						else if (y>CameraY)
						{
							glCallList(Seeker->DisplayList+2);
						}
					}

					if (Seeker->FloorTexture!=-1)
					{
						glCallList(Seeker->DisplayList+4);
					}

					if (Seeker->CeilingTexture!=-1)
					{
						glCallList(Seeker->DisplayList+5);
					}
				}

				glPopMatrix();
			}

			break;
	}

	if (MovePlayer)
	{
		if (WalkOffset>=SqSize)
		{
			WalkOffset=0;

			if (Walking==0)
			{
				CameraY++;
			}
			else
			{
				CameraX++;
			}

			Walking=-1;
		}
		else if (WalkOffset<=-SqSize)
		{
			WalkOffset=0;

			if (Walking==2)
			{
				CameraY--;
			}
			else
			{
				CameraX--;
			}

			Walking=-1;
		}
		else if (WalkOffset>0.0)
		{
			WalkOffset=WalkOffset+WalkSpeed;

			if (WalkOffset>SqSize)
			{
				WalkOffset=SqSize;
			}
		}
		else if (WalkOffset<0.0)
		{
			WalkOffset=WalkOffset-WalkSpeed;

			if (WalkOffset<-SqSize)
			{
				WalkOffset=-SqSize;
			}
		}

		if (Turning>0)
		{
			TurnOffset=TurnOffset+TurnSpeed;
			DegreesTurned=DegreesTurned+TurnSpeed;
		}
		else if (Turning<0)
		{
			TurnOffset=TurnOffset-TurnSpeed;
			DegreesTurned=DegreesTurned+TurnSpeed;
		}

		if (TurnOffset>45.0 && Turning==1)
		{
			Turning=2;
			CameraFacing=(CameraFacing+1)%4;
			TurnOffset=-90.0+TurnOffset;
		}

		if (TurnOffset<-45.0 && Turning==-1)
		{
			Turning=-2;

			if (--CameraFacing==-1)
			{
				CameraFacing=3;
			}

			TurnOffset=90.0+TurnOffset;
		}

		if (DegreesTurned>=90.0)
		{
			Turning=0;
			TurnOffset=0.0;
			DegreesTurned=0.0;
		}
	}

	glPopAttrib();

	return true;
}

bool Dantares::StepForward(bool Force)
{
	if (Walking>-1 || Turning!=0 || CurrentMap==-1)
	{
		return false;
	}

	switch (CameraFacing)
	{
		case 0:
			if (CameraY+1<Maps[CurrentMap]->YSize &&
				(Maps[CurrentMap]->WalkArray[CameraX][CameraY+1] || Force))
			{
				Walking=0;
				WalkOffset=WalkOffset+WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 1:
			if (CameraX+1<Maps[CurrentMap]->XSize &&
				(Maps[CurrentMap]->WalkArray[CameraX+1][CameraY] || Force))
			{
				Walking=1;
				WalkOffset=WalkOffset+WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 2:
			if (CameraY>0 &&
				(Maps[CurrentMap]->WalkArray[CameraX][CameraY-1] || Force))
			{
				Walking=2;
				WalkOffset=WalkOffset-WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 3:
			if (CameraX>0 &&
				(Maps[CurrentMap]->WalkArray[CameraX-1][CameraY] || Force))
			{
				Walking=3;
				WalkOffset=WalkOffset-WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
	}

	return false;
}

bool Dantares::StepBackward(bool Force)
{
	if (Walking>-1 || Turning!=0 || CurrentMap==-1)
	{
		return false;
	}

	switch (CameraFacing)
	{
		case 0:
			if (CameraY>0 &&
				(Maps[CurrentMap]->WalkArray[CameraX][CameraY-1] || Force))
			{
				Walking=2;
				WalkOffset=WalkOffset-WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 1:
			if (CameraX>0 &&
				(Maps[CurrentMap]->WalkArray[CameraX-1][CameraY] || Force))
			{
				Walking=3;
				WalkOffset=WalkOffset-WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 2:
			if (CameraY+1<Maps[CurrentMap]->YSize &&
				(Maps[CurrentMap]->WalkArray[CameraX][CameraY+1] || Force))
			{
				Walking=0;
				WalkOffset=WalkOffset+WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 3:
			if (CameraX+1<Maps[CurrentMap]->XSize &&
				(Maps[CurrentMap]->WalkArray[CameraX+1][CameraY] || Force))
			{
				Walking=1;
				WalkOffset=WalkOffset+WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
	}

	return false;
}

bool Dantares::StepLeft(bool Force)
{
	if (Walking>-1 || Turning!=0 || CurrentMap==-1)
	{
		return false;
	}

	switch (CameraFacing)
	{
		case 0:
			if (CameraX>0 &&
				(Maps[CurrentMap]->WalkArray[CameraX-1][CameraY] || Force))
			{
				Walking=3;
				WalkOffset=WalkOffset-WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 1:
			if (CameraY+1<Maps[CurrentMap]->YSize &&
				(Maps[CurrentMap]->WalkArray[CameraX][CameraY+1] || Force))
			{
				Walking=0;
				WalkOffset=WalkOffset+WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 2:
			if (CameraX+1<Maps[CurrentMap]->XSize &&
				(Maps[CurrentMap]->WalkArray[CameraX+1][CameraY] || Force))
			{
				Walking=1;
				WalkOffset=WalkOffset+WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 3:
			if (CameraY>0 &&
				(Maps[CurrentMap]->WalkArray[CameraX][CameraY-1] || Force))
			{
				Walking=2;
				WalkOffset=WalkOffset-WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
	}

	return false;
}

bool Dantares::StepRight(bool Force)
{
	if (Walking>-1 || Turning!=0 || CurrentMap==-1)
	{
		return false;
	}

	switch (CameraFacing)
	{
		case 0:
			if (CameraX+1<Maps[CurrentMap]->XSize &&
				(Maps[CurrentMap]->WalkArray[CameraX+1][CameraY] || Force))
			{
				Walking=1;
				WalkOffset=WalkOffset+WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 1:
			if (CameraY>0 &&
				(Maps[CurrentMap]->WalkArray[CameraX][CameraY-1] || Force))
			{
				Walking=2;
				WalkOffset=WalkOffset-WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 2:
			if (CameraX>0 &&
				(Maps[CurrentMap]->WalkArray[CameraX-1][CameraY] || Force))
			{
				Walking=3;
				WalkOffset=WalkOffset-WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
		case 3:
			if (CameraY+1<Maps[CurrentMap]->YSize &&
				(Maps[CurrentMap]->WalkArray[CameraX][CameraY+1] || Force))
			{
				Walking=0;
				WalkOffset=WalkOffset+WalkSpeed;

				return true;
			}
			else
			{
				return false;
			}

			break;
	}

	return false;
}

bool Dantares::TurnLeft()
{
	if (Walking>-1 || Turning!=0 || CurrentMap==-1)
	{
		return false;
	}

	Turning=-1;
	TurnOffset=TurnOffset-TurnSpeed;

	return true;
}

bool Dantares::TurnRight()
{
	if (Walking>-1 || Turning!=0 || CurrentMap==-1)
	{
		return false;
	}

	Turning=1;
	TurnOffset=TurnOffset+TurnSpeed;

	return true;
}

bool Dantares::SetWalkingSpeed(float WSpeed)
{
	if (WSpeed==0.0)
	{
		WalkSpeed=SqSize/15.0;

		return true;
	}

	WalkSpeed=SqSize/WSpeed;

	return true;
}

bool Dantares::SetTurningSpeed(float TSpeed)
{
	if (TSpeed==0.0)
	{
		TurnSpeed=5.0;

		return true;
	}

	if (TSpeed<0.0 || TSpeed>90.0)
	{
		return false;
	}

	TurnSpeed=TSpeed;

	return true;
}

int Dantares::IsWalking() const
{
	return Walking;
}

float Dantares::GetWalkOffset() const
{
	return WalkOffset;
}

int Dantares::IsTurning() const
{
	if (Turning<0)
	{
		return -1;
	}

	if (Turning>0)
	{
		return 1;
	}

	return 0;
}

float Dantares::GetTurnOffset() const
{
	return TurnOffset;
}

int Dantares::GetPlayerX() const
{
	if (CurrentMap==-1)
	{
		return -1;
	}

	return CameraX;
}

int Dantares::GetPlayerY() const
{
	if (CurrentMap==-1)
	{
		return -1;
	}

	return CameraY;
}

int Dantares::GetPlayerFacing() const
{
	if (CurrentMap==-1)
	{
		return -1;
	}

	return CameraFacing;
}

int Dantares::GetCurrentSpace() const
{
	if (CurrentMap==-1)
	{
		return -1;
	}

	return Maps[CurrentMap]->MapArray[CameraX][CameraY];
}

int Dantares::GetSpace(int XCoord, int YCoord) const
{
	if (CurrentMap == -1 || XCoord < 0 || YCoord < 0 ||
		XCoord >= Maps[CurrentMap]->XSize ||
		YCoord >= Maps[CurrentMap]->YSize)
	{
		return -1;
	}

	return Maps[CurrentMap]->MapArray[XCoord][YCoord];
}

bool Dantares::SpaceIsWalkable(int XCoord, int YCoord) const
{
	if (CurrentMap == -1 || XCoord < 0 || YCoord < 0 ||
		XCoord >= Maps[CurrentMap]->XSize ||
		YCoord >= Maps[CurrentMap]->YSize)
	{
		return false;
	}

	return Maps[CurrentMap]->WalkArray[XCoord][YCoord];
}

Dantares::SpaceClass::SpaceClass(int Type)
{
	SpaceType=Type;
	FloorTexture=-1;
	CeilingTexture=-1;
	WallTexture=-1;
	DisplayList=glGenLists(6);
	Next=NULL;
}

Dantares::SpaceClass::SpaceClass(const SpaceClass &Copy)
{
	SpaceType = Copy.SpaceType;
	FloorTexture = Copy.FloorTexture;
	CeilingTexture = Copy.CeilingTexture;
	WallTexture = Copy.WallTexture;
	DisplayList = Copy.DisplayList;

	if (Copy.Next != NULL)
	{
		Next = new SpaceClass(*Copy.Next);
	}
	else
	{
		Next = NULL;
	}
}

Dantares::SpaceClass::~SpaceClass()
{
	delete Next;
}

Dantares::MapClass::MapClass(int MaxX, int MaxY)
{
	XSize=MaxX;
	YSize=MaxY;

	SpaceInfo=new SpaceClass(0);

	MapArray=new int*[MaxX];
	WalkArray=new bool*[MaxX];

	for (int x=0; x<MaxX; x++)
	{
		MapArray[x]=new int[MaxY];
		WalkArray[x]=new bool[MaxY];
	}
}

Dantares::MapClass::MapClass(const MapClass &Copy)
{
	XSize = Copy.XSize;
	YSize = Copy.YSize;

	MapArray = new int*[XSize];
	WalkArray = new bool*[XSize];

	for (int x = 0; x < XSize; x++)
	{
		MapArray[x] = new int[YSize];
		WalkArray[x] = new bool[YSize];
	}

	for (int x = 0; x < XSize; x++)
	{
		for (int y = 0; y < YSize; y++)
		{
			MapArray[x][y] = Copy.MapArray[x][y];
			WalkArray[x][y] = Copy.WalkArray[x][y];
		}
	}

	SpaceInfo = new SpaceClass(*Copy.SpaceInfo);
}

Dantares::MapClass::~MapClass()
{
	for (int x=0; x<XSize; x++)
	{
		delete [] MapArray[x];
		delete [] WalkArray[x];
	}

	delete [] MapArray;
	delete [] WalkArray;

	delete SpaceInfo;
}

bool Dantares::MapClass::SpaceDefined(int Space)
{
	SpaceClass *Seeker=SpaceInfo;

	while (Seeker!=NULL)
	{
		if (Seeker->SpaceType==Space)
		{
			return true;
		}

		Seeker=Seeker->Next;
	}

	return false;
}

void Dantares::MapClass::AddSpace(int Space)
{
	SpaceClass *Seeker=SpaceInfo;

	while (Seeker->Next!=NULL)
	{
		Seeker=Seeker->Next;
	}

	Seeker->Next=new SpaceClass(Space);
}

Dantares::SpaceClass *Dantares::MapClass::FindSpace(int Space)
{
	SpaceClass *Seeker=SpaceInfo;

	while (Seeker!=NULL)
	{
		if (Seeker->SpaceType==Space)
		{
			break;
		}

		Seeker=Seeker->Next;
	}

	return Seeker;
}
