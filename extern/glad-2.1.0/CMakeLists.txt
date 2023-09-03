add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/extern/glad-2.1.0/include)
