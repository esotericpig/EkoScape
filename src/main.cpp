/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cybel/types/cybel_error.h"

#include "ekoscape_game.h"

// SDL2 requires standard main().
// - https://wiki.libsdl.org/SDL2/FAQWindows#i_get_undefined_reference_to_sdl_main_%2E%2E%2E
int main(int argc,char** argv) {
  using namespace ekoscape;

  for(int i = 1; i < argc; ++i) {
    if(std::string_view{argv[i]} == "--version") {
      std::cout << EkoScapeGame::kTitle << std::endl;
      return 0;
    }
  }

  try {
#if defined(__EMSCRIPTEN__)
    EkoScapeGame::run_on_web();
#else
    EkoScapeGame eko_game{};
    eko_game.run_loop();

    std::cout << "[INFO] Stopping gracefully." << std::endl;
#endif
  } catch(const CybelError& e) {
    EkoScapeGame::show_error_global(e.what());
    return 1;
  }

  return 0;
}
