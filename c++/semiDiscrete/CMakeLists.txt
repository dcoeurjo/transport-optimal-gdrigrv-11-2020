PROJECT(semiDiscrete)


cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


##### Geogram cmake
include(cmake/CPM.cmake)
if(WIN32)
   set(VORPALINE_PLATFORM2 Win-vs-dynamic-generic)
elseif(APPLE)
   set(VORPALINE_PLATFORM2 Darwin-clang-dynamic)
elseif(UNIX)
   set(VORPALINE_PLATFORM2 Linux64-gcc-dynamic)
endif()
set(GEOGRAM_WITH_EXPLORAGRAM ON)
CPMAddPackage(
  NAME   geogram
  VERSION 1.7.6
  GITHUB_REPOSITORY alicevision/geogram
  OPTIONS
      "GEOGRAM_LIB_ONLY ON"
      "VORPALINE_PLATFORM ${VORPALINE_PLATFORM2}"
      "GEOGRAM_WITH_EXPLORAGRAM ON"
  )

include_directories(${geogram_SOURCE_DIR}/src/lib/)


add_executable(semiDiscrete semiDiscrete.cpp)
target_link_libraries(semiDiscrete PRIVATE geogram)
if(UNIX)
  find_package(Threads REQUIRED)
  target_link_libraries(${EXAMPLE} Threads::Threads)
endif()
