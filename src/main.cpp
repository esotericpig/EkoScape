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

/*
TODO: Use own Ogg music from iPad.
TODO: Experiment with different tex for dead/living spaces.
      probably have living space look slightly different in some way.
      use marble for white tex.

TODO: Set target_fps to 40 (or 30) to see what original time should be and adjust delay/speeds.
TODO: Make all maps easier to play. I think fruit will solve this.

TODO: Update all READMEs.
      - Add to README that you can make a pull request to submit a new game,
        or can create an issue, or comment in discussion tab.

TODO: Use CMake & Conan.
      - https://internalpointers.com/post/modern-cmake-beginner-introduction
      - https://github.com/gosu/gosu/blob/master/CMakeLists.txt
TODO: Test pulling down project in clean Linux VM and building.

TODO: Try using emscripten to make web version.
      - Break main loop into init_run()/run_frame() or w/e for emscripten.
TODO: Make appimage/flatpack/snap.
      - Make desktop entry with assets icon.
TODO: Publish on itch.io?
*/
