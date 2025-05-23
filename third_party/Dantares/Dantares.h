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

/*  The Dantares Engine v1.3
    All code written by Ryan Witmer in Apple Xcode
    ryan@averagesoftware.com
    www.averagesoftware.com
    www.averagesoftware.com/dantares.html
    https://phasercat.com

    Changelog
    - v1.3
        - Fixed TurnLeft() and TurnRight() to also update the DegreesTurned so that the MovePlayer
          logic in Draw() doesn't go over 90 degrees, which caused a jerky drawing animation
          when TurnSpeed wasn't a multiple of 10.
        - Applied IDE suggestions.
    - v1.2
        - Changed SetPlayerPosition(int, int) to reuse the current CameraFacing.
    - v1.1
        - Changed MAXMAPS to be inside Dantares.
        - Changed to use smart pointers and vector.
        - Added DANTARES_* platform macros for the includes.
        - Added move constructors and deleted copy constructors (Rule of 5).
            - Cannot copy OpenGL display lists.
        - Added destructor to SpaceClass to auto-delete OpenGL display lists.
        - Added PrintDebugInfo().
        - Fixed AddMap(const int**) to be AddMap(const int* const*).
            - https://isocpp.org/wiki/faq/const-correctness#constptrptr-conversion
*/

#ifndef DANTARES_H
#define DANTARES_H

#if defined(DANTARES_PLATFORM_MACOS)
    #ifndef GL_SILENCE_DEPRECATION
    #define GL_SILENCE_DEPRECATION
    #endif

    //Mac OS X OpenGL headers.
    //#include<OpenGL/OpenGL.h>
    #include<OpenGL/gl.h>
    //#include<OpenGL/glu.h>
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
    //#include<GL/glu.h>
#else
    //X11 OpenGL headers.
    //#include<GL/glx.h>
    #include<GL/gl.h>
    //#include<GL/glu.h>
#endif

#include<iostream>
#include<memory>
#include<vector>

/**
 * Copy constructors/assignments are not allowed because OpenGL Display Lists cannot be copied,
 * so SpaceClass cannot copy its Display Lists.
 *
 * Move constructors/assignments are allowed though.
 */
class Dantares
{
public:
    static constexpr int MAXMAPS = 10;
    //The maximum number of maps the engine will store.
    //Adjust this to fit your preferences.

    explicit Dantares(float SquareSize, float FloorHeight, float CeilingHeight);
    /*  Constructor takes three parameters to initialize the engine.

        Parameters:
        float SquareSize - The size of each map square in OpenGL coordinates.
        float FloorHeight - The position of the floor on the Y axis.
        float CeilingHeight - The position of the ceiling on the Y axis.
    */

    Dantares(const Dantares &Copy) = delete;
    /*  Copy constructor.
    */

    Dantares(Dantares &&Other) noexcept;
    /*  Move constructor.
    */

    Dantares &operator = (const Dantares &r) = delete;
    /*  = operator to handle transfers.
    */

    Dantares &operator = (Dantares &&r) noexcept;
    /*  = operator to handle move transfers.
    */

    virtual ~Dantares() noexcept = default;
    /*  Destructor cleans up all map and texture information.
    */

    int AddMap(const void *Map, int SizeX, int SizeY);
    int AddMap(const int* const *Map, int SizeX, int SizeY);
    /*  Adds a map and returns the map number.  The map must be a two-dimensional integer array.
        0s in the map are treated as empty spaces, all other numbers are solid squares.

        Parameters:
        void *Map - The 2D integer array representing the map.
        int **Map - The dynamically allocated integer array representing the map.
        int LastX - The size of the first index of the map array.
        int LastY - The size of the second index of the map array.

        Returns - Function returns the ID number of the stored map.  A return value
                  of -1 indicates failure.

        Note: Adding a map does NOT make it active.  Be sure to make a map active using
        the SetCurrentMap function before applying textures.  You may store as many maps
        as indicated in the const int MAXMAPS.  Attempting to store a map beyond
        the limit will result in failure.
    */

