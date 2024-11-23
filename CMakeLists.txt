#######
# AnD #
#######


# PROJECT PROPERTIES 

cmake_minimum_required(VERSION 3.15)
project(algorithms-and-data-structures VERSION 1.0.0)


# COMPILER PROPERTIES

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(GCC_LIKE_CXX "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(MSVC_CXX "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
add_compile_options(
    "$<$<BOOL:${GCC_LIKE_CXX}>:-Wall;-Wextra;-Wvla;-Wshadow;-Wformat=2;-Wunused>"
    "$<$<BOOL:${MSVC_CXX}>:-W3>"
)


# PATHS

include_directories(${CMAKE_SOURCE_DIR}/include/a-n-d/data-structures)
include_directories(${CMAKE_SOURCE_DIR}/include/a-n-d/algortihms)


# LIBRARIES

FILE (GLOB_RECURSE PROJECT_SRCS ./src/*.cc ./src/*.h)
FILE (GLOB_RECURSE PROJECT_INCLUDE ./include/a-n-d/*.h)
add_library(${PROJECT_NAME} STATIC ${PROJECT_SRCS} ${PROJECT_INCLUDE})


# SUBDIRECTORIES

add_subdirectory(src)
add_subdirectory(test)