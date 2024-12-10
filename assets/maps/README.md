# Maps | EkoScape #

You can copy & rename [template_unix.txt](template_unix.txt) or [template_windows.txt](template_windows.txt) into a current or new sub folder to begin editing.

The [user/](user/) folder was created for you. The [fanmade/](fanmade/) folder was created for single, community-submitted Map files.

## Submitting Your Map Files ##

You can submit your Map files [here on GitHub Discussions](https://github.com/esotericpig/EkoScape/discussions/1) or as an [Issue](https://github.com/esotericpig/EkoScape/issues) for a chance to have them included in the next version! Or to simply share them for others to try.

Alternatively, you can just copy & paste them into a comment on [itch.io](https://esotericpig.itch.io/ekoscape) using a Markdown code block (backticks).

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
  - The 1st number, **7**, is the turning speed of the Player. A **higher** number is faster.
  - The 2nd number, **11**, is the walking speed of the Player. A **lower** number is faster.
  - You can use decimal numbers (floats) such as **7.4** or **11.11**.
- Line **7** is the "floor" space, inside of single quotes, for Things that can move/disappear, such as the Player and Robots.
  - You cannot use a Thing as the space, but you can use a Wall or a Portal instead of a Floor space.
- Line **8** is the delay, in milliseconds, of the Robots.
  - In this example, **1000** would mean that the Robots move every 1 second.
- Lines **10+** are the Grids (i.e., the floors/layers) of the Map.
  - Each Grid is separated by an empty line (line **15** in this example).
  - You can only have a **maximum of 10** Grids.
  - You **must** have at least one Player space (`^`, `v`, `>`, `<`) and one End space (`$`) in at least one of the Grids.

Line separators are platform-independent, so you can use the ones on Linux (`\n`), Windows (`\r\n`), and/or Classic Mac (`\r`).

You can place your Map files inside of any sub folder in `assets/maps/` or in a new sub folder, and it will be picked up by the game automatically.

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
| `=`                    | **[1]** Statue. Doesn't move.                                                            |
| `!`                    | **[2]** Normal. Moves normally.                                                          |
| `\|` (pipe)            | **[3]** Ghost. Moves normally, but can go through walls, like a ghost.                   |
| `?`                    | **[4]** Snake. Tough; a "snake" that leaves behind a temporary "tail."                   |
| `Q` (uppercase)        | **[5]** Worm. Very tough; a "snake" (`?`) that can go through walls, like a ghost snake. |

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
