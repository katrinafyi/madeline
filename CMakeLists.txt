cmake_minimum_required(VERSION 3.10)

project(madeline)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(HELLOIMGUI_WITH_SDL ON)
# add the executable
# add_executable(madeline src/main.cpp)

# add_subdirectory(vendor/sdl EXCLUDE_FROM_ALL)
# if(TARGET SDL2::SDL2main)
#     # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
#     target_link_libraries(madeline PRIVATE SDL2::SDL2main)
# endif()
# target_link_libraries(madeline PRIVATE SDL2::SDL2)

##########################################################
# Prepare hello_imgui during configure time
##########################################################
include(FetchContent)
FetchContent_Declare(
    hello_imgui
    GIT_REPOSITORY https://github.com/pthom/hello_imgui.git
    # Enter the desired git tag below
    GIT_TAG 08db4ba66fd35c5c6d6a336a35efd7d49f7d91e6
)
FetchContent_MakeAvailable(hello_imgui)
# Make cmake function `hello_imgui_add_app` available
list(APPEND CMAKE_MODULE_PATH ${HELLOIMGUI_CMAKE_PATH})
include(hello_imgui_add_app)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(z3
        GIT_REPOSITORY https://github.com/Z3Prover/z3
        GIT_TAG        z3-4.12.1
)
FetchContent_MakeAvailable(z3)


##########################################################
# Build your app
##########################################################
hello_imgui_add_app(madeline src/main.cpp src/hexcells.cpp src/lib/hexgrid.cpp src/ui.cpp src/prover.cpp)

# https://github.com/Z3Prover/z3/blob/master/examples/c%2B%2B/CMakeLists.txt
target_include_directories(madeline PRIVATE ${z3_SOURCE_DIR}/src/api )
target_include_directories(madeline PRIVATE ${z3_SOURCE_DIR}/src/api/c++ )
target_include_directories(madeline PRIVATE ${Z3_CXX_INCLUDE_DIRS})
target_link_libraries(madeline PRIVATE ${Z3_LIBRARIES})
target_compile_options(madeline PRIVATE ${Z3_COMPONENT_CXX_FLAGS})

target_link_libraries(madeline PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(madeline PRIVATE libz3)

target_compile_options(madeline PRIVATE -g -fsanitize=address,undefined -Wall -pedantic)
target_link_options(madeline PRIVATE -g -fsanitize=address,undefined)


