-- Simple webserver example
reti_new_project("SimpleWebServer", "src/example/SimpleWebServer")
reti_executable()
reti_cpp()
links { "SCIUtil", "NetTools" }

-- Generate a self signed certificate for debuging purpose
webserver_root = "%{wks.location}/etc/example-webserver-data"
debugdir(webserver_root)
