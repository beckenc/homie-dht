#ifndef _BME280_IMPL_
#define _BME280_IMPL_

#include "SensorInterface.hpp"
#include <Wire.h>
#include <Adafruit_BME280.h>

class BME280_Impl : public SensorInterface {
  public:
    BME280_Impl(uint8_t i2caddr);
    virtual ~BME280_Impl();

    virtual float temperature();
    virtual float humidity();
    virtual float pressure();
    virtual SensorInterface::SensorState state();
  private:
    void readSensor();
    Adafruit_BME280 bme;
    SensorInterface::SensorState ss;
    float h; // humidity in percent
    float t; // temperature as Celsius
    float p; // pressure in hPa
};

inline BME280_Impl::BME280_Impl(uint8_t i2caddr)
  : bme()
  , ss(SensorInterface::state())
  , t(SensorInterface::temperature())
  , h(SensorInterface::humidity())
  , p(SensorInterface::pressure()) {
  if (!bme.begin(i2caddr)) {
    Homie.getLogger() << "Couldn't find BME280" << endl;
    ss = connect_error;
  }
  else
  {
    // weather monitoring
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );
  }
}

inline BME280_Impl::~BME280_Impl() {

}

inline void BME280_Impl::readSensor() {
  if(ss == connect_error)
    return;

  static unsigned long last = 0;
  unsigned long now = millis();
  if((now - last) > 60000UL || (last == 0)) { // suggested: 1 reading per minute
  
    // Only needed in forced mode! In normal mode, you can remove the next line.
    bme.takeForcedMeasurement(); // has no effect in normal mode
    
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
      last = now;
    }    
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

inline float BME280_Impl::pressure() {
  readSensor();
  return p;
}

inline SensorInterface::SensorState BME280_Impl::state() {
  readSensor();
  return ss;
}


#endif
