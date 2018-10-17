#ifndef _DHTxx_Impl_IMPL_
#define _DHTxx_Impl_IMPL_

#include "SensorInterface.hpp"
#include <DHT.h>

class DHTxx_Impl : public SensorInterface {
  public:
    enum DHTTYPE    {
      TYPE11 = DHT11,
      TYPE21 = DHT21,
      TYPE22 = DHT22
    };
    DHTxx_Impl(int pin, DHTTYPE type);
    virtual ~DHTxx_Impl();

    virtual float temperature();
    virtual float humidity();
    virtual SensorInterface::SensorState state();

  private:
    void readSensor();
    DHT dht;
    SensorInterface::SensorState ss;
    float h; // humidity in percent
    float t; // temperature as Celsius
};

inline DHTxx_Impl::DHTxx_Impl(int pin, DHTTYPE type)
  : dht(pin, type)
  , ss(SensorInterface::state())
  , t(SensorInterface::temperature())
  , h(SensorInterface::humidity()) {
  dht.begin();
}

inline DHTxx_Impl::~DHTxx_Impl() {

}

inline void DHTxx_Impl::readSensor() {
  static unsigned long last = 0;
  unsigned long now = millis();
  if((now - last) > 10000UL || (last == 0)) {
    float _t = dht.readTemperature();
    float _h = dht.readHumidity();
    // Check if any reads failed and keep old values (to try again).
    if (isnan(_h) || isnan(_t)) {
      ss = SensorInterface::read_error;
      Homie.getLogger() << "Failed to read DHT sensor!" << endl;
    } else {
      ss = SensorInterface::ok;
      t = _t;
      h = _h;
      last = now;
    }
  }
}

inline float DHTxx_Impl::temperature() {
  readSensor();
  return t;
}

inline float DHTxx_Impl::humidity() {
  readSensor();
  return h;
}

inline SensorInterface::SensorState DHTxx_Impl::state() {
  readSensor();
  return ss;
}

#endif
