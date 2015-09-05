cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

add_library (multitude
  src/loader.cc
  src/matrix.cc
  include/block.h
  include/loader.h
  include/matrix.h
  include/ops.h
  include/thread_pool.h
)

install (DIRECTORY include/ DESTINATION include/multitude)

install (
  TARGETS multitude
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)
