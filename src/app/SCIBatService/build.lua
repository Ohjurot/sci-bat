-- Simple modbus demo project (used to comunicate with a SIEMENS logo to test modbus lib)
reti_new_project("SCIBatService", "src/app/SCIBatService")
reti_executable()
reti_cpp()
links { "SCIUtil", "NetTools", "ModbusMaster" }

webserver_root = "%{wks.location}/etc/sci-bat-app-dir"
targetname("sci-bat-service")
debugdir(webserver_root)
debugargs { "-a", "./app", "-c", "./conf", "--trace" }
