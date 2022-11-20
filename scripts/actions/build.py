# This action will generate the project and invoke a build

import subprocess
import sys
import platform

if __name__ == "__main__":
    # Os name
    osname = platform.system()

    # Action depends on os
    if osname ==  "Linux":
        subprocess.run(["python3", "./scripts/autogen.py", "generate-project", *sys.argv[1::]])
        subprocess.run(["make"])
    else:
        subprocess.run(["autogen.cmd", "generate-project", *sys.argv[1::]])
        # TODO: MS build
