/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cybel/types/cybel_error.h"

#include "ekoscape_game.h"

#if defined(__EMSCRIPTEN__)

static std::unique_ptr<ekoscape::EkoScapeGame> eko{};

void run_ems_frame();
bool on_ems_gl_context(int event_type,const void* reserved,void* user_data);

#endif // __EMSCRIPTEN__.

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

  #if defined(__EMSCRIPTEN__)
    try {
      eko = std::make_unique<EkoScapeGame>();
    } catch(const CybelError& e) {
      EkoScapeGame::show_error_global(e.what());
      return 1;
    }

    // emscripten_set_webglcontextlost_callback("#canvas",nullptr,false,on_ems_gl_context);
    // emscripten_set_webglcontextrestored_callback("#canvas",nullptr,false,on_ems_gl_context);
    emscripten_set_main_loop(run_ems_frame,0,false);
  #else
    try {
      EkoScapeGame eko{};
      eko.run_loop();

      std::cerr << "[INFO] Stopping gracefully." << std::endl;
    } catch(const CybelError& e) {
      EkoScapeGame::show_error_global(e.what());
      return 1;
    }
  #endif

  return 0;
}

#if defined(__EMSCRIPTEN__)

void run_ems_frame() {
  using namespace ekoscape;

  if(!eko) { return; }

  try {
    if(!eko->run_frame()) {
      std::cerr << "[INFO] Stopping gracefully." << std::endl;
      eko = nullptr;
      emscripten_cancel_main_loop();
    }
  } catch(const CybelError& e) {
    eko->show_error(e.what());
    eko = nullptr;
    emscripten_cancel_main_loop();
  }
}

// Decided not to use this because it's too much work to restore the game state after GL context restored.
// It's better to just reload the entire page.
//
// To simulate GL context lost & restored in JS:
//   let gle = Module.canvas.getContext('webgl2').getExtension('WEBGL_lose_context');
//   gle.loseContext();
//   gle.restoreContext();
//
// See:
// - https://www.khronos.org/webgl/wiki/HandlingContextLost
// - https://emscripten.org/docs/api_reference/html5.h.html#id93
bool on_ems_gl_context(int event_type,const void* /*reserved*/,void* /*user_data*/) {
  // using namespace ekoscape;

  switch(event_type) {
    case EMSCRIPTEN_EVENT_WEBGLCONTEXTLOST:
      std::cerr << "[WARN] WebGL context lost." << std::endl;
      break;

    case EMSCRIPTEN_EVENT_WEBGLCONTEXTRESTORED:
      std::cerr << "[INFO] WebGL context restored. Attempting to re-store game." << std::endl;
      break;
  }

  return false;
}

#endif // __EMSCRIPTEN__.
