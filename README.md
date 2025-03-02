# EkoScape #

[![EkoShot](https://github.com/user-attachments/assets/51fa9a49-e7b5-4543-b68d-e827af763cd2)](https://esotericpig.itch.io/ekoscape)

[![itch.io](https://img.shields.io/badge/play-itch.io-fa5c5c)](https://esotericpig.itch.io/ekoscape)
[![Source Code](https://img.shields.io/badge/source-github-211f1f.svg)](https://github.com/esotericpig/EkoScape)
[![License](https://img.shields.io/github/license/esotericpig/EkoScape.svg)](LICENSE)

EkoScape is a simple 3D step-based game where you run through a maze rescuing your fellow alien *Ekos*, like a 3D Pac-Man, with robot enemies, fruit, and portals. The maps are simple text files that you can edit!

## Playing ##

You can play in your Web browser or download the game on [itch.io](https://esotericpig.itch.io/ekoscape).

If you choose to download, simply run it in the same folder containing the `assets` folder, or you can install & run the game from the official [itch app](https://itch.io/app) where the game has been tested to work in sandbox mode (if you're concerned about security).

You can edit the Map files in [assets/maps/](assets/maps/) or make your own! See [assets/maps/README.md](assets/maps/README.md) for more details, which also includes how to submit your Map files for the next version.

### System Requirements ###

Web:
- WebGL 2.0+ (OpenGL ES 3.0+)
- It's recommended to turn on hardware/graphics acceleration in your browser's settings.

Desktop:
- OpenGL 2.1+
  - Use Mesa if you have a lower version.
- Linux x86_64 (AppImage)
  - glibc 2.14+ (Ubuntu 22.04+; Fedora 16+)
  - Might need [FUSE](https://github.com/AppImage/AppImageKit/wiki/FUSE) for running the AppImage -- usually already installed.
- macOS arm64 or x86_64
  - Because this game is free and signing costs money, it's currently unsigned, so you'll need to manually override the security warning when running:
    - https://support.apple.com/guide/mac-help/open-a-mac-app-from-an-unknown-developer-mh40616/mac
- Windows x64

### Additional System Notes ###

- Linux:
  - A Desktop Entry file is provided if you wish to integrate it with your DE (Desktop Environment). Open the provided file, `io.github.esotericpig.ekoscape.desktop`, in a text editor to read more details or [read online here](https://github.com/esotericpig/EkoScape/blob/main/res/io.github.esotericpig.ekoscape.desktop).

## Code History ##

EkoScape is a remake of an old game I made back in 2004 (with minor edits in later years) using my friend's 3D step-based engine: The Dantares Engine by Ryan Witmer. You can find the original code in [relics/](relics/).

Back then, I made it for Windows only. In 2024, I re-wrote the code for it in SDL2 (multi-platform) using modern C++20. However, it still uses The Dantares Engine and legacy OpenGL 2.1+, as homage to the original game and engine.

In the beginning, I rewrote the original code for fun in a couple of days. Having enjoyed the process, I decided to flesh it out into multiple, generic files, while adding a menu and a lot of extra stuff (such as portals, fruit, and multiple grids), which took over a month.

The code is a bit over-engineered, but I designed it so that I could use parts of it in other projects. I did make a simple ECS (Entity-Component-System) for it, but because the game is quite simple and the ECS code produced more files/lines of code, I decided to just stick with the original class-based structure.

Initially, `src/cybel` was named `src/core`, but I decided to make it into its own Game Engine (kind of). I then put it in its own namespace, called `cybel`. Because of this, I simply use `using namespace cybel` inside of the `ekoscape` namespace, as I didn't like putting `cybel::` everywhere.

**Update 2025-02:** I updated the game to work on the Web using Emscripten. I had to add OpenGL ES 3.0 (WebGL 2.0) support to the Renderer class and to Dantares (made a new Dantares2 class to preserve the "original").

## Contents ##

- [Hacking](#hacking)
  - [Setup](#setup)
  - [IDEs](#ides)
  - [Configuring Build](#configuring-build)
  - [Building](#building)
  - [Running](#running)
  - [Checking Code Quality](#checking-code-quality)
  - [Building Linux AppImage](#building-linux-appimage)
  - [Packaging Up](#packaging-up)
  - [Miscellaneous](#miscellaneous)
- [Hacking for Web](#hacking-for-web)
  - [OpenGL ES for Desktop](#opengl-es-for-desktop)
  - [Setup for Web](#setup-for-web)
  - [CMake for Web](#cmake-for-web)
  - [Misc. for Web](#misc-for-web)
- [Releasing](#releasing)
  - [New Release](#new-release)
  - [Publishing](#publishing)
- [Credits](#credits)
- [License](#license)

## Hacking ##

### Setup ###

This project uses **CMake**, [Ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages), and [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started). Please install these for your system.

- For quickly getting started, see the [GitHub Workflows](.github/workflows/) or the top of [CMakeLists.txt](CMakeLists.txt).
- If you have Ruby installed, there is also a script -- [scripts/dev.rb](scripts/dev.rb) -- for rapid development.

### IDEs ###

Optional stuff for IDEs.

- **CLion**
  - CMake Profiles are included.
  - The env var `VCPKG_ROOT` is defaulted to `./vcpkg` so that you can just make a symlink to the actual path in the top of this project's folder, or instead, you'll need to edit the profiles (Settings => Build, Execution, Deployment => CMake) to change the env var appropriately: `VCPKG_ROOT=/path/to/vcpkg`.

### Configuring Build ###

Platform-specific notes:
- On **Linux**, OpenGL/GLEW/GLU are only façades in `vcpkg`, so you need to install them manually. Also, you might need additional system dependencies if it builds SDL2 from source.
  - On Ubuntu, you can simply look at the [Linux GH Workflow](.github/workflows/linux.yml).
  - https://github.com/libsdl-org/SDL/blob/SDL2/docs/README-linux.md
  - https://github.com/AppImage/AppImageKit/wiki/FUSE
- On **macOS**, you need to install GLEW manually:
  - `brew install glew`

The following command will be very slow the first time you run it, as `vcpkg` downloads the dependencies. Also, it downloads & extracts additional [Assets](https://github.com/esotericpig/EkoScape/releases/tag/v1.99) to the `assets/` dir (only if `assets/images/EkoScape.png` doesn't exist).

```bash
cmake --preset default
```

Optionally, for various params, see the [GH Workflows](.github/workflows/) or the [vcpkg docs](https://learn.microsoft.com/en-us/vcpkg/users/triplets).

Examples:

```bash
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

```bash
vcpkg install sdl2 --triplet=""
```

### Building ###

Build for Release or Debug:

```bash
cmake --build --preset default --config Release
cmake --build --preset default --config Debug
```

By default, Ninja uses the maximum number of parallel jobs for your system (which can cause your computer to run like crazy). Instead, you can use the `-j` option to use less jobs in parallel:

```bash
cmake --build --preset default --config Release -j 1
cmake --build --preset default --config Release -j 5
```

### Running ###

Run from the top-level directory (for the `assets/` folder):

```bash
cmake --build --preset default --config Release --target run
cmake --build --preset default --config Debug --target run
```

Or run directly:

```bash
./bin/Release/EkoScape
./bin/Debug/EkoScape
```

### Checking Code Quality ###

Must have `cppcheck` installed.

```bash
cmake --build --preset default --config Release --target check
```

### Building Linux AppImage ###

This automatically downloads [linuxdeploy](https://github.com/linuxdeploy/linuxdeploy/releases/tag/continuous) to `build/downloads/` if it doesn't exist. The config is ignored and always uses the Release config.

```bash
cmake --build --preset default --config Release --target appimage

# Test running it:
./build/AppImage/EkoScape-x86_64.AppImage
```

If your platform is not `x86_64`, you'll need to change which `linuxdeploy` to use. See the ones available [here](https://github.com/linuxdeploy/linuxdeploy/releases/tag/continuous).

```bash
rm -r build

# aarch64, armhf, i386, static-x86_64, x86_64
cmake --preset default -DLINUXDEPLOY_ARCH=aarch64

cmake --build --preset default --config Release --target appimage
```

### Packaging Up ###

When configuring, optionally add a suffix for the filename:

```bash
cmake --preset default -DPKG_SUFFIX="-x64"
```

Now run the target `package`. It uses `--install` & **CPack** to package up the files.

```bash
cmake --build --preset default --config Release --target package
```

### Miscellaneous ###

While playing the game, press `F3` to see the FPS in the top left corner. The game is capped at 60 FPS.

There are various scripts in the [scripts/](scripts/) folder for development, but not necessary, just for convenience.

## Hacking for Web ##

### OpenGL ES for Desktop ###

For rapid testing of OpenGL ES without using the Web (slower compile/link time), simply edit the `default` preset in [CMakePresets.json](CMakePresets.json) to use GLES:

```json
{
  "RENDERER": "GLES"
}
```

Or, pass it in when configuring CMake using `-DRENDERER=GLES`.

Now you can test OpenGL ES in the Desktop version instead.

### Setup for Web ###

Install [Emscripten](https://emscripten.org/docs/getting_started/downloads.html).

On Linux/macOS, I don't recommend adding `source ./emsdk_env.sh` to your Bash/Zsh RC file because it outputs a bunch of noise every time you open a terminal. I just make sure to call it before I need it by using an alias:

```bash
alias ems_src='source "$HOME/Code/clones/emsdk/emsdk_env.sh"'
```

On Windows, you need to run `emsdk_env.bat` instead.

You should now have an `EMSDK` env var, and the below file should exist, which is used in [CMakePresets.json](CMakePresets.json):

```bash
ls "$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
```

### CMake for Web ###

Now just use the CMake preset `web`, and everything should just work like normal. This preset uses separate `build_web` & `bin_web` folders than the `default` preset so that you can test both the Desktop & Web versions without having to rebuild everything when switching between them.

```bash
# Configure for Web.
cmake --preset web

# Build for Web.
cmake --build --preset web --config Release
# - Use 1 job instead.
cmake --build --preset web --config Release -j 1

# Run for Web [starts a server using `emrun` (Python3)].
cmake --build --preset web --config Release --target run
# - Or, run directly.
emrun --no-browser bin_web/Release
```

### Misc. for Web ###

Both [scripts/dev.rb](scripts/dev.rb) and [scripts/artifacts.rb](scripts/artifacts.rb) have Web options, if you use those scripts.

## Releasing ##

### New Release ###

Optional: To update dependencies, update your `vcpkg` clone and then run `vcpkg x-update-baseline` in this project's folder. Clean any build folders and then test building & playing the game.

Bump the version in the following places:
- [CMakeLists.txt](CMakeLists.txt)
- [src/ekoscape_game.h](src/ekoscape_game.h)
- [res/io.github.esotericpig.ekoscape.metainfo.xml](res/io.github.esotericpig.ekoscape.metainfo.xml)

With the [GitHub CLI](https://cli.github.com) (`gh`), create a new release. Make sure to change the versions appropriately (at the end):

```bash
gh release create --draft --notes "$(git --no-pager log "$(git describe --tags --abbrev=0)"..HEAD --pretty=format:'- %B')" --title v2.x v2.x
```

### Publishing ###

See [scripts/artifacts.rb](scripts/artifacts.rb) for all of your publishing needs.

- On Linux, to test the Windows icon in the exe, you can use `wine explorer .` and then check it out.

Example workflow:

```bash
# Use `-n` to only perform a dry-run.
# Use `-c <channel>` to filter which artifacts/channels to use (fuzzy search).

# Download GitHub artifacts to `build/artifacts/`.
./scripts/artifacts.rb -g

# Extract (decompress) artifacts to `build/artifacts/`.
./scripts/artifacts.rb -x

# Validate artifact folders for itch.io.
./scripts/artifacts.rb -v -c lin
./scripts/artifacts.rb -v -c mac
./scripts/artifacts.rb -v -c win
./scripts/artifacts.rb -v -c web

# Publish artifact folders to itch.io.
./scripts/artifacts.rb -I -c lin
./scripts/artifacts.rb -I -c mac
./scripts/artifacts.rb -I -c win
./scripts/artifacts.rb -I -c web

# Check status of itch.io builds.
./scripts/artifacts.rb -s
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
> Copyright (c) 2004, 2024-2025 Bradley Whited  
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