    bool DeleteMap(int MapID);
    /*  Deletes the map specified by the map ID number.

        Parameters:
        int MapID - The ID number of the map to be deleted.

        Returns - Function returns true if deletion was successful.  Returns false if
                  deletion fails, or the map doesn't exist.
    */

    bool IsMap(int MapID) const;
    /*  Determines if the given map ID is assigned.

        Parameters:
        int MapID - The ID number of the map to check for.

        Returns - Function returns true if the specified map ID is assigned.
                  Returns false otherwise.
    */

    bool SetWallTexture(int SpaceID, int TextureID, bool Delete=false);
    /*  Applies a texture to a space number.

        Parameters:
        int SpaceID - Specifies the map symbol that the texture will apply to.  This
                      number corresponds to the numbers in the map array.  If a texture is
                      already assigned to that space type, it will be replaced by the new
                      texture.
        int TextureID - The texture ID of the 2D OpenGL texture that will be applied to the
                        space type.
        bool Delete=false - Delete is false by default, and need not be passed.  If Delete is
                            passed as true, the texture assignment currently held by SpaceID
                            will be removed, and TextureID will be disregarded.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetFloorTexture(int SpaceID, int TextureID, bool Delete=false);
    /*  Applies a floor texture to a space number.

        Parameters:
        int SpaceID - Specifies the map symbol that the texture will apply to.  This
                      number corresponds to the numbers in the map array.  If a texture is
                      already assigned to that space type, it will be replaced by the new
                      texture.
        int TextureID - The texture ID of the 2D OpenGL texture that will be applied to the
                        space type.
        bool Delete=false - Delete is false by default, and need not be passed.  If Delete is
                            passed as true, the texture assignment currently held by SpaceID
                            will be removed, and TextureID will be disregarded.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetCeilingTexture(int SpaceID, int TextureID, bool Delete=false);
    /*  Applies a ceiling texture to a space number.

        Parameters:
        int SpaceID - Specifies the map symbol that the texture will apply to.  This
                      number corresponds to the numbers in the map array.  If a texture is
                      already assigned to that space type, it will be replaced by the new
                      texture.
        int TextureID - The texture ID of the 2D OpenGL texture that will be applied to the
                        space type.
        bool Delete=false - Delete is false by default, and need not be passed.  If Delete is
                            passed as true, the texture assignment currently held by SpaceID
                            will be removed, and TextureID will be disregarded.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetMasterFloorTexture(int TextureID, bool Delete=false);
    /*  Applies a texture to the floor of all empty (0) spaces on the map.

        Parameters:
        int TextureID - The texture ID of the 2D OpenGL texture that will be applied to
                        the floor.
        bool Delete=false - Delete is false by default, and need not be passed.  If Delete is
                            passed as true, the texture assigned for the floor will be removed,
                            and TextureID will be disregarded.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetMasterCeilingTexture(int TextureID, bool Delete=false);
    /*  Applies a texture to the ceiling of all empty (0) spaces on the map.

        Parameters:
        int TextureID - The texture ID of the 2D OpenGL texture that will be applied to
                        the ceiling.
        bool Delete=false - Delete is false by default, and need not be passed.  If Delete is
                            passed as true, the texture assigned for the ceiling will be removed,
                            and TextureID will be disregarded.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetCurrentMap(int MapID);
    /*  Sets the current map.  All calls to the Draw function and the various texture functions
        apply to the current map.

        Parameters:
        int MapID - The ID number of the map you wish to make current.

        Returns - Function returns true is successful, false if the map ID is unassigned.  If
                  the function returns false, the previously active map will remain active.
    */

    int GetCurrentMap() const;
    /*  Retrieves the ID number of the current map.

        Returns - Returns the ID number of the active map.  Returns -1 if no map is active.
    */

