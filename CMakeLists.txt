cmake_minimum_required(VERSION 2.8)

project(Bonus_VyhodnoceniVyrazu)
aux_source_directory( . SRC )
add_compile_options(-Wall -Wextra -std=c99 -g)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} ${SRC})
