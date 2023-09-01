# Soldat++
C++23 implementation of [Soldat](https://github.com/soldat/soldat)

### Requirements
The project is written in C++23 and depends on:
- [OpenGL 2.1+](https://www.opengl.org/)
- [glad](https://glad.dav1d.de/)
- [glfw-3.3.8+](https://www.glfw.org/)
- [glm](https://github.com/g-truc/glm)
- [stb](https://github.com/nothings/stb)
- [FreeType](https://freetype.org/)
- [gtest 1.13.0+](https://github.com/google/googletest)

### Building with vcpkg
Make sure you have CMake installed.
To install all the required package using vcpkg:
```
vcpkg install glfw3 glm stb freetype simpleini gtest --triplet x64-windows
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<path/to/vcpkg/scripts/buildsystems/vcpkg.cmake> -DVCPKG_TARGET_TRIPLET=x64-windows
```

On linux:
```
sudo vcpkg install glfw3 glm stb freetype simpleini gtest --triplet x64-linux
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<path/to/vcpkg/scripts/buildsystems/vcpkg.cmake> -DVCPKG_TARGET_TRIPLET=x64-linux
```

You should now be able to build executables using `make` on linux or to open generated solution in visual studio on windows
