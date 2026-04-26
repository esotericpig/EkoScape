/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cybel/cybel_engine.h"
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
    auto& cybel_engine = CybelEngine::init(EkoScapeGame::build_config());

    cybel_engine.run(std::make_unique<EkoScapeGame>(cybel_engine));
  } catch(const CybelError& e) {
    CybelEngine::show_error_global(EkoScapeGame::kTitle,e.what());
    return 1;
  }

  return 0;
}
