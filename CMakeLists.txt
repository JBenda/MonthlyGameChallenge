cmake_minimum_required(VERSION 3.10) 
project("Chase Dungeon")

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR} inc/)

aux_source_directory(src/ SOURCE_FILES)
add_executable(enter ${SOURCE_FILES})
target_link_libraries(enter ${CURSES_LIBRARIES})
