file(GLOB_RECURSE LIB_SOURCES *.cpp)
file(GLOB_RECURSE HEADER_SOURCES *.hpp)
list(REMOVE_ITEM LIB_SOURCES main.cpp)


add_library(server_lib ${LIB_SOURCES} ${HEADER_SOURCES})

target_compile_definitions(server_lib PUBLIC ${SERVER_COMPILE_DEFINITIONS})

if(MSVC)
  target_compile_options(server_lib PRIVATE /Wall)
else()
  target_compile_options(server_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_include_directories(server_lib PUBLIC ${CMAKE_SOURCE_DIR}/source/server)

target_link_libraries(server_lib PUBLIC shared_lib)
target_link_libraries(server_lib PRIVATE GameNetworkingSockets::static)
target_link_libraries(server_lib PRIVATE spdlog)

add_executable(server main.cpp)
target_compile_definitions(server PUBLIC ${SERVER_COMPILE_DEFINITIONS})

if(MSVC)
  target_compile_options(server PRIVATE /Wall)
  set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
else()
  target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic)
  set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
endif()

target_link_libraries(server PRIVATE server_lib)
target_link_libraries(server PRIVATE GameNetworkingSockets::static)
target_link_libraries(server PRIVATE spdlog)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/source/server)
