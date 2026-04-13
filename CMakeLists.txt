# FreeRS-42; a re-implementation of Ricochet Xtreme under SDL.
#
# Please see the file LICENSE in the source's root directory.

cmake_minimum_required(VERSION 3.20.0)
project(FRS42 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(engine EXCLUDE_FROM_ALL)

# ------------------------------------------------------------------------------
# Code Quality Tools
# ------------------------------------------------------------------------------
include(cmake/CodeQuality.cmake)

add_subdirectory(src)
