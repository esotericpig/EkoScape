/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ekoscape.h"

// TODO: implement 'r' to refresh assets.
// TODO: credits in ReadMe and in game: Ryan, Monogram
//       - https://datagoblin.itch.io/monogram
//       - https://www.piskelapp.com/
//       - And for stars too?
//       If press 'f', make weird colors and also flip cell & robot (flips capture).
//       Also use star2.png.
// TODO: Implement MenuPlayScene.
//       Need to walk assets/maps dir. Show as "classic / Title by Author" or something.
//       'r' in MenuPlay should also re-crawl dirs.

// TODO: Implement mini map in GameScene and include number of Ekos rescued out of total.
//       Just do colors. If walkable, then nothing. If non-walkable or Entity/Thing, then color.
//       - Special color for non-walkable, Robot, Cell, and "fruit".
//       Moore neighborhood of 4 spaces out or whatever (param). Can implement in Map.
// TODO: Implement Game Over in GameScene to show message and to press Enter.
//       After enter, kGoBack.
//       Continue to have update/draw, but ignore user input.

// TODO: Use own Ogg music from iPad.
// TODO: Cruise control button? So continuously presses up. Can turn on or off.
//       Maybe always on?
// TODO: Implement "fruit" in Game so can eat robots -- pacman: if eat, they go back to origin?
// TODO: Implement portals and multiple layers/floors? -- pacman: 0-9 are matching portals; 0<=>0, 1<=>1
// TODO: Any other pacman stuff?

// TODO: Experiment with different tex for dead/living spaces.
//       probably have living space look slightly different in some way.
//       use marble for white tex.

// TODO: Set target_fps to 40 (or 30) to see what original time should be and adjust delay/speeds.
// TODO: Make all maps easier to play.

// TODO: Add to README that you can make a pull request to submit a new game,
//       or can create an issue, or comment in discussion tab.
// TODO: Update all READMEs.

// TODO: Make desktop entry with assets icon.
// TODO: Try using emscripten to make web version.
// TODO: Make flatpack/snap/appimage.
// TODO: Publish on itch.io?
// TODO: Can we put this on Roku?
//       - https://www.4waytechnologies.com/blog/roku-new-idk-complete-guide-2021

// TODO: Use CMake & vcpkg. Maybe use CLion first and see what it uses.
//       - https://internalpointers.com/post/modern-cmake-beginner-introduction
//       - https://github.com/gosu/gosu/blob/master/CMakeLists.txt
//       Use Meson?
//       - https://mesonbuild.com/Wrapdb-projects.html
//       - https://mesonbuild.com/Wrap-dependency-system-manual.html
//       - and just have ruby script to update source files
//       - https://mesonbuild.com/FAQ.html#should-i-check-for-buildtype-or-individual-options-like-debug-in-my-build-files
//       - mylib = library('mylib', 'mysource.c', c_args: ['-DFOO'])
// TODO: Test pulling down project in clean Linux VM and building.

// I believe SDL requires `int main(int,char*[])`.
// - https://wiki.libsdl.org/SDL2/FAQWindows#i_get_undefined_reference_to_sdl_main_...
int main(int /*argc*/,char** /*argv*/) {
  using namespace ekoscape;

  std::unique_ptr<EkoScape> eko{};

  try {
    EkoScape::Config config{};

    config.scale_factor = 0.8333f; // Arrival?
    //config.size = {740,500}; // For GIFs/screenshots.
    config.fps = 60;
    config.vsync = true;

    // This is the width/height that the game is developed in and used for scaling 2D sprites (menu, etc.).
    // These are fixed values and should not be changed.
    config.target_size = {1600,900};

    eko = std::make_unique<EkoScape>(config);
  } catch(const CybelError& e) {
    EkoScape::show_error_global(e.what());
    return -1;
  } catch(const std::invalid_argument& e) {
    EkoScape::show_error_global(Util::build_str("Invalid argument: ",e.what()));
    return -1;
  }

  eko->run();

  return 0;
}
