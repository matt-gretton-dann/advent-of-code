cmake_minimum_required(VERSION 3.21)
project(advent-of-code)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB years LIST_DIRECTORIES true RELATIVE "${CMAKE_SOURCE_DIR}" CONFIGURE_DEPENDS
        "20[1-9][0-9]")
message(STATUS "Years to examine: ${years}")
foreach (year IN LISTS years)
  file(GLOB puzzles RELATIVE "${CMAKE_SOURCE_DIR}" CONFIGURE_DEPENDS "${year}/puzzle-*.cc")
  foreach (puzzle IN LISTS puzzles)
    string(REGEX REPLACE "^.*puzzle-(.*)\\.cc" "puzzle-${year}-\\1" puzzle_name "${puzzle}")
    string(REGEX REPLACE "^.*puzzle-(.*)\\.cc"
            "${CMAKE_SOURCE_DIR}/${year}/puzzle-\\1.CMakeLists.txt" sub_cmake_lists "${puzzle}")
    message(STATUS "Puzzle: ${puzzle_name} - source ${puzzle}")
    add_executable("${puzzle_name}" "${puzzle}")
    if (EXISTS "${sub_cmake_lists}")
      include("${sub_cmake_lists}")
    endif ()
  endforeach ()
endforeach ()
