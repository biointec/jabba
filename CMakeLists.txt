cmake_minimum_required (VERSION 2.6)

project (Jabba)

# project version
set (${PROJECT_NAME}_MAJOR_VERSION 1)
set (${PROJECT_NAME}_MINOR_VERSION 1)
set (${PROJECT_NAME}_PATCH_LEVEL 0)
add_definitions("-DJABBA_MAJOR_VERSION=${${PROJECT_NAME}_MAJOR_VERSION}")
add_definitions("-DJABBA_MINOR_VERSION=${${PROJECT_NAME}_MINOR_VERSION}")
add_definitions("-DJABBA_PATCH_LEVEL=${${PROJECT_NAME}_PATCH_LEVEL}")

# set compiler
set(CMAKE_CXX_COMPILER /usr/bin/g++)
# set g++ specific flags
#set(CMAKE_CXX_FLAGS "-std=c++11 -ggdb -g3 -pedantic -Wall -Wextra -Wno-sign-compare -fopenmp -lpthread")
set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -pedantic -Wall -Wextra -Wno-sign-compare -fopenmp -lpthread -DNDEBUG -o $@")

add_subdirectory(src)
