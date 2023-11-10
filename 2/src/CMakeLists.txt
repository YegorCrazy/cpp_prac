project("c++ prac 2")
add_executable(main src/main.cpp src/classes.h src/classes.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address")