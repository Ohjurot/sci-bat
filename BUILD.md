# Building RETI

### Required Software
- python3
- conan (pip install conan)
- CMake
- Visual Studio 2022 (Windows only) 
- g++ with C++20 support (Linux only. Ubuntu comes with all requirements. Debian currently 01.2023 NOT)
- `uuid-dev` (Linux only)           

## Windows
1) **Generating the visual studio solution**: run `autogen generate-project` in this directory.
2) **Compiling the software**: Now open the generate visual studio solution and build the project from the GUI

## Linux
**IMPORTANT**: Please edit your default conan profile (`/home/<USER>/.conan/profiles/default`) and make sure you are using the new abi (`compiler.libcxx=libstdc++11`)
1) **Building the software**: run `./autogen.sh build` in this directory.
2) **Packaging into an debian archive**: run `./autogen.sh dpack` in this directory. This will automatically build the software in release mode and pack a debian package for distribution with all required files. The archive will consume the version defined in the `./VERSION` file.
