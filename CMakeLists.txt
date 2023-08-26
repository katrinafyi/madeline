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

##########################################################
# Build your app
##########################################################
hello_imgui_add_app(madeline src/main.cpp)


target_compile_options(madeline PRIVATE -g -fsanitize=address,undefined -Wall -pedantic)
target_link_options(madeline PRIVATE -g -fsanitize=address,undefined)


