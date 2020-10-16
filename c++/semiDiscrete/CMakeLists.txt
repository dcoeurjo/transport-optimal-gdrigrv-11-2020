PROJECT(semiDiscrete)

cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 11)

set(EXAMPLES
  semiDiscrete 
)

foreach(EXAMPLE ${EXAMPLES})
  add_executable(${EXAMPLE} ${EXAMPLE}.cpp Delaunay_psm.cpp)
    if(UNIX)
      target_link_libraries(${EXAMPLE} -lm)
    endif()
endforeach()
