cmake_minimum_required (VERSION 2.6.3)

project (Jabba)

# project version
set (${PROJECT_NAME}_MAJOR_VERSION 1)
set (${PROJECT_NAME}_MINOR_VERSION 0)
set (${PROJECT_NAME}_PATCH_LEVEL 0)
add_definitions("-DJABBA_MAJOR_VERSION=${${PROJECT_NAME}_MAJOR_VERSION}")
add_definitions("-DJABBA_MINOR_VERSION=${${PROJECT_NAME}_MINOR_VERSION}")
add_definitions("-DJABBA_PATCH_LEVEL=${${PROJECT_NAME}_PATCH_LEVEL}")

# set the default configuration to Release
if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# set the module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# set some definitions
add_definitions("-DMAXKMERLENGTH=127")
add_definitions("-DCATEGORIES=2")
add_definitions("-DBROWNIE_MAJOR_VERSION=${${PROJECT_NAME}_MAJOR_VERSION}")
add_definitions("-DBROWNIE_MINOR_VERSION=${${PROJECT_NAME}_MINOR_VERSION}")
add_definitions("-DBROWNIE_PATCH_LEVEL=${${PROJECT_NAME}_PATCH_LEVEL}")

# set windows specific flags
if (MSVC)
        add_definitions("-D_SCL_SECURE_NO_WARNINGS")
        add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif (MSVC)

# set g++ specific flags
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_CXX_FLAGS "-Wno-deprecated -std=c++11")
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -Wall -pedantic -Wno-long-long -Wno-deprecated -Wall -Wextra -Wno-sign-compare")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g3")
        set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -Wall -pedantic -Wno-long-long")
endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)

# uncomment the portion below to disable assertions
if (CMAKE_BUILD_TYPE STREQUAL Release)
        add_definitions(-DNDEBUG)
else (CMAKE_BUILD_TYPE STREQUAL Release)
        add_definitions(-DDEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL Release)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(src)
