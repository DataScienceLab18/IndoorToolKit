CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

project (Simulator)

OPTION(BUILD_TEST "Build SimSystem test cases." OFF)

SET (CMAKE_CXX_FLAGS "-frounding-math -Wall -std=c++11 -O3 -fopenmp")
SET (CMAKE_CXX_FLAGS_DEBUG "-frounding-math -Wall -std=c++11 -g -O0")
SET (CMAKE_CXX_FLAGS_RELEASE "-frounding-math -Wall -std=c++11 -O3 -fopenmp")

FIND_PACKAGE(Boost 1.53)
FIND_PACKAGE(CGAL)

ADD_SUBDIRECTORY (src)

if (BUILD_TEST)
  enable_testing()
  FIND_PACKAGE(GTest REQUIRED)
  ADD_SUBDIRECTORY(test)
endif()