    bool ChangeSquare(int XCoord, int YCoord, int NewType);
    /*  Changes a square on the map to another type of square.

        Parameters:
        int XCoord - The X coordinate of the square to be changed, this corresponds to
                     the first index of the map array.
        int YCoord - The Y coordinate of the square to be changed, this corresponds to
                     the second index of the map array.
        int NewType - The type of square that the space identified by XCoord and YCoord
                      will be changed to.  0, for example, would be an empty space.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool MakeSpaceNonWalkable(int XCoord, int YCoord);
    /*  Makes the space specified by XCoord and YCoord non-walkable.  All non-0 spaces
        in the map array are non-walkable by default.

        Parameters:
        int XCoord - The X coordinate of the square to be made non-walkable.  This
                     corresponds to the first index of the map array.
        int YCoord - The Y coordinate of the square to be made non-walkable.  This
                     corresponds to the second index of the map array.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool MakeSpaceWalkable(int XCoord, int YCoord);
    /*  Makes the space specified by XCoord and YCoord walkable.  All 0 spaces in the
        map array are walkable by default.

        Parameters:
        int XCoord - The X coordinate of the square to be made walkable.  This
                     corresponds to the first index of the map array.
        int YCoord - The Y coordinate of the square to be made walkable.  This
                     corresponds to the second index of the map array.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetPlayerPosition(int XCoord, int YCoord);
    /*  Places the camera in the center of the map space specified by XCoord and
        YCoord, facing the current direction.  If the player is currently moving,
        this function will cancel the movement.

        Space (0,0) is considered to be the bottom left corner of the map.

        Parameters:
        int XCoord - The X coordinate of the space to place the camera.  This
                     corresponds to the first index of the map array.
        int YCoord - The Y coordinate of the space to place the camera.  This
                     corresponds to the second index of the map array.

        Returns - Function returns true if successful, and false otherwise.  Reasons
                  for failure may include an illegal direction, or a space outside
                  the map dimensions.  Placing the camera in a non-walkable space
                  is allowed.
    */

    bool SetPlayerPosition(int XCoord, int YCoord, int Facing);
    /*  Places the camera in the center of the map space specified by XCoord and
        YCoord, facing the direction specified by Facing.  If the player is currently
        moving, this function will cancel the movement.

        Space (0,0) is considered to be the bottom left corner of the map.

        Parameters:
        int XCoord - The X coordinate of the space to place the camera.  This
                     corresponds to the first index of the map array.
        int YCoord - The Y coordinate of the space to place the camera.  This
                     corresponds to the second index of the map array.
        int Facing=0 - The direction the camera will be facing.
                       0 represents north, facing up the Y axis.
                       1 represents east, facing up the X axis.
                       2 represents south, facing down the Y axis.
                       3 represents west, facing down the X axis.

        Returns - Function returns true if successful, and false otherwise.  Reasons
                  for failure may include an illegal direction, or a space outside
                  the map dimensions.  Placing the camera in a non-walkable space
                  is allowed.
    */

    bool GenerateMap();
    /*  Creates an OpenGL display list for the currently active map.  You must call
        this function at least once before drawing your map.  Any changes to the map's
        texture information will require a call to this function to reflect the changes.
        Try to avoid needless calls to this function.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool Draw(int Distance=10, bool MovePlayer=true);
    /*  Draws the active map.  After the function exits, the modelview matrix will retain
        the transformations made by this function.  This will allow you to insert other
        objects into the map.  Also, the state of GL_TEXTURE_2D will be preserved.  Calls
        to glColorxx() will not affect this function.  If the player is currently moving
        or turning as a result of one of the functions below, the player's movement will
        be incremented during this function.

        Parameters:
        int Distance=10 - The distance in front of the player that will be drawn every frame,
                          in terms of map squares.  The default value is 10, which means
                          that the engine will draw 10 squares in front of the
                          player.  Increasing this number will increase the drawn area,
                          but will decrease performance.  Take note of how far out map
                          squares are drawn when adding objects to the map.
        bool MovePlayer=true - Flag that indicates incrementation of the player's movement
                               during the function.  Value is true by default.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool StepForward(bool Force=false);
    /*  Moves the player forward one space.  The player will not enter spaces flagged
        as non-walkable, or spaces off the edge of the map.

        Parameters:
        bool Force=false - If this value is passed as true, the player will move even if
                           the destination square is marked as non-walkable.  The player
                           is still prevented from leaving the map.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool StepBackward(bool Force=false);
    /*  Moves the player backward one space.  The player will not enter spaces flagged
        as non-walkable, or spaces off the edge of the map.

        Parameters:
        bool Force=false - If this value is passed as true, the player will move even if
                           the destination square is marked as non-walkable.  The player
                           is still prevented from leaving the map.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool StepLeft(bool Force=false);
    /*  Sidesteps the player left one space.  The player will not enter spaces flagged
        as non-walkable, or spaces off the edge of the map.

        Parameters:
        bool Force=false - If this value is passed as true, the player will move even if
                           the destination square is marked as non-walkable.  The player
                           is still prevented from leaving the map.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool StepRight(bool Force=false);
    /*  Sidesteps the player right one space.  The player will not enter spaces flagged
        as non-walkable, or spaces off the edge of the map.

        Parameters:
        bool Force=false - If this value is passed as true, the player will move even if
                           the destination square is marked as non-walkable.  The player
                           is still prevented from leaving the map.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool TurnLeft();
    /*  Turns the player 90 degrees to the left.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool TurnRight();
    /*  Turns the player 90 degrees to the right.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetWalkingSpeed(float WSpeed);
    /*  Sets the per-frame rate of movement when walking.  Default is 1/15 the square size.

        Parameters:
        float WSpeed - The walking speed of the player is determined by dividing the square
                       size provided to the constructor by this parameter.  Thus, the larger
                       the number provided, the slower the player's walking speed.  Passing a
                       value of 0 will set walking speed to the default value.

        Returns - Function returns true if successful, and false if an illegal value is
                  passed to the function.
    */

