# PATHFINDER ESP32

This is a simple project which utilises the ESP32 as an interface between a host an Intel DE1-SoC FPGA. The main application has been written specifically for the transmission of a graph object and its corresponding shortest path solution.

## WiFi

WiFi protocol is employed to communicate between Host and ESP32 wirelessly, provided they are on the same LAN.

The `WiFi` library provided the majority of the functions and classes required for bidirectional transmission of data.

## SPI

SPI protocol is utilised for communication beteween EPS32 and FPGA. Because the host and , subsequently, the ESP32 are giving commands to the FPGA,
the ESP32 assumes the role of SPI master and the FPGA that of SPI slave. The differentiating factor between master slave is that only the master drives the clock `SCLK` and chip select `CS`.

Class `Slave` was written as a wrapper for functions provided by the `SPI` library and it adds chip select logic for transmissions

## Serialization

Serialization of objects was achieved with the help of `ArduinoJson` library. Objects are encoded as a dictionary data structure, where variables are represented using key-value pairs. The dictionary can then be decoded at the other endpoint into an equivalent object, irrespective of programming language.

The ESP32 deserializes and serializes incoming strings to validate the contents of transmissions. The `Graph` class was developed to validate Host->FPGA transmissions. A corresponding class could be developed for FPGA->Host transmission as a means to validate shortest path response.