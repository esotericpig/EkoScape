/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ekoscape/global.h"

#include "ekoscape/ekoscape.h"

#include <cybel/core/cybel_error.h>
#include <cybel/cybel_engine.h>

#include <iostream>
#include <string_view>

// SDL2 requires standard main().
// - https://wiki.libsdl.org/SDL2/FAQWindows#i_get_undefined_reference_to_sdl_main_%2E%2E%2E
int main(int argc,char** argv) {
  using namespace ekoscape;

  for(int i = 1; i < argc; ++i) {
    if(std::string_view{argv[i]} == "--version") {
      std::cout << EkoScape::kTitle << std::endl;
      return 0;
    }
  }

  try {
    auto& engine = CybelEngine::init(EkoScape::build_config());

    engine.run<EkoScape>(engine);
  } catch(const CybelError& e) {
    CybelEngine::show_error(e.what());
    return 1;
  }

  return 0;
}
