cmake_minimum_required (VERSION 3.20)

if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake )
	message(STATUS "Found ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake; using it!")
	set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
		CACHE STRING "Vcpkg toolchain file")
endif()

project (Soldank++)

option(BUILD_SERVER_ENABLED "Build server" ON)
option(BUILD_CLIENT_ENABLED "Build client" ON)
option(BUILD_TESTS_ENABLED "Build all available tests" ON)

find_package(GameNetworkingSockets CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(PNG REQUIRED)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS On)

CPMAddPackage("gh:brofield/simpleini#v4.20")
CPMAddPackage("gh:gabime/spdlog#v1.13.0")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")

if (BUILD_CLIENT_ENABLED)
    CPMAddPackage("gh:glfw/glfw#3.3.8")
    CPMAddPackage("gh:nothings/stb#master") # TODO: including master is not recommended
    if (stb_ADDED)
        add_library(stb source/client/stb.cpp)
        target_include_directories(stb PUBLIC ${stb_SOURCE_DIR})
        add_library(stb::stb ALIAS stb)
    endif()
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
        NAME imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.90.1
        VERSION 1.90.1
        DOWNLOAD_ONLY YES
    )
    if (imgui_ADDED)
        file (GLOB imgui_sources ${imgui_SOURCE_DIR}/*.cpp)
        list (APPEND imgui_sources ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
        list (APPEND imgui_sources ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
        add_library(imgui STATIC ${imgui_sources})
        target_link_libraries(imgui PRIVATE glfw)
        target_include_directories(imgui SYSTEM PUBLIC $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>)
        add_library(imgui::imgui ALIAS imgui)
    endif()
endif()

if (BUILD_TESTS_ENABLED)
    CPMAddPackage(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG v1.14.0
        VERSION 1.14.0
        OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
    )
endif()

if (BUILD_SERVER_ENABLED)
CPMAddPackage(
    NAME dascript
    OPTIONS 
        "DAS_TOOLS_DISABLED ON"
        "DAS_TESTS_DISABLED ON"
        "DAS_PROFILE_DISABLED ON"
        "DAS_TUTORIAL_DISABLED ON"
    URL https://github.com/GaijinEntertainment/daScript/archive/9ae1c4cd73d7da06084aedb5741bb1da68b63b88.tar.gz
)
endif()

add_subdirectory(extern)
add_subdirectory(source)
if (BUILD_TESTS_ENABLED)
    enable_testing()
    add_subdirectory(tests)
endif()
