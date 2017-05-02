# homie-dht
MQTT Humidity, Temperature and Air Preasure Sensor Node based on HOMIE. Support DHT11, DHT22, BME280, SHT30
## Features
* Configurable Sensor Type
* Configurable MQTT Publish Interval
* OTA Support (tested with jpmens/homie-ota)
* Generic Sensor Interface to ease implementation of further sensor types
## MQTT Topics
* temperature °C
* heat index °C
* relative humidtiy
* absolute humidity
* air preasure (if provided by the sensor)
* health state (sensor reading errors)
* homie internals
## Supported Sensors
* DHT11
* DHT22
* BME280 (planded)
* SHT30 (planed)
## Aplications
homie-dht volkszaehler integration (Node-RED)

https://gist.github.com/nuccleon/108d9656ee4375fc95cfc0e9cd1eab0b

