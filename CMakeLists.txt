cmake_minimum_required (VERSION 3.20)


if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake )
	message(STATUS "Found ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake; using it!")
	set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
		CACHE STRING "Vcpkg toolchain file")
endif()

project (Soldat++)

find_package(GameNetworkingSockets CONFIG REQUIRED)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS On)

CPMAddPackage("gh:glfw/glfw#3.3.8")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage("gh:brofield/simpleini#v4.20")
CPMAddPackage("gh:gabime/spdlog#v1.13.0")
CPMAddPackage("gh:nothings/stb#master") # TODO: including master is not recommended
CPMAddPackage(
    NAME freetype
    GITHUB_REPOSITORY freetype/freetype
    GIT_TAG VER-2-13-2
    VERSION 2.13.2
    OPTIONS "FT_DISABLE_HARFBUZZ 1"
)
if (freetype_ADDED)
    add_library(Freetype::Freetype ALIAS freetype)
endif()

CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG v1.14.0
    VERSION 1.14.0
    OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
)

enable_testing()

add_subdirectory(extern)
add_subdirectory(source)
add_subdirectory(tests)
