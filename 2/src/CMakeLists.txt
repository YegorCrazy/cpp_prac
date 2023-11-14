project("c++ prac 2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -std=c++17")
include_directories(../generator/src)
add_executable(main src/main.cpp src/classes.h src/classes.cpp ../generator/src/generate.h ../generator/src/generate.cpp)