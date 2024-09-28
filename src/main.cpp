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
