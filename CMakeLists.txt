cmake_minimum_required(VERSION 3.26)
project(teste)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

set(CMAKE_C_STANDARD 11)

add_executable(teste main.c)
target_link_libraries(teste ${SDL2_LIBRARY} -lmingw32 -lSDL2main -lSDL2 m)


