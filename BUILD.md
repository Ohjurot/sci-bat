# Building RETI

### Required Software
- python3
- conan (pip install conan)
- CMake
- Visual Studio 2022 (Windows only) 

## Windows
1) **Generating the visual studio solution**: run `autogen generate-project` in this directory.
2) **Compiling the software**: Now open the generate visual studio solution and build the project from the GUI

## Linux
**IMPORTANT**: Please edit your default conan profile (`/home/<USER>/.conan/profiles/default`) and make sure you are using the new abi (`compiler.libcxx=libstdc++11`)
1) **Generating the visual studio solution**: run `autogen generate-project` in this directory.

