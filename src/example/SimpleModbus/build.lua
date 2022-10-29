-- Simple modbus demo project (used to comunicate with a SIEMENS logo to test modbus lib)
reti_new_project("SimpleModbus", "src/example/SimpleModbus")
reti_executable()
reti_cpp()
links { "SCIUtil", "NetTools", "ModbusMaster" }
