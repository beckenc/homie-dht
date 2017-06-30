#ifndef _SHT3x_Impl_
#define _SHT3x_Impl_
 
#include "SensorInterface.hpp"
#include "Adafruit_SHT31.h"

class SHT3x_Impl : public SensorInterface {
  public:
    SHT3x_Impl(uint8_t i2caddr);
    virtual ~SHT3x_Impl();

    virtual float temperature();
    virtual float humidity();
    virtual SensorInterface::SensorState state();
  private:
    void readSensor();
    Adafruit_SHT31 sht3x;
    SensorInterface::SensorState ss;
    float h; // humidity in percent
    float t; // temperature as Celsius
};

inline SHT3x_Impl::SHT3x_Impl(uint8_t i2caddr)
  : sht3x()
  , ss(SensorInterface::state())
  , t(SensorInterface::temperature())
  , h(SensorInterface::humidity()) {
  if (! sht3x.begin(i2caddr)) {
    Homie.getLogger() << "Couldn't find SHT" << endl;
    ss = connect_error;
  }
}

inline SHT3x_Impl::~SHT3x_Impl() {

}

inline void SHT3x_Impl::readSensor() {
  if(ss == connect_error)
     return;
  float _t = sht3x.readTemperature();
  float _h = sht3x.readHumidity();
  // Check if any reads failed and keep old values (to try again).
  if (isnan(_h) || isnan(_t)) {
    ss = SensorInterface::read_error;
    Homie.getLogger() << "Failed to read SHT sensor!" << endl;
  } else {
    ss = SensorInterface::ok;
    t = _t;
    h = _h;
  }
}

inline float SHT3x_Impl::temperature() {
  readSensor();
  return t;
}

inline float SHT3x_Impl::humidity() {
  readSensor();
  return h;
}

inline SensorInterface::SensorState SHT3x_Impl::state() {
  readSensor();
  return ss;
}

#endif

