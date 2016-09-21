cmake_minimum_required(VERSION 3.2)
project(opengl_examples)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=gnu11")

macro(add_executable _name)
    # invoke built-in add_executable
    _add_executable(${ARGV})
    if (TARGET ${_name})
        target_link_libraries(${_name} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} m)
    endif ()
endmacro()

add_executable(ex1 lec01/ex1.c)
add_executable(ex2 lec01/ex2.c)
add_executable(ex3 lec01/ex3.c)
add_executable(ex4 lec01/ex4.c)
add_executable(ex5 lec01/ex5.c)
add_executable(ex6 lec03/ex6/ex6.c)
add_executable(ex7 lec03/ex7/ex7.c)
add_executable(ex8 lec04/ex8/ex8.c)
add_executable(hw1 hw1/gears.c)
add_executable(hw2 hw2/lorenz.c)
add_executable(hw3 hw3/hw3.c)

