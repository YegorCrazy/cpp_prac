project("c++ prac 1")
add_executable(main src/main.cpp src/rational.h src/matrix.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