    bool SetTurningSpeed(float TSpeed);
    /*  Sets the per-frame rate of movement when turning, in degrees.  The default is 5.0.

        Parameters:
        float TSpeed - The number of degrees the player will cover per frame when turning.
                       This value must be less than or equal to 90.0.  Passing a value of
                       0 will set turning speed to the default value.

        Returns - Function returns true is successful, and false if an illegal value is
                  passed to the function.
    */

    int IsWalking() const;
    /*  Determines if the player is walking, and in which direction.

        Returns - Function returns -1 if the player is not walking and one of the following
                  otherwise:
                  0 represents north, walking up the Y axis.
                  1 represents east, walking up the X axis.
                  2 represents south, walking down the Y axis.
                  3 represents west, walking down the X axis.
    */

    float GetWalkOffset() const;
    /*  Retrieves the offset value that determines the player's in-between square location.

        Returns - Function returns the player's offset value, which may be negative depending
                  on which direction the player is walking.  Returns 0.0 if the player is not
                  walking.
    */

    int IsTurning() const;
    /*  Determines if the player is turning, and in which direction.

        Returns - Function returns 0 if the player is not turning.  Returns -1 if the player is
                  turning left, and 1 if the player is turning right.
    */

    float GetTurnOffset() const;
    /*  Retrieves the degree offset during a turn.

        Returns - Function returns the player's turn offset, which may be negative.
                  Returns 0.0 if the player is not turning.
    */

    int GetPlayerX() const;
    /*  Retrieves the player's X position in the map array.

        Returns - Function returns the player's current X location in the map
                  array.  If the player is currently walking, the value returned
                  will correspond to the square the player is currently
                  leaving.  Returns -1 if no map is active.
    */

    int GetPlayerY() const;
    /*  Retrieves the player's Y position in the map array.

        Returns - Function returns the player's current Y location in the map
                  array.  If the player is currently walking, the value returned
                  will correspond to the square the player is currently
                  leaving.  Returns -1 if no map is active.
    */

    int GetPlayerFacing() const;
    /*  Retrieves the direction the player is currently facing.

        Returns - Function returns the player's current direction.  During turns,
                  the player's direction changed after forty-five degrees of
                  movement.  Returns -1 if no map is active.
    */

    int GetCurrentSpace() const;
    /*  Retrieves the type of the space upon which the player is standing.

        Returns - Function returns the ID number of the player's current space.
                  If the player is walking, the space ID corresponds to the space
                  that the player is leaving.  Returns -1 if no map is active.
    */

