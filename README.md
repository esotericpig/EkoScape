# EkoScape

![Screenshot of EkoScape](https://github.com/user-attachments/assets/5fb2cde1-719f-437f-a980-bbbf2cdd89bb)

Work in Progress.

This is an old game I made back in 2004 (with minor edits in later years) using my friend's 3D step-based engine (grid/tile-based movement): The Dantares Engine by Ryan Witmer.

I originally made it for Windows only. In 2024, I re-wrote the code for it in SDL (tested on Linux) using modern C++17. However, it still uses The Dantares Engine and requires legacy OpenGL 2.0+.

The game runs & plays, but there is currently no Main Menu, and you need to manually edit which map to play.

The code is a bit over-engineered, but I designed it so that I could use parts of it in other projects. I was planning to make a simple ECS (Entity Component System), but because the game is quite simple and the ECS code produced more files/lines of code, I decided to just to stick with the original class-based structure.
