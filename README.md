# Soldat++
C++23 implementation of [Soldat](https://github.com/soldat/soldat)

### Requirements
The project is written in C++23 and depends on:
- [OpenGL 2.1+](https://www.opengl.org/)
- [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake)
- [glad](https://glad.dav1d.de/)
- [glfw-3.3.8+](https://www.glfw.org/)
- [glm](https://github.com/g-truc/glm)
- [stb](https://github.com/nothings/stb)
- [FreeType](https://freetype.org/)
- [gtest 1.13.0+](https://github.com/google/googletest)

### Building
Make sure you have CMake installed. The project uses [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) to download required packages.
To install all the required package using:
```
mkdir build
cd build
cmake ..
```
You should now be able to build executables using `make` on linux or to open generated solution in visual studio on windows
