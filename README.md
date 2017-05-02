# homie-dht
MQTT Humidity, Temperature and Air Preasure Sensor Node based on HOMIE. Support DHT11, DHT22, BME280, SHT30

MQTT Topics 
* temperature °C
* heat index °C
* relative humidtiy
* absolute humidity
* air preasure (if provided by the sensor)
* health state (sensor reading errors)
* homie internals

Following Sensors are supported (or planed to be)
* DHT11
* DHT22
* BME280 (planded)
* SHT30 (planed)

Support of OTA. Tested with jpmens/homie-ota
