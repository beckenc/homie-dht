#ifndef _BME280_IMPL_
#define _BME280_IMPL_

#include "SensorInterface.hpp"
#include <Wire.h>
#include <Adafruit_BME280.h>

class BME280_Impl : public SensorInterface {
  public:
    BME280_Impl();
    virtual ~BME280_Impl();

    virtual float temperature();
    virtual float humidity();
    virtual float preasure();
    virtual SensorInterface::SensorState state();
  private:
    void readSensor();
    Adafruit_BME280 bme;
    SensorInterface::SensorState ss;
    float h; // humidity in percent
    float t; // temperature as Celsius
    float p; // preasure in hPa
};

inline BME280_Impl::BME280_Impl()
  : bme()
  , ss(SensorInterface::state())
  , t(SensorInterface::temperature())
  , h(SensorInterface::humidity())
  , p(SensorInterface::preasure()) {
  if (!bme.begin(0x76)) {   // Set to 0x76 for alternative i2c addr
    Homie.getLogger() << "Couldn't find BME280" << endl;
  }
}

inline BME280_Impl::~BME280_Impl() {

}

inline void BME280_Impl::readSensor() {
  float _t = bme.readTemperature();
  float _h = bme.readHumidity();
  float _p = bme.readPressure() / 100.0F;
  // Check if any reads failed and keep old values (to try again).
  if (isnan(_h) || isnan(_t) || isnan(_p)) {
    ss = SensorInterface::read_error;
    Homie.getLogger() << "Failed to read BME280 sensor!" << endl;
  } else {
    ss = SensorInterface::ok;
    t = _t;
    h = _h;
    p = _p;
  }
}

inline float BME280_Impl::temperature() {
  readSensor();
  return t;
}

inline float BME280_Impl::humidity() {
  readSensor();
  return h;
}

inline float BME280_Impl::preasure() {
  readSensor();
  return p;
}

inline SensorInterface::SensorState BME280_Impl::state() {
  readSensor();
  return ss;
}


#endif
