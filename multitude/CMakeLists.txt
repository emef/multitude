cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

# --------------------------------------------------
# library
add_library (multitude
  src/loader.cc
  src/matrix.cc
  include/block.h
  include/loader.h
  include/matrix.h
  include/ops.h
  include/thread_pool.h
)

# --------------------------------------------------
# docs
find_package(Doxygen)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  @ONLY
)

add_custom_target(
  docs
  ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# --------------------------------------------------
# install
install (DIRECTORY include/ DESTINATION include/multitude)

install (
  TARGETS multitude
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)
