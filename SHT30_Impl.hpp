#ifndef _SHT30_IMPL_
#define _SHT30_IMPL_

#include "SensorInterface.hpp"
#include "Adafruit_SHT31.h"

class SHT30_Impl : public SensorInterface {
  public:
    SHT30_Impl();
    virtual ~SHT30_Impl();

    virtual float temperature();
    virtual float humidity();
    virtual SensorInterface::SensorState state();
  private:
    void readSensor();
    Adafruit_SHT31 sht30;
    SensorInterface::SensorState ss;
    float h; // humidity in percent
    float t; // temperature as Celsius
};

inline SHT30_Impl::SHT30_Impl()
  : sht30()
  , ss(SensorInterface::state())
  , t(SensorInterface::temperature())
  , h(SensorInterface::humidity()) {
  if (! sht30.begin(0x45)) {   // Set to 0x45 for alternate i2c addr
    Homie.getLogger() << "Couldn't find SHT30" << endl;
  }
}

inline SHT30_Impl::~SHT30_Impl() {

}

inline void SHT30_Impl::readSensor() {
  float _t = sht30.readTemperature();
  float _h = sht30.readHumidity();
  // Check if any reads failed and keep old values (to try again).
  if (isnan(_h) || isnan(_t)) {
    ss = SensorInterface::read_error;
    Homie.getLogger() << "Failed to read SHT30 sensor!" << endl;
  } else {
    ss = SensorInterface::ok;
    t = _t;
    h = _h;
  }
}

inline float SHT30_Impl::temperature() {
  readSensor();
  return t;
}

inline float SHT30_Impl::humidity() {
  readSensor();
  return h;
}

inline SensorInterface::SensorState SHT30_Impl::state() {
  readSensor();
  return ss;
}

#endif

