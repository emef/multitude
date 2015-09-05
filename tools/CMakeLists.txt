cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

add_executable (mformat mformat.cc)

install(
  TARGETS mformat
  DESTINATION bin
)
