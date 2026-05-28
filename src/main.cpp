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

  CybelEngine* engine = nullptr;

  try {
    engine = &CybelEngine::init(EkoScapeGame::build_config());

    engine->run(std::make_unique<EkoScapeGame>(*engine));
  } catch(const CybelError& e) {
    if(engine) {
      engine->show_error(e.what());
    } else {
      CybelEngine::show_error_no_window(EkoScapeGame::kTitle,e.what());
    }

    return 1;
  }

  return 0;
}
