# This action will generate a linux .deb package (After invoking a full Release build)

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
        packageDir = f"./package/sci-bat_{version}_amd64"
        if os.path.exists(packageDir):
            shutil.rmtree(packageDir)
        os.mkdir(packageDir) 

        # Create infrastructure
        os.mkdir(f"{packageDir}/DEBIAN")
        controlFile = open(f"{packageDir}/DEBIAN/control","w+")
        controlFile.write(f"Package: sci-bat\n")
        controlFile.write(f"Version: {version}\n")
        controlFile.write(f"Architecture: amd64\n")
        controlFile.write(f"Essentials: no\n")
        controlFile.write(f"Priority: optional\n")
        controlFile.write(f"Depends: openssl\n")
        controlFile.write(f"Maintainer: Ludwig Fuechsl\n")
        controlFile.write(f"Description: Modbus n MQTT gateway for SBC inverter\n")
        controlFile.write(f"Installed-Size: {9.7 * 1024 * 1024}\n")
        controlFile.close()

        # Copy scripts
        shutil.copyfile("./etc/dep/postinst", f"{packageDir}/DEBIAN/postinst")
        os.chmod(f"{packageDir}/DEBIAN/postinst", 0o0775)
        shutil.copyfile("./etc/dep/prerm", f"{packageDir}/DEBIAN/prerm")
        os.chmod(f"{packageDir}/DEBIAN/prerm", 0o0775)
        shutil.copyfile("./etc/dep/postrm", f"{packageDir}/DEBIAN/postrm")
        os.chmod(f"{packageDir}/DEBIAN/postrm", 0o0775)

        # Copy all binary files
        os.mkdir(f"{packageDir}/usr")
        os.mkdir(f"{packageDir}/usr/bin")
        shutil.copyfile("./build/x86_64-release/bin/sci-bat-service", f"{packageDir}/usr/bin/sci-bat")

        # Copy webserver data
        os.mkdir(f"{packageDir}/usr/bin/sci-bat.d/")
        os.mkdir(f"{packageDir}/usr/bin/sci-bat.d/webserver")
        shutil.copytree("./etc/sci-bat-app-dir/app/webserver/www-data", f"{packageDir}/usr/bin/sci-bat.d/webserver/www-data")
        shutil.copytree("./etc/sci-bat-app-dir/app/webserver/templates", f"{packageDir}/usr/bin/sci-bat.d/webserver/templates")

        # Create config dirs
        os.mkdir(f"{packageDir}/etc")
        os.mkdir(f"{packageDir}/etc/sci-bat")
        shutil.copyfile("./etc/sci-bat-app-dir/conf/webserver-conf-linux.xml", f"{packageDir}/etc/sci-bat/webserver-conf.xml")

        #  Service 
        os.mkdir(f"{packageDir}/etc/systemd")
        os.mkdir(f"{packageDir}/etc/systemd/system")
        shutil.copyfile("./etc/dep/sci-bat.service", f"{packageDir}/etc/systemd/system/sci-bat.service")

        # Build package
        subprocess.run(["dpkg-deb", "--build", "--root-owner-group", packageDir])
