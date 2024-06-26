cmake_minimum_required(VERSION 3.25.0)
project(render VERSION 0.1.0 LANGUAGES C)

file(GLOB sources "src/*.c" "src/**/*.c")
add_executable(render ${sources})
target_compile_features(render PRIVATE c_std_17)

add_library(glad lib/glad/src/gl.c)
target_include_directories(glad PUBLIC lib/glad/include)

add_library(stb lib/stb/src/stb_image.c)
target_include_directories(stb PUBLIC lib/stb/include)

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME glfw
  VERSION 3.4
  GITHUB_REPOSITORY glfw/glfw
  GIT_TAG 3.4
)

target_link_libraries(render glfw glad stb)
