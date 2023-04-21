cmake_minimum_required (VERSION 2.8.11)
project (cpp20-coroutines-for-beginners)

set (CMAKE_CXX_STANDARD 20)

add_executable(chat sources/chat/main.cpp)
add_executable(interleave sources/interleave/main.cpp)
add_executable(scheduling sources/scheduling/main.cpp)
add_executable(scheduling_alternative sources/scheduling_alternative/main.cpp)
