-- Simple webserver example
reti_new_project("SimpleWebServer", "src/example/SimpleWebServer")
reti_executable()
reti_cpp()
links { "SCIUtil", "NetTools" }

-- Generate a self signed certificate for debuging purpose
webserver_root = "%{wks.location}/etc/example-webserver-data"
debugdir(webserver_root)
postbuildcommands {
    "openssl req -config \"%{wks.location}/etc/openssl.cnf\" -x509 -newkey rsa:2048 " .. 
        "-keyout \"" .. webserver_root .. "/key.pem\" -out \"" .. webserver_root .. "/cert.pem\"" .. 
        "-sha256 -days 365 -nodes -subj /CN=localhost" 
}
