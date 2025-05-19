# Maps | EkoScape #

You can copy & rename [template_unix.txt](template_unix.txt) or [template_windows.txt](template_windows.txt) into a current or new sub folder to begin editing.

The [user/](user/) folder was created for you. The [fanmade/](fanmade/) folder was created for single, community-submitted Map files.

For making the Map Grids, it's recommended to use one of these visual/drawing editors:
- [ASCII Map Editor](https://stmn.itch.io/ascii-map-editor) by stmn
- [ASCII Mapper](https://notimetoplay.itch.io/ascii-mapper) by No Time To Play

## Submitting Your Map Files ##

To have your maps included in the next version of EkoScape (or to simply share them for others to try), submit them by either:
- Posting a comment on the [EkoScape itch.io page](https://esotericpig.itch.io/ekoscape) (can use Markdown backticks).
- Or by replying to [this GitHub Discussion](https://github.com/esotericpig/EkoScape/discussions/1).
- Or by creating a [GitHub Issue](https://github.com/esotericpig/EkoScape/issues).

## Example Map Rundown ##

Example Map file with line numbers:

```
 1:  [EkoScape/v1]
 2:  Name of Map
 3:  Author's Name
 4:
 5:  7 11
 6:
 7:  ' '
 8:  1000
 9:
10:  ###########
11:  # >  #  ! #
12:  #    #    #
13:  # @  :  0 #
14:  ###########
15:
16:  ########
17:  # 0  $ #
18:  ########
```

Rundown:
- Line **1** is the required header to identify the file as a Map.
  - Only the header matters, so you can use any (or no) file extension.
- Lines **2** & **3** are self-explanatory.
- Line **5** is the speed of the Player.
  - The 1st number, **7**, is the **turning speed** of the Player in degrees per frame from **0.0** to **90.0** degrees.
    - A lower number is **slower**, and a higher number is **faster**:
      - `1` degree turns the Player 1 degree per frame (very slow).
      - `90` degrees turns the Player instantaneously in one frame (very fast).
    - A value of `0` uses the default turning speed.
  - The 2nd number, **11**, is the **walking speed** of the Player, expressed as the number of frames it takes to complete walking one step (at 60 FPS), based on the formula `<seconds> = <speed> / 60 FPS`.
    - A lower number is **faster**, and a higher number is **slower**:
      - `1` frame takes the Player ~0.017 seconds (1 / 60 FPS) to walk a step (instantaneous, very fast).
      - `60` frames takes the Player 1 second (60 / 60 FPS) to walk a step (very slow).
      - `120` frames takes the Player 2 seconds (120 / 60 FPS) to walk a step (super slow).
    - A value of `0` uses the default walking speed.
  - For the speeds, you can use decimal numbers (floats) such as **7.4** or **11.11**.
- Line **7** is the "floor" space, inside of single quotes, for Things that can move/disappear, such as the Player and Robots.
  - You cannot use a Thing as the space, but you can use a Wall or a Portal instead of a Floor space.
- Line **8** is the delay, in milliseconds, of the Robots.
  - In this example, **1000** would mean that the Robots move every 1 second.
- Lines **10+** are the Grids (i.e., the floors/layers) of the Map.
  - Each Grid is separated by an empty line (line **15** in this example).
  - You can only have a **maximum of 10** Grids.
  - You **must** have at least one Player space (`^`, `v`, `>`, `<`) and one End space (`$`) in at least one of the Grids.

Line separators are platform-independent, so you can use the ones on Linux (`\n`), Windows (`\r\n`), and/or Classic Mac (`\r`).

You can place your Map files inside of any sub folder in `assets/maps/` or in a new sub folder, and they will be picked up by the game automatically.

## Grid Space Types ##

| Space Type             | Description                                                                              |
|------------------------|------------------------------------------------------------------------------------------|
| **Floors**             | Player/Robot can walk on.                                                                |
| ` ` (space)            | Normal floor.                                                                            |
| `w` (lowercase)        | White marble floor.                                                                      |
| `*`                    | Outer space floor.                                                                       |
| **Walls**              | Player & some Robots cannot walk through.                                                |
| `#`                    | Normal wall.                                                                             |
| `W` (uppercase)        | White marble wall.                                                                       |
| `x` (lowercase)        | Outer space "wall" (barrier).                                                            |
| `&`                    | Fake End (`$`) that acts as a wall.                                                      |
| `_`                    | Black void (doesn't draw anything).                                                      |
| **Fake (Ghost) Walls** | Player/Robot can walk through, even though looks like a Wall.                            |
| `%`                    | Normal ghost wall.                                                                       |
| `+`                    | White marble ghost wall.                                                                 |
| **Player (Thing)**     | Starting position of the Player...                                                       |
| `^`                    | ...facing north.                                                                         |
| `v` (lowercase)        | ...facing south.                                                                         |
| `>`                    | ...facing east.                                                                          |
| `<`                    | ...facing west.                                                                          |
| **Things**             |                                                                                          |
| `@`                    | Cell of a trapped Eko to be rescued.                                                     |
| `:`                    | Fruit that allows the Player to temporarily eat Robots.                                  |
| `$`                    | End (Spaceship) of the Map that the Player touches to flee.                              |
| **Portals**            | Player/Robot touches to warp to the corresponding Portal.                                |
| `0` - `9`              | `0` warps to `0`; `1` warps to `1`; and so on. If 3+ of the same, picks randomly.        |
| **Robots (Things)**    | **[tier#]** In order of least to most difficult.                                         |
| `=`                    | **[5]** Statue. Doesn't move.                                                            |
| `!`                    | **[4]** Normal. Moves normally.                                                          |
| `\|` (pipe)            | **[3]** Ghost. Moves normally, but can go through walls, like a ghost.                   |
| `?`                    | **[2]** Snake. Tough; a "snake" that leaves behind a temporary "tail."                   |
| `Q` (uppercase)        | **[1]** Worm. Very tough; a "snake" (`?`) that can go through walls, like a ghost snake. |

### Cheatsheet by Type ###

```
Floors:         w *
Walls:        # W x & _
Ghost Walls:  % +
Player:       ^ v > <
Things:       @ : $
Portals:      0 1 2 3 4 5 6 7 8 9
Robots:       = ! | ? Q
```

### Cheatsheet by Material ###

```
Normals:        # %
Whites:       w W +
Outer space:  * x _
Ends:           $ &
```
