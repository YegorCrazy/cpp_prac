cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -std=c++17")

SET(SRC src/functions.h src/functions.cpp)

add_executable(test src/tests.cpp ${SRC})
target_link_libraries(test gtest gmock)

add_executable(main src/main.cpp ${SRC})
