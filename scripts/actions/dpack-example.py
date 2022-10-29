# This action will generate the project and invoke a build

import subprocess
import platform
import shutil
import os

if __name__ == "__main__":
    # Os name
    osname = platform.system()

    # Do a release build
    if osname ==  "Linux":
        subprocess.run(["python3", "./scripts/autogen.py", "build", "Release"])
    else:
        subprocess.run(["autogen", "build", "Release"])

    # Make debian package
    if osname ==  "Linux":
        # Make package dir
        if not os.path.exists("./package"):
            os.mkdir("./package")
            

        # Get version of package
        version = open('VERSION', 'r').read().strip()
        print(f"Using version {version} from VERSION file.")

        # Assert new directory
        packageDir = f"./package/sci-bat-example_{version}_amd64"
        if os.path.exists(packageDir):
            shutil.rmtree(packageDir)
        os.mkdir(packageDir) 

        # Create infrastructure
        os.mkdir(f"{packageDir}/DEBIAN")
        controlFile = open(f"{packageDir}/DEBIAN/control","w+")
        controlFile.write(f"Package: sci-bat-example\n")
        controlFile.write(f"Version: {version}\n")
        controlFile.write(f"Architecture: all\n")
        controlFile.write(f"Essentials: no\n")
        controlFile.write(f"Priority: optional\n")
        controlFile.write(f"Depends: openssl\n")
        controlFile.write(f"Maintainer: Ludwig Fuechsl\n")
        controlFile.write(f"Description: Test package for reti with a webserver as service\n")
        controlFile.close()

        # Copy scripts
        shutil.copyfile("./etc/example-dep/postinst", f"{packageDir}/DEBIAN/postinst")
        os.chmod(f"{packageDir}/DEBIAN/postinst", 0o0775)

        # Copy all binary files
        os.mkdir(f"{packageDir}/usr")
        os.mkdir(f"{packageDir}/usr/bin")
        shutil.copyfile("./build/x86_64-release/bin/SimpleWebServer", f"{packageDir}/usr/bin/sci-example-webserver")
        shutil.copyfile("./build/x86_64-release/bin/SimpleMQTT", f"{packageDir}/usr/bin/sci-example-mqtt")
        shutil.copyfile("./build/x86_64-release/bin/SimpleModbus", f"{packageDir}/usr/bin/sci-example-modbus")

        # Create config dirs
        os.mkdir(f"{packageDir}/etc")
        os.mkdir(f"{packageDir}/etc/sci-example")
        shutil.copyfile("./etc/example-webserver-data/config-linux.xml", f"{packageDir}/etc/sci-example/webserver-config.xml")
        shutil.copyfile("./etc/example-mqtt-data/settings.xml", f"{packageDir}/etc/sci-example/mqtt-config.xml")

        # Build package
        subprocess.run(["dpkg-deb", "--build", "--root-owner-group", packageDir])
