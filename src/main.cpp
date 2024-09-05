/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ekoscape_game.h"

// SDL requires standard main().
// - https://wiki.libsdl.org/SDL2/FAQWindows#i_get_undefined_reference_to_sdl_main_%2E%2E%2E
int main(int /*argc*/,char** /*argv*/) {
  using namespace ekoscape;

  std::unique_ptr<EkoScapeGame> eko{};

  try {
    eko = std::make_unique<EkoScapeGame>();
  } catch(const CybelError& e) {
    EkoScapeGame::show_error_global(e.what());
    return 1;
  } catch(const std::invalid_argument& e) {
    EkoScapeGame::show_error_global(Util::build_str("Invalid argument: ",e.what()));
    return 2;
  }

  eko->run();

  return 0;
}

// TODO: implement 'r' to refresh assets.

// TODO: In GameScene, display text of author and title?
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
//       Break main loop into init_run()/run_frame() or w/e for emscripten.
// TODO: Make appimage/flatpack/snap.
// TODO: Publish on itch.io?
// TODO: Can we put this on Roku?
//       - https://www.4waytechnologies.com/blog/roku-new-idk-complete-guide-2021

// TODO: Use CMake & Conan.
//       - https://internalpointers.com/post/modern-cmake-beginner-introduction
//       - https://github.com/gosu/gosu/blob/master/CMakeLists.txt
// TODO: Test pulling down project in clean Linux VM and building.
