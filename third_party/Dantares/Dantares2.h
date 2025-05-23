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

#ifndef DANTARES2_H
#define DANTARES2_H

#include<iostream>
#include<memory>
#include<unordered_map>
#include<vector>

class Dantares2
{
public:
    using GLuint = unsigned int;

    class RendererClass
    {
    public:
        struct Vector3f
        {
            float X{}, Y{}, Z{};
        };

        struct QuadListData
        {
            GLuint TextureID = 0;
            Vector3f Normal{};
            Vector3f Vertices[4] = {};
        };

        virtual ~RendererClass() noexcept = default;

        virtual void BeginDraw() = 0;
        virtual void EndDraw() = 0;

        virtual void TranslateModelMatrix(float X, float Y, float Z) = 0;
        virtual void RotateModelMatrix(float Angle, float X, float Y, float Z) = 0;
        virtual void UpdateModelMatrix() = 0;
        virtual void PushModelMatrix() = 0;
        virtual void PopModelMatrix() = 0;

        virtual GLuint GenerateQuadLists(int Count) = 0;
        virtual void DeleteQuadLists(GLuint ID, int Count) = 0;
        virtual void CompileQuadList(GLuint ID, int Index, const QuadListData &Data) = 0;
        virtual void DrawQuadList(GLuint ID, int Index) = 0;
    };

    static constexpr int MAXMAPS = 10;
    //The maximum number of maps the engine will store.
    //Adjust this to fit your preferences.

    static constexpr float TARGET_DELTA_TIME = 1.0f / 60.0f;
    //The delta time (1 second / FPS) in seconds that the original Engine targeted.
    //It's used to adjust the received value in UpdateDeltaTime() so that the
    //original code that used this Engine doesn't have to update its speed values.
    //This cannot be 0, else it will cause a divide by 0.

    static constexpr int DIR_NORTH = 0;
    static constexpr int DIR_EAST  = 1;
    static constexpr int DIR_SOUTH = 2;
    static constexpr int DIR_WEST  = 3;

    explicit Dantares2(RendererClass &Renderer, float SquareSize, float FloorHeight, float CeilingHeight);
    /*  Constructor takes four parameters to initialize the engine.

        Parameters:
        RendererClass &Renderer - The renderer implementation to use for rendering.
        float SquareSize - The size of each map square in OpenGL coordinates.
        float FloorHeight - The position of the floor on the Y axis.
        float CeilingHeight - The position of the ceiling on the Y axis.
    */

    Dantares2(const Dantares2 &Copy) = delete;
    /*  Copy constructor.
    */

    Dantares2(Dantares2 &&Other) noexcept;
    /*  Move constructor.
    */

    Dantares2 &operator = (const Dantares2 &Copy) = delete;
    /*  = operator to handle transfers.
    */

    Dantares2 &operator = (Dantares2 &&Other) noexcept;
    /*  = operator to handle move transfers.
    */

    virtual ~Dantares2() noexcept = default;
    /*  Destructor cleans up all map and texture information.
    */

    int AddMap(const void *Map, int SizeX, int SizeY);
    //See: https://isocpp.org/wiki/faq/const-correctness#constptrptr-conversion
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

    bool SetWallTexture(int SpaceID, GLuint TextureID, bool Delete=false);
    /*  Applies a texture to a space number.

        Parameters:
        int SpaceID - Specifies the map symbol that the texture will apply to.  This
                      number corresponds to the numbers in the map array.  If a texture is
                      already assigned to that space type, it will be replaced by the new
                      texture.
        GLuint TextureID - The texture ID of the 2D OpenGL texture that will be applied to the
                           space type.
        bool Delete=false - Delete is false by default, and need not be passed.  If Delete is
                            passed as true, the texture assignment currently held by SpaceID
                            will be removed, and TextureID will be disregarded.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetFloorTexture(int SpaceID, GLuint TextureID, bool Delete=false);
    /*  Applies a floor texture to a space number.

        Parameters:
        int SpaceID - Specifies the map symbol that the texture will apply to.  This
                      number corresponds to the numbers in the map array.  If a texture is
                      already assigned to that space type, it will be replaced by the new
                      texture.
        GLuint TextureID - The texture ID of the 2D OpenGL texture that will be applied to the
                           space type.
        bool Delete=false - Delete is false by default, and need not be passed.  If Delete is
                            passed as true, the texture assignment currently held by SpaceID
                            will be removed, and TextureID will be disregarded.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetCeilingTexture(int SpaceID, GLuint TextureID, bool Delete=false);
    /*  Applies a ceiling texture to a space number.

        Parameters:
        int SpaceID - Specifies the map symbol that the texture will apply to.  This
                      number corresponds to the numbers in the map array.  If a texture is
                      already assigned to that space type, it will be replaced by the new
                      texture.
        GLuint TextureID - The texture ID of the 2D OpenGL texture that will be applied to the
                           space type.
        bool Delete=false - Delete is false by default, and need not be passed.  If Delete is
                            passed as true, the texture assignment currently held by SpaceID
                            will be removed, and TextureID will be disregarded.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetMasterFloorTexture(GLuint TextureID, bool Delete=false);
    /*  Applies a texture to the floor of all empty (0) spaces on the map.

        Parameters:
        GLuint TextureID - The texture ID of the 2D OpenGL texture that will be applied to
                           the floor.
        bool Delete=false - Delete is false by default, and need not be passed.  If Delete is
                            passed as true, the texture assigned for the floor will be removed,
                            and TextureID will be disregarded.

        Returns - Function returns true if successful, and false otherwise.
    */

