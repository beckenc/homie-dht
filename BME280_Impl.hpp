#ifndef _BME280_IMPL_
#define _BME280_IMPL_

#include "SensorInterface.hpp"

class BME280_Impl : public SensorInterface {
public:
   BME280_Impl();
   virtual ~BME280_Impl();

   virtual float temperature();
   virtual float humidity();
   virtual float preasure();
   virtual SensorInterface::SensorState state();
};

inline BME280_Impl::BME280_Impl() {
  
}

inline BME280_Impl::~BME280_Impl() {
  
}

inline float BME280_Impl::temperature() {
  return SensorInterface::temperature();
}

inline float BME280_Impl::humidity() {
  return SensorInterface::humidity();
}

inline float BME280_Impl::preasure() {
  return SensorInterface::preasure();
}

inline SensorInterface::SensorState BME280_Impl::state() {
   return SensorInterface::state();
}


#endif
