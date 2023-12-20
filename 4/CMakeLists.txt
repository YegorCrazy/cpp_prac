cmake_minimum_required(VERSION 3.0)

SET(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

SET(SRC src/LifeCondition.h src/LifeCondition.cpp src/Mutations.h src/Selection.h 
    src/Selection.cpp src/Breed.h src/Breed.cpp src/MainCycle.h src/MainCycle.cpp)

add_executable(main src/main.cpp ${SRC})

target_link_libraries(main ncurses)