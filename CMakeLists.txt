cmake_minimum_required (VERSION 3.20)
project (Soldat++)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS On)

enable_testing()

add_subdirectory(extern)
add_subdirectory(source)
add_subdirectory(tests)