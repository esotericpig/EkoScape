/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "core/common.h"

#include "core/ekoscape_error.h"
#include "core/util.h"

#include "ekoscape.h"

// TODO: Go ahead and commit map files to have original. Then change speeds later.
// TODO: Make desktop entry with assets icon.

// TODO: Implement MenuScene.
//       - Have `default` and `realistic` graphics and can choose in menu.
//       - gluOrtho2D(0.0,width_,0.0,height_);
// TODO: Implement GameOver (GameOverScene?) in GameScene to show message and to press Enter.
//       After enter, go to MenuScene.
// TODO: Implement mini map (MiniMapScene?) in GameScene and include number of Ekos rescued out of total.
// TODO: Use own Ogg music from iPad.
// TODO: If special flag, make weird colors and also flip cell & robot (flips capture).
// TODO: Cruise control button? So continuously presses up. Can turn on or off.
//       Maybe always on?

// TODO: Experiment with different tex for dead/living spaces.
//       probably have living space look slightly different in some way.
//       use marble for white tex.
// TODO: Set target_fps to 40 (or 30) to see what original time should be and adjust delay/speeds.
// TODO: Make all maps easier to play.

// TODO: Add to README that you can make a pull request to submit a new game,
//       or can create an issue, or comment in discussion tab.
// TODO: Update all READMEs.
// TODO: Make flatpack/snap/appimage.
// TODO: Publish on itch.io?

// TODO: Use CMake. Probably Premake actually. Maybe use CLion first and see what it uses.
//       - https://internalpointers.com/post/modern-cmake-beginner-introduction
//       Use Meson?
//       - https://mesonbuild.com/Wrapdb-projects.html
//       - https://mesonbuild.com/Wrap-dependency-system-manual.html
//       - and just have ruby script to update source files
//       - https://mesonbuild.com/FAQ.html#should-i-check-for-buildtype-or-individual-options-like-debug-in-my-build-files
//       - mylib = library('mylib', 'mysource.c', c_args: ['-DFOO'])


// TODO: Remove after testing.
using namespace ekoscape;


// I believe SDL requires `int main(int,char*[])`, so can't use `int main()`, etc.
// - https://wiki.libsdl.org/SDL2/FAQWindows#i_get_undefined_reference_to_sdl_main_...
int main(int /*argc*/,char** /*argv*/) {
  using namespace ekoscape;

  std::unique_ptr<EkoScape> eko{};

  try {
    EkoScape::Config config{};

    config.ge.scale_factor = 0.8333f; // Arrival?
    config.ge.target_fps = 60;
    //config.ge.clear_color.set_i(106,48,137,255); // Cosmic purple.

    eko = std::make_unique<EkoScape>(config);
  } catch(const EkoScapeError& e) {
    EkoScape::show_error_globally(e.what());
    return -1;
  } catch(const std::invalid_argument& e) {
    EkoScape::show_error_globally(Util::build_string("Invalid argument: ",e.what()));
    return -1;
  }

  if(eko) { eko->run(); }

  return 0;
}
