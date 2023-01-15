# SCI-BAT | HEMS powered smart battery controller #
*Project for [FK04/BENG/RETI](https://w3-mediapool.hm.edu/mediapool/media/fk04/fk04_lokal/_fk04/fwp1/projekte_1/projekte_im_wise_22_23/Projekt_RE_TI_WS_22_23.pdf) at [HM.EDU](https://ee.hm.edu/)*

## About the project
This project was created during my time at the Munich University of Applied Sciences. This is one part of the project (the software).  

Features:
* Connects to a Single phase SMA Battery inverert via Modbus
* Connects to a home energe managment system (HEMS) via MQTT
* Connects to a relais card via a serial interface (Provides cooling / heating of the inverter and battery space)
* Provideds a responsive web GUI for configuration and monitoring
* Converts MQTT <--> Modbus commands for controlling the battery

## Bulding the project
See [this](BUILD.md) for more information.
