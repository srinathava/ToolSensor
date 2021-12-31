# ToolSensor

This code is for sensing when a power tool is turned on/off using a combination of an ESP8266
and a [Modern Device current sensor](https://moderndevice.com/product/current-sensor/). The 
tool status is then transmitted via MQTT to a central gate coordinator which then opens/closes
the appropriate gates and turns on the dust collector.

This project is part of an overall set of projects mean to automate dust collection in my
woodworking workshop.

NOTE: You will create to create a file called `credentials.hpp` in the same location as 
`main.hpp` with the following contents:

```c
#define WIFI_SSID "MyNetWorkSSID"
#define WIFI_PASS "MyNetworkPassword"
#define MQTT_CONTROLLER_IP "192.168.4.1"
```

You will also need to modify the following line:

```c
const std::string TOOL_ID = "bandsaw";
const int NUM_READS = 20;
const float OFF_THRESHOLD = 50.0;
const float ON_THRESHOLD = 300.0;
```

to identify the tool you are interested in and the threshold for the current sensor reading. The code
takes an avergate of `NUM_READS` readings of the analog pin `A0` and compares it to the `ON_THRESHOLD`
and `OFF_THRESHOLD` values to identify when the tool in question has been turned on/off.
