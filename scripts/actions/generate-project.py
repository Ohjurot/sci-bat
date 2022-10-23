# This action will generate the project

import subprocess
import sys
import platform

if __name__ == "__main__":
    # Os name
    osname = platform.system()

    # Config vars
    conanBuildType = "Debug"
    premakeAction = None

    # Check system
    if osname ==  "Linux":
        premakeAction = "gmake2"
    else:
        premakeAction = "vs2022"

    # Conan install dependencys
    print(f"Installing 3rd party dependency's...")
    subprocess.run(["conan", "install", ".", "--build", "missing", "-s", f"build_type={conanBuildType}"])
    # Invoke premake
    print(f"Generating project...")
    subprocess.run(["vendor/premake/premake5", f"{premakeAction}"])
