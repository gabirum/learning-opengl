cmake_minimum_required(VERSION 3.25.0)
project(render VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(cglm CONFIG REQUIRED)
find_package(Stb REQUIRED)

add_library(glad STATIC lib/glad/src/gl.c)
target_include_directories(glad PUBLIC lib/glad/include)

file(GLOB sources "src/*.c" "src/**/*.c")
add_executable(render ${sources})
target_compile_features(render PRIVATE c_std_17)
target_include_directories(render PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(render glfw glad cglm::cglm)
