# EkoScape #

[![EkoShot](https://github.com/user-attachments/assets/51fa9a49-e7b5-4543-b68d-e827af763cd2)](https://esotericpig.itch.io/ekoscape)

[![itch.io](https://img.shields.io/badge/play-itch.io-fa5c5c)](https://esotericpig.itch.io/ekoscape)
[![Latest Release](https://img.shields.io/github/v/release/esotericpig/EkoScape?include_prereleases&display_name=release&color=1a7f37)](https://github.com/esotericpig/EkoScape/releases/latest)
[![Source Code](https://img.shields.io/badge/source-github-211f1f.svg)](https://github.com/esotericpig/EkoScape)
[![License](https://img.shields.io/github/license/esotericpig/EkoScape.svg)](LICENSE)

EkoScape is a simple 3D step-based game where you run through a maze rescuing your fellow alien *Ekos*, like a 3D Pac-Man, with robot enemies, fruit, and portals. The maps are simple text files that you can edit!

## Code Notes ##

EkoScape is a remake of an old game I made back in 2004 (with minor edits in later years) using my friend's 3D step-based engine: The Dantares Engine by Ryan Witmer. You can find the original code in [relics/](relics/).

Back then, I made it for Windows only. In 2024, I re-wrote the code for it in SDL2 (multi-platform) using modern C++20. However, it still uses The Dantares Engine and legacy OpenGL 2.0+, as homage to the original game and engine.

In the beginning, I rewrote the original code for fun in a couple of days. Having enjoyed the process, I decided to flesh it out into multiple, generic files, while adding a menu and a lot of extra stuff (such as portals, fruit, and multiple grids), which took over a month.

The code is a bit over-engineered, but I designed it so that I could use parts of it in other projects. I did make a simple ECS (Entity-Component-System) for it, but because the game is quite simple and the ECS code produced more files/lines of code, I decided to just stick with the original class-based structure.

Initially, `src/cybel` was named `src/core`, but I decided to make it into its own Game Engine (kind of). I then put it in its own namespace, called `cybel`. Because of this, I simply use `using namespace cybel` inside of the `ekoscape` namespace, as I didn't like putting `cybel::` everywhere.

## Playing ##

**System Requirements:**
- Linux x86_64 (AppImage)
  - glibc 2.14+ (Ubuntu 22.04+; Fedora 16+)
  - Might need [FUSE](https://github.com/AppImage/AppImageKit/wiki/FUSE) for running the AppImage -- usually already installed.
- macOS arm64 or x86_64
  - Because this game is free and signing costs money, it's currently unsigned, so you'll need to manually override the security warning when running:
    - https://support.apple.com/guide/mac-help/open-a-mac-app-from-an-unknown-developer-mh40616/mac
- Windows x64

You can download the game on [itch.io](https://esotericpig.itch.io/ekoscape) or from the [latest release on GitHub](https://github.com/esotericpig/EkoScape/releases/latest). You can also install the game from the official [itch app](https://itch.io/app) -- the game has been tested to install & run from it, even in sandbox mode.

Then simply run it in the same folder containing the `assets` folder.

You can edit the Map files in [assets/maps/](assets/maps/) or make your own! See [assets/maps/README.md](assets/maps/README.md) for more details, which also includes how to submit your Map files for the next version.

**Additional System Notes:**
- Linux:
  - A Desktop Entry file is provided if you wish to integrate it with your DE (Desktop Environment). Open the provided file -- `com.github.esotericpig.EkoScape.desktop` -- in a text editor to read more details or [read online here](https://github.com/esotericpig/EkoScape/blob/main/res/com.github.esotericpig.EkoScape.desktop).

## Contents ##

- [Hacking](#hacking)
  - [IDEs](#ides)
  - [Configuring Build](#configuring-build)
  - [Building](#building)
  - [Running](#running)
  - [Checking Code Quality](#checking-code-quality)
  - [Building Linux AppImage](#building-linux-appimage)
  - [Packaging Up](#packaging-up)
  - [Miscellaneous](#miscellaneous)
  - [New Release](#new-release)
- [Credits](#credits)
- [License](#license)

## Hacking ##

This project uses **CMake**, [Ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages), and [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started). Please install these for your system.

See the [GitHub Workflows](.github/workflows/) or the top of [CMakeLists.txt](CMakeLists.txt) for quickly getting started.

If you have Ruby installed, there is also a script -- [scripts/dev.rb](scripts/dev.rb) -- for rapid development.

### IDEs ###

Optional stuff for IDEs.

- **CLion**
  - CMake Profiles are included. You'll need to edit them (Settings => Build, Execution, Deployment => CMake) to change the env var appropriately: `VCPKG_ROOT=/path/to/vcpkg`

### Configuring Build ###

Platform-specific notes:
- On **Linux**, OpenGL/GLEW/GLU are only façades in `vcpkg`, so you need to install them manually. Also, you might need additional system dependencies if it builds SDL2 from source.
  - On Ubuntu, you can simply look at the [Linux GH Workflow](.github/workflows/linux.yml).
  - https://github.com/libsdl-org/SDL/blob/SDL2/docs/README-linux.md
  - https://github.com/AppImage/AppImageKit/wiki/FUSE
- On **macOS**, you need to install GLEW manually:
  - `brew install glew`

The following command will be very slow the first time you run it, as `vcpkg` downloads the dependencies. Also, it downloads & extracts additional [Assets](https://github.com/esotericpig/EkoScape/releases/tag/v1.99) to the `assets/` dir (only if `assets/images/EkoScape.png` doesn't exist).

```
cmake --preset default
```

Optionally, for various params, see the [GH Workflows](.github/workflows/) or the [vcpkg docs](https://learn.microsoft.com/en-us/vcpkg/users/triplets).

Examples:

```
# Linux
cmake --preset default \
      -DVCPKG_TARGET_TRIPLET="x64-linux" \
      -DVCPKG_BUILD_TYPE=release \
      -DLINUXDEPLOY_ARCH="x86_64" \
      -DPKG_SUFFIX="-x86_64"

# macOS
cmake --preset default \
      -DBUILD_SHARED_LIBS=OFF \
      -DCMAKE_OSX_ARCHITECTURES=arm64 \
      -DVCPKG_OSX_ARCHITECTURES=arm64 \
      -DVCPKG_TARGET_TRIPLET="arm64-osx" \
      -DVCPKG_CRT_LINKAGE=static \
      -DVCPKG_LIBRARY_LINKAGE=static \
      -DVCPKG_BUILD_TYPE=release \
      -DPKG_SUFFIX="-arm64"

# Windows (PowerShell)
cmake --preset default `
      -DBUILD_SHARED_LIBS=OFF `
      -DVCPKG_TARGET_TRIPLET="x64-windows-static" `
      -DVCPKG_CRT_LINKAGE=static `
      -DVCPKG_LIBRARY_LINKAGE=static `
      -DVCPKG_BUILD_TYPE=release `
      -DPKG_SUFFIX="-x64"
```

To see the list of `vcpkg` Triplets, you can use the unofficial-official hack:

```
vcpkg install sdl2 --triplet=""
```

### Building ###

Build for Release or Debug:

```
cmake --build --preset default --config Release
cmake --build --preset default --config Debug
```

This defaults to 5 parallel jobs. Instead, you can either use the preset `no-jobs` (which causes Ninja to use the max number of jobs) or use the `-j` option:

```
cmake --build --preset no-jobs --config Release

cmake --build --preset default --config Release -j 1
```

### Running ###

Run from the top-level directory (for the `assets/` folder).

```
cmake --build --preset default --config Release --target run
cmake --build --preset default --config Debug --target run
```

Or run directly:

```
./bin/Release/EkoScape
./bin/Debug/EkoScape
```

### Checking Code Quality ###

Must have `cppcheck` installed.

```
cmake --build --preset default --config Release --target check
```

### Building Linux AppImage ###

This automatically downloads [linuxdeploy](https://github.com/linuxdeploy/linuxdeploy/releases/tag/continuous) to `build/downloads/` if it doesn't exist.

```
cmake --build --preset default --config Release --target appimage

# Test running it:
./build/AppImage/EkoScape-x86_64.AppImage
```

If your platform is not `x86_64`, you'll need to change which `linuxdeploy` to use. See the ones available [here](https://github.com/linuxdeploy/linuxdeploy/releases/tag/continuous).

```
rm -r build

# aarch64, armhf, i386
cmake --preset default -DLINUXDEPLOY_ARCH=aarch64

cmake --build --preset default --config Release --target appimage
```

### Packaging Up ###

When configuring, optionally add a suffix for the filename:

```
cmake --preset default -DPKG_SUFFIX="-x64"
```

Now run the target `package`. It uses `--install` & **CPack** to package up the files. Currently, `Release` is always used, even if you specify differently.

```
cmake --build --preset default --config Release --target package
```

### Miscellaneous ###

While playing the game, press `F3` to see the FPS in the top left. The game is capped at 60 FPS and has VSync enabled.

There are various scripts in the [scripts/](scripts/) folder for development, but not necessary, just for convenience.

### New Release ###

Optional: To update dependencies, update your `vcpkg` clone and then run `vcpkg x-update-baseline` in this project's folder. Clean any build folders and then test building & playing the game.

With the [GH CLI](https://cli.github.com) (`gh`) installed, run this script to download all Workflow Artifacts to `build/artifacts/`:

```
./scripts/get_artifacts.sh
```

(On Linux, to test the Windows icon in the exe, you can use `wine explorer .` and then check it out.)

Create a new release:

```
gh release create --draft --generate-notes v2.0 build/artifacts/*
```

## Credits ##

- [The Dantares Engine](third_party/Dantares/) by [Ryan Witmer](https://phasercat.com)
- [Monogram font](https://datagoblin.itch.io/monogram) by datagoblin
- [Star textures](https://kronbits.itch.io/particle-pack) by Kronbits
- [Piskel](https://www.piskelapp.com) for pixel editing.

## License ##

Due to The Dantares Engine being licensed under GPL, I also had to license this game under GPL. However, I might extract my Cybel Engine out (doesn't depend on The Dantares Engine) and place it under a more permissive license (like LGPL or MIT), but probably no one cares to use it anyway.

[GNU GPL v3+](LICENSE)

> EkoScape (<https://github.com/esotericpig/EkoScape>)  
> Copyright (c) 2004, 2024 Bradley Whited  
> 
> EkoScape is free software: you can redistribute it and/or modify  
> it under the terms of the GNU General Public License as published by  
> the Free Software Foundation, either version 3 of the License, or  
> (at your option) any later version.  
> 
> EkoScape is distributed in the hope that it will be useful,  
> but WITHOUT ANY WARRANTY; without even the implied warranty of  
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
> GNU General Public License for more details.  
> 
> You should have received a copy of the GNU General Public License  
> along with EkoScape.  If not, see <https://www.gnu.org/licenses/>.  
