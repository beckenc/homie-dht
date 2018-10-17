# homie-dht
MQTT Humidity, Temperature and Air Preasure Sensor Node based on HOMIE. Support DHT11, DHT22, BME280, SHT30 and SHT31.
Optional use esp's deep sleep mode
## Features
* Configurable Sensor Type
* Configurable MQTT Publish Interval
* Configurable Deep Sleep
* Configurable threashold to force acyclic sensor reading / publishing (publishKey and publishValue)
* React on $broadcast/wakeup -> seconds, to prevent from going to deep sleep till seconds are gone
* OTA Support (tested with [jpmens/homie-ota](https://github.com/jpmens/homie-ota))
* Generic Sensor Interface to ease implementation of further sensor types

You can update the configuration on-the-fly by publishing incremental JSON updates to $implementation/config/set e.g. homie-dht/xxxxxxxx/$implementation/config/set -> {"settings":{"type":"sht31"}}
You can force a factory reset by publishing homie-dht/xxxxxxxx/$implementation/reset -> true
Keep the home-dht awake till seconds are gone, e.g. homie-dht/$broadcast/wakeup -> 5

## MQTT Topics
* temperature °C
* heat index °C
* dew point °C
* relative humidtiy
* absolute humidity
* air preasure (if provided by the sensor)
* health state (sensor reading errors)
* homie internals
## Supported Sensors
* DHT11
* DHT22
* BME280
* SHT30
* SHT31
## Dependencies
* [marvinroger/homie-esp8266](https://github.com/marvinroger/homie-esp8266)
* [marvinroger/async-mqtt-client](https://github.com/marvinroger/async-mqtt-client)
* [me-no-dev](https://github.com/me-no-dev/ESPAsyncWebServer)
* [me-no-dev/ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP)
* [bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson)
* [thomasfredericks/Bounce2](https://github.com/thomasfredericks/Bounce2)
* [adafruit/DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library.git)
* [adafruit/Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor.git)
* [adafruit/Adafruit_SHT31](https://github.com/adafruit/Adafruit_SHT31)
* [adafruit/Adafruit_BME280_Library](https://github.com/adafruit/Adafruit_BME280_Library)
## Aplications
homie-dht volkszaehler integration (Node-RED)

https://gist.github.com/nuccleon/108d9656ee4375fc95cfc0e9cd1eab0b