    int GetSpace(int XCoord, int YCoord) const;
    /*  Retrieves the requested space type.

        Parameters:
        int XCoord - The X coordinate of the requested space.  This corresponds to
                     the first index of the map array.

        int YCoord - The Y coordinate of the requested space.  This corresponds to
                     the second index of the map array.

        Returns - Function returns the ID number of the requested space.  Returns
                  -1 if no map is active, or if the coordinates are invalid.
    */

    bool SpaceIsWalkable(int XCoord, int YCoord) const;
    /*  Retrieves the walkability of the requested space.

        Parameters:
        int XCoord - The X coordinate of the requested space.  This corresponds to
                     the first index of the map array.

        int YCoord - The Y coordinate of the requested space.  This corresponds to
                     the second index of the map array.

        Returns - Function returns true if the requested space is walkable, and false
                  if the space is non-walkable, or if the coordinates are invalid.
    */

    void PrintDebugInfo(std::ostream &Out = std::cout) const;
    /*  For testing purposes.
    */

private:
    //Class for describing spaces.
    class SpaceClass
    {
    public:
        static constexpr GLsizei DISPLAY_LIST_RANGE = 6;

        explicit SpaceClass(int Type);
        SpaceClass(const SpaceClass &Copy) = delete;
        SpaceClass(SpaceClass &&Other) noexcept;
        SpaceClass &operator = (const SpaceClass &r) = delete;
        SpaceClass &operator = (SpaceClass &&r) noexcept;
        virtual ~SpaceClass() noexcept;
        void ResetDisplayList();
        void PrintDebugInfo(std::ostream &Out = std::cout, int Indent = 0) const;

        int SpaceType = 0;               //The type of the space.
        int FloorTexture = -1;           //Floor texture ID.
        int CeilingTexture = -1;         //Ceiling texture ID.
        int WallTexture = -1;            //Wall texture ID.
        int DisplayList = -1;            //Display list for the space.

    private:
        void MoveFrom(SpaceClass &&Other) noexcept;
        void DeleteDisplayList() noexcept;
    };

    //Class for storing maps.
    class MapClass
    {
    public:
        explicit MapClass(int MaxX, int MaxY);       //Constructor sets map size.
        MapClass(const MapClass &Copy) = delete;
        MapClass(MapClass &&Other) noexcept;
        MapClass &operator = (const MapClass &r) = delete;
        MapClass &operator = (MapClass &&r) noexcept;
        virtual ~MapClass() noexcept = default;      //Destructor for cleanup.
        bool SpaceDefined(int Space);
        void AddSpace(int Space);
        SpaceClass *FindSpace(int Space);
        void PrintDebugInfo(std::ostream &Out = std::cout, int Indent = 0) const;

        std::vector<std::vector<int>> MapArray{};    //Array for the map.
        std::vector<std::vector<bool>> WalkArray{};  //Array for walkability.
        std::vector<SpaceClass> SpaceInfo{};         //Array of space information.
        int XSize{};                                 //Map width.
        int YSize{};                                 //Map height.

    private:
        void MoveFrom(MapClass &&Other) noexcept;
    };

    void MoveFrom(Dantares &&Other) noexcept;

    int CurrentMap{};
    //The ID number of the currently active map.
    int NextMapID{};
    //The next map ID number to be assigned.
    float SqSize{};
    //Size of the map squares.
    float Floor{};
    //Floor height.
    float Ceiling{};
    //Ceiling height.
    int CameraX{};
    //X coordinate of the camera.
    int CameraY{};
    //Y coordinate of the camera.
    int CameraFacing{};
    //Direction the camera is facing.
    int Walking{};
    //Flag that indicates whether or not the player is moving, and which direction.
    int Turning{};
    //Flag that indicates whether or not the player is turning, and which direction.
    float WalkSpeed{};
    //Walking distance covered per frame.
    float TurnSpeed{};
    //Degrees turned per frame.
    float WalkOffset{};
    //Distance between squares when walking.
    float TurnOffset{};
    //Degrees between directions when turning.
    float DegreesTurned{};
    //Tracking variable for surface hiding while turning.
    std::unique_ptr<MapClass> Maps[MAXMAPS]{};
    //Pointers to the stored maps.
};

#endif
