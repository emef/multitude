cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories (${PROJECT_SOURCE_DIR}/multitude)
link_directories (${PROJECT_SOURCE_DIR}/multitude)

add_executable (example example.cc)
target_link_libraries (example multitude)
