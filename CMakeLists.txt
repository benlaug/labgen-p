#
# Copyright - Benjamin Laugraud - 2014
#

# CMake parameter(s).
cmake_minimum_required(VERSION 2.8)

# Disable RPATH support for OS X.
if    (${CMAKE_MAJOR_VERSION} EQUAL 3 AND APPLE)
  cmake_policy(SET CMP0042 OLD)
endif (${CMAKE_MAJOR_VERSION} EQUAL 3 AND APPLE)

# Project name.
project(LaBGen-P)

# C++ flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Boost.
find_package(Boost REQUIRED program_options system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# OpenCV.
find_package(OpenCV REQUIRED core highgui video legacy)
include_directories(SYSTEM ${OpenCV_INCLUDE_DIR})

# Executables.
file(GLOB SRC_FILES *.h *.cpp)
add_executable(LaBGen-P ${SRC_FILES})
target_link_libraries(LaBGen-P ${Boost_LIBRARIES} ${OpenCV_LIBS})