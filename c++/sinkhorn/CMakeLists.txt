project(sinkhorn)

cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

set(SINKHORNS
  main
)

foreach(SINKHORN ${SINKHORNS})
    add_executable(${SINKHORN} ${SINKHORN}.cpp)
    if(UNIX)
      target_link_libraries(${SINKHORN} -lm)
    endif()
endforeach()
