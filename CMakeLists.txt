cmake_minimum_required(VERSION 3.2)
project(opengl_examples)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

include_directories(lib/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

link_libraries(${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} m)

SET(LIB_SRC lib/src/errcheck.c lib/src/fatal.c lib/src/loadtexbmp.c lib/src/object.c lib/src/print.c lib/src/project.c)
add_library(CSCIx229 STATIC ${LIB_SRC})

add_executable(ex1 lec01/ex1.c)
add_executable(ex2 lec01/ex2.c)
add_executable(ex3 lec01/ex3.c)
add_executable(ex4 lec01/ex4.c)
add_executable(ex5 lec01/ex5.c)
add_executable(ex6 lec03/ex6/ex6.c)
add_executable(ex7 lec03/ex7/ex7.c)
add_executable(ex8 lec04/ex8/ex8.c)
add_executable(ex9 lec04/ex9/ex9.c)
add_executable(hw1 hw1/gears.c)
add_executable(hw2 hw2/lorenz.c)
add_executable(hw3 hw3/hw3.c)
add_executable(hw4 hw4/hw4.c)
add_executable(hw5 hw5/hw5.c)

link_libraries(CSCIx229)
add_executable(ex10 lec05/ex10/ex10.c)
add_executable(ex11 lec05/ex11/ex11.c)
add_executable(ex13 lec06/ex13/ex13.c)
add_executable(ex27 lec11/ex27.c)
