#ifndef _SHT30_IMPL_
#define _SHT30_IMPL_

#include "SensorInterface.hpp"

class SHT30_Impl : public SensorInterface {
public:
   SHT30_Impl();
   virtual ~SHT30_Impl();

   virtual float temperature();
   virtual float humidity();
   virtual SensorInterface::SensorState state();
};

inline SHT30_Impl::SHT30_Impl() {
  
}

inline SHT30_Impl::~SHT30_Impl() {
  
}

inline float SHT30_Impl::temperature() {
   return SensorInterface::temperature();  
}

inline float SHT30_Impl::humidity() {
   return SensorInterface::humidity();  
}

inline SensorInterface::SensorState SHT30_Impl::state() {
   return SensorInterface::state();
}

#endif

