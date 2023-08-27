file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.hpp)

find_package(Freetype REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

# TODO: uncomment this to stop the app from opening cmd on windows
# IF (WIN32)
#    add_executable(client WIN32 ${SOURCE_FILES} ${HEADER_FILES})
# ELSE()
   add_executable(client ${SOURCE_FILES} ${HEADER_FILES})
# ENDIF()
target_compile_definitions(client PUBLIC ${CLIENT_COMPILE_DEFINITIONS})

if(MSVC)
  target_compile_options(client PRIVATE /W4 /WX)
else()
  target_compile_options(client PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

target_link_libraries(client PRIVATE glfw)
target_link_libraries(client PRIVATE glad)
target_link_libraries(client PRIVATE glm::glm)
target_link_libraries(client PRIVATE Freetype::Freetype)

target_include_directories(client PRIVATE ${Stb_INCLUDE_DIR})
target_include_directories(client PRIVATE ${SIMPLEINI_INCLUDE_DIRS})
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/source/client)
