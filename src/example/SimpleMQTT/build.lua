-- Simple MQTT example
reti_new_project("SimpleMQTT", "src/example/SimpleMQTT")
reti_executable()
reti_cpp()
links { "SCIUtil", "NetTools" }

-- Generate a self signed certificate for debuging purpose
mqtt_root = "%{wks.location}/etc/example-mqtt-data"
debugdir(mqtt_root)
