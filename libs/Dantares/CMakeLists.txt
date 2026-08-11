cmake_minimum_required(VERSION 3.22)

project("Dantares"
    VERSION "2.0"
    LANGUAGES C CXX
)

add_library(Dantares OBJECT)
add_library(Dantares::Dantares ALIAS Dantares)

target_compile_features(Dantares PUBLIC cxx_std_20)

set(dan_compile_defs "")

if(EMSCRIPTEN)
    list(APPEND dan_compile_defs DANTARES_PLATFORM_WEB)
elseif(APPLE)
    list(APPEND dan_compile_defs DANTARES_PLATFORM_MACOS)
elseif(WIN32)
    list(APPEND dan_compile_defs DANTARES_PLATFORM_WINDOWS)
else() # Linux
    list(APPEND dan_compile_defs DANTARES_PLATFORM_LINUX)
endif()

target_compile_definitions(Dantares PUBLIC
    ${dan_compile_defs}
)

target_include_directories(Dantares PUBLIC
    "src"
)
target_sources(Dantares PRIVATE
    # @glob_src=src:src/
#    "src/Dantares/Dantares.cpp"
    "src/Dantares/Dantares2.cpp"
    "src/Dantares/Dantares2GL.cpp"
    # @glob_src
)
