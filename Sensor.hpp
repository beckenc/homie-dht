#ifndef _SENSOR_NODE_
#define _SENSOR_NODE_

#include <Homie.h>
#include <limits>
#include "BME280_Impl.hpp"
#include "DHTxx_Impl.hpp"
#include "SHT3x_Impl.hpp"

class Sensor {
  public:
    Sensor();
    virtual ~Sensor();

    void setup();
    bool publish();
    bool force();

  private:
    // taken from adafruit DHT library
    float computeHeatIndex(float temperature, float relativeHumidity) const;
    // Sättigungsdampfdruck in hPa
    double SDD(float temperature) const;
    // Dampfdruck in hPa
    double DD(float temperature, float relativeHumidity) const;
    // Taupunkt in °C
    float TD(float temperature, float relativeHumidity) const;
    // absolute Feuchte in g Wasserdampf pro m3 Luft
    float AF(float temperature, float relativeHumidity) const;
    // check and update the health state. publish health state if it has changed.
    void checkHealth(bool publish);

    SensorInterface *sensor;
    SensorInterface::SensorState ss;
    unsigned long errors;
    static const constexpr char* dht11 = "dht11";
    static const constexpr char* dht21 = "dht21";
    static const constexpr char* dht22 = "dht22";
    static const constexpr char* bme280 = "bme280";
    static const constexpr char* sht30 = "sht30";
    static const constexpr char* sht31 = "sht31";
    static const constexpr char* temperature = "temperature";
    static const constexpr char* pressure = "pressure";
    static const constexpr char* humidity = "humidity";
    static const constexpr char* healthState = "healthState";
    HomieSetting<const char*> typeSetting;
    HomieSetting<const char*> forceKeySetting;
    HomieSetting<double> forceValSetting;
    float lastTemperature;
    float lastHumidity;
    float lastPressure;
    HomieNode *sensorStateNode;
    HomieNode *temperatureNode;
    HomieNode *humidityNode;
    HomieNode *pressureNode;
};

inline Sensor::Sensor()
  : sensor(NULL)
  , ss(SensorInterface::unknown)
  , errors(0)
  , typeSetting("type", "Type of the sensor. Use either  \"dht11\", \"dht21\", \"dht22\", \"bme280\", \"sht30\" or \"sht31\"")
  , forceKeySetting("force key", "The key of the force value. Use either \"temperature\", \"pressure\" or \"humidity\"")
  , forceValSetting("force value", "The value for the force key. E.g. Key / Value of 0.5 and \"temperature\" means publishing on 0.5 degress temperature difference.")
  , lastTemperature(std::numeric_limits<float>::min())
  , lastHumidity(std::numeric_limits<float>::min())
  , lastPressure(std::numeric_limits<float>::min())  
  , sensorStateNode(NULL)
  , temperatureNode(NULL)
  , humidityNode(NULL)
  , pressureNode(NULL) {
  typeSetting.setDefaultValue(sht31).setValidator([] (const char* candidate) {
    return (String(candidate) == dht11) ||
           (String(candidate) == dht21) ||
           (String(candidate) == dht22) ||
           (String(candidate) == bme280) ||
           (String(candidate) == sht30) ||
           (String(candidate) == sht31);
  });
  forceKeySetting.setDefaultValue("temperature").setValidator([] (const char* candidate) {
    return (String(candidate) == temperature) ||
           (String(candidate) == humidity) ||
           (String(candidate) == pressure);
    });
  forceValSetting.setDefaultValue(0.2);
  sensorStateNode = new HomieNode(healthState, healthState);
  temperatureNode = new HomieNode(temperature, temperature);
  humidityNode = new HomieNode(humidity, humidity);
  pressureNode = new HomieNode(pressure, pressure);

  sensorStateNode->advertise("health");
  sensorStateNode->advertise("errors");
  temperatureNode->advertise("absolut");
  temperatureNode->advertise("heatindex");
  temperatureNode->advertise("dewPoint");
  humidityNode->advertise("relative");
  humidityNode->advertise("absolute");
  pressureNode->advertise("pressure");
}

inline Sensor::~Sensor() {
  delete pressureNode;
  delete humidityNode;
  delete temperatureNode;
  delete sensorStateNode;
  delete sensor;
}

inline float Sensor::computeHeatIndex(float temperature, float percentHumidity) const {
  // Using both Rothfusz and Steadman's equations
  // http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml


  // to fahrenheit
  temperature = temperature * 1.8 + 32;

  float hi = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (percentHumidity * 0.094));

  if (hi > 79) {
    hi = -42.379 +
          2.04901523 * temperature +
         10.14333127 * percentHumidity +
         -0.22475541 * temperature * percentHumidity +
         -0.00683783 * pow(temperature, 2) +
         -0.05481717 * pow(percentHumidity, 2) +
          0.00122874 * pow(temperature, 2) * percentHumidity +
          0.00085282 * temperature * pow(percentHumidity, 2) +
         -0.00000199 * pow(temperature, 2) * pow(percentHumidity, 2);

    if ((percentHumidity < 13) && (temperature >= 80.0) && (temperature <= 112.0))
      hi -= ((13.0 - percentHumidity) * 0.25) * sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);

    else if ((percentHumidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0))
      hi += ((percentHumidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);
  }

  hi = (hi - 32) * 0.55555;
  return hi;
}