    bool SetMasterCeilingTexture(GLuint TextureID, bool Delete=false);
    /*  Applies a texture to the ceiling of all empty (0) spaces on the map.

        Parameters:
        GLuint TextureID - The texture ID of the 2D OpenGL texture that will be applied to
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

    void UpdateDeltaTime(float DT);
    /*  Updates the internal delta time in seconds for speed calculations.
        This value is also adjusted by TARGET_DELTA_TIME.

        If you cap the FPS automatically with some type of sleep function, then you can
        safely ignore this function, but the speed might be bad at lower FPS.

        If you decide to use this function, it must be called at least once before
        calling Draw(..., true), MovePlayer(), and the movement functions.

        Parameters:
        float DT - The new delta time in seconds to use.
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

    bool MovePlayer();
    /*  Increments the player's movement. Unnecessary to call if Draw() was called with
        the MovePlayer param set to true.

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

    bool IsWalking() const;
    /*  Determines if the player is walking or not.

        Returns - Function returns true if the player is walking, and false otherwise.
    */

    int GetWalkDirection() const;
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

    bool IsTurning() const;
    /*  Determines if the player is turning or not.

        Returns - Function returns true if the player is turning, and false otherwise.
    */

    int GetTurnDirection() const;
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

protected:
    //Class for describing spaces.
    class SpaceClass
    {
    public:
        static constexpr int FACE_COUNT      = 6;
        static constexpr int FACE_WALL_FAR   = 0;                        //North/Back.
        static constexpr int FACE_WALL_RIGHT = 1;                        //East.
        static constexpr int FACE_WALL_NEAR  = 2;                        //South/Front.
        static constexpr int FACE_WALL_LEFT  = 3;                        //West.
        static constexpr int FACE_FLOOR      = 4;
        static constexpr int FACE_CEILING    = 5;

        explicit SpaceClass(RendererClass *Renderer, int Type) noexcept;

        SpaceClass(const SpaceClass &Copy) = delete;
        SpaceClass(SpaceClass &&Other) noexcept;
        SpaceClass &operator = (const SpaceClass &Copy) = delete;
        SpaceClass &operator = (SpaceClass &&Other) noexcept;
        virtual ~SpaceClass() noexcept;

        void GenerateQuadLists();

        void PrintDebugInfo(std::ostream &Out = std::cout, int Indent = 0) const;

        RendererClass *Renderer = nullptr;
        int SpaceType = 0;                                               //The type of the space.
        GLuint FloorTexture = 0;                                         //Floor texture ID.
        GLuint CeilingTexture = 0;                                       //Ceiling texture ID.
        GLuint WallTexture = 0;                                          //Wall texture ID.
        GLuint QuadList = 0;                                             //Quad list for the space.

    private:
        void MoveFrom(SpaceClass &&Other) noexcept;
        void DeleteQuadLists() noexcept;
    };

    //Class for storing maps.
    class MapClass
    {
    public:
        explicit MapClass(RendererClass *Renderer, int MaxX, int MaxY);  //Constructor sets map size.

        MapClass(const MapClass &Copy) = delete;
        MapClass(MapClass &&Other) noexcept;
        MapClass &operator = (const MapClass &Copy) = delete;
        MapClass &operator = (MapClass &&Other) noexcept;
        virtual ~MapClass() noexcept = default;

        SpaceClass &AddSpaceIfAbsent(int SpaceID);
        SpaceClass *FindSpace(int XCoord, int YCoord);
        SpaceClass *FindSpace(int SpaceID);

        void ChangeSquare(int XCoord, int YCoord, int NewType);
        void ChangeWalkability(int XCoord, int YCoord, bool Walkable);

        int GetSpaceType(int XCoord, int YCoord) const;
        bool SpaceIsWalkable(int XCoord, int YCoord) const;

        void PrintDebugInfo(std::ostream &Out = std::cout, int Indent = 0) const;

        RendererClass *Renderer = nullptr;
        //Map of space information.
        std::unordered_map<int, std::unique_ptr<SpaceClass>> SpaceInfo{};
        int XSize = 0;                                                   //Map width.
        int YSize = 0;                                                   //Map height.

    protected:
        std::vector<std::vector<int>> MapArray{};                        //Array for the map.
        std::vector<std::vector<bool>> WalkArray{};                      //Array for walkability.

    private:
        void MoveFrom(MapClass &&Other) noexcept;
    };

    RendererClass *Renderer = nullptr;
    //The renderer implementation to use for rendering.
    int CurrentMap = -1;
    //The ID number of the currently active map.
    int NextMapID = 0;
    //The next map ID number to be assigned.
    float SqSize = 0.125f;
    //Size of the map squares.
    float Floor = -0.04f;
    //Floor height.
    float Ceiling = 0.04f;
    //Ceiling height.
    int CameraX = 0;
    //X coordinate of the camera.
    int CameraY = 0;
    //Y coordinate of the camera.
    int CameraFacing = DIR_NORTH;
    //Direction the camera is facing.
    int Walking = -1;
    //Flag that indicates whether or not the player is moving, and which direction.
    int Turning = 0;
    //Flag that indicates whether or not the player is turning, and which direction.
    float DeltaTime = 1.0f;
    //The delta time in seconds for speed calculations that has been adjusted by TARGET_DELTA_TIME.
    float WalkSpeed{};
    //Walking distance covered per frame.
    float TurnSpeed{};
    //Degrees turned per frame.
    float WalkOffset = 0.0f;
    //Distance between squares when walking.
    float TurnOffset = 0.0f;
    //Degrees between directions when turning.
    float DegreesTurned = 0.0f;
    //Tracking variable for surface hiding while turning.
    std::unique_ptr<MapClass> Maps[MAXMAPS]{};
    //Pointers to the stored maps.

private:
    void MoveFrom(Dantares2 &&Other) noexcept;
};

#endif
