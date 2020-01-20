cmake_minimum_required(VERSION 3.12) 
project("Chess Dungeon")


# find_package(Curses REQUIRED)
set(CURSES_INCLUDE_DIRS "/usr/include/" "/usr/include/x86_64-linux-gnu/")
set(CURSES_LIBRARIES 
	"/lib/x86_64-linux-gnu/libncurses.so.5"
	"/lib/x86_64-linux-gnu/libtinfo.so.5")

include_directories(${CURSES_INCLUDE_DIRS} inc/)

aux_source_directory(src/ SOURCE_FILES)
add_executable(enter ${SOURCE_FILES})
set_property(TARGET enter PROPERTY CXX_STANDARD 20)
target_link_libraries(enter ${CURSES_LIBRARIES})