inline double Sensor::SDD(float temperature) const {
  float a = (temperature >= 0) ? 7.5 : 7.6;
  float b = (temperature >= 0) ? 237.3 : 240.7;
  float exponent = ((a * temperature) / (b + temperature));
  float sdd = 6.1078 * pow(10, exponent);
  return sdd;
}

inline double Sensor::DD(float temperature, float relativeHumidity) const {
  double dd = relativeHumidity / 100 * SDD(temperature);
  return dd;
}

inline float Sensor::TD(float temperature, float relativeHumidity) const {
  float a = (temperature >= 0) ? 7.5 : 7.6;
  float b = (temperature >= 0) ? 237.3 : 240.7;
  float v = log10(DD(temperature, relativeHumidity) / 6.1078);
  float td = b * v / (a - v);
  return td;
}

inline float Sensor::AF(float temperature, float relativeHumidity) const {
  double dd = DD(temperature, relativeHumidity);
  float af = 216.686912909 * dd / (temperature + 273.15);
  return af;
}

inline void Sensor::checkHealth(bool publish) {
  SensorInterface::SensorState state = SensorInterface::unknown;
  if(sensor) {
    state = sensor->state();
  }
  String health = [](SensorInterface::SensorState state) {
    if (state == SensorInterface::ok)
      return "ok";
    if (state == SensorInterface::read_error)
      return "read_error";
    if (state == SensorInterface::connect_error)
      return "connect_error";
    return "unknown";
  }(state);

  if (state != ss) {
    ss = state;
  }
  if (ss != SensorInterface::ok) {
    Homie.getLogger() << "Sensor health check failed: " << health << endl;
    errors++;
  }
  if(publish) {
    // todo: keep errors and health persistant
    sensorStateNode->setProperty("errors").setRetained(false).send(String(errors));
    sensorStateNode->setProperty("health").setRetained(false).send(health);
  }
}

inline void Sensor::setup() {
  String type(typeSetting.get());
  if (type == dht11) {
    sensor = new DHTxx_Impl(2, DHTxx_Impl::TYPE11);
  } else if (type == dht21) {
    sensor = new DHTxx_Impl(2, DHTxx_Impl::TYPE21);
  } else if (type == dht22) {
    sensor = new DHTxx_Impl(2, DHTxx_Impl::TYPE22);
  } else if (type == bme280) {
    sensor = new BME280_Impl(0x76);
  } else if (type == sht30) {
    sensor = new SHT3x_Impl(0x45);
  } else if (type == sht31) {
    sensor = new SHT3x_Impl(0x44);
  } else {
    sensor = new SensorInterface();
  }
}

inline bool Sensor::force() {
  bool force = false;
  checkHealth(false);
  if (ss != SensorInterface::ok) {
    return force;
  }
  
  String forceKey(forceKeySetting.get());
  if (forceKey == temperature) {
    force = (abs(lastTemperature - sensor->temperature()) > forceValSetting.get());
  } else if (forceKey == humidity) {
    force = (abs(lastHumidity - sensor->humidity()) > forceValSetting.get());
  } else if (forceKey == pressure) {
    force = (abs(lastPressure - sensor->pressure()) > forceValSetting.get());
  }
  return force;
}

inline bool Sensor::publish() {
  checkHealth(true);
  if (ss != SensorInterface::ok) {
    return false;
  }

  float temperature = sensor->temperature();
  float humidity = sensor->humidity();
  float pressure = sensor->pressure();

  String t(temperature);
  String h(humidity);
  String p(pressure);
  String hi(computeHeatIndex(temperature, humidity));
  String sdd(SDD(temperature));
  String dd(DD(temperature, humidity));
  String td(TD(temperature, humidity));
  String af(AF(temperature, humidity));

  Homie.getLogger() << "Sensor reading: " << endl <<
                    "  • errors                : " << errors << endl <<
                    "  • temperature           : " << t      << " °C"   << endl <<
                    "  • pressure              : " << p      << " hPa"  << endl <<
                    "  • relative humidity     : " << h      << " %"    << endl <<
                    "  • heat index:           : " << hi     << " °C"   << endl <<
                    "  • dew point temperature : " << td     << " °C"   << endl <<
                    "  • absolute humidty      : " << af     << " g/m³" << endl;

  temperatureNode->setProperty("absolute").setRetained(true).send(t);
  temperatureNode->setProperty("heatindex").setRetained(true).send(hi);
  temperatureNode->setProperty("dewPoint").setRetained(true).send(td);
  humidityNode->setProperty("relative").setRetained(true).send(h);
  humidityNode->setProperty("absolute").setRetained(true).send(af);
  pressureNode->setProperty("pressure").setRetained(true).send(p);

  lastTemperature = temperature;
  lastHumidity = humidity;
  lastPressure = pressure;
  
  return true;
}

#endif
