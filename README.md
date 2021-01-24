# Doorbell Hack Indoor Unit (Homekit)

## Instruction

To hack my analog doorbell (MT670C-CK2) I grab V+ and GND (0-15V) and meassure the voltage.
When somebody is pressing the doorbell the Voltage will be around 13.5V. The ESP8266 will send an MQTT message to my homebridge plugin which trigger the Homekit Doorbell.

## Build Source

A Config.h is needed needed

```Arduino
#ifndef CONFIG_H
#define CONFIG_H

#define WIFI*SSID "wifi name"
#define WIFI_PW "wifi password"
#define MQTT_BROKER "mqttbroker adress"

#endif
```
