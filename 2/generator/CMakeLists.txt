project("c++ prac 2 generator")
add_executable(main src/main.cpp src/generate.h src/generate.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address")