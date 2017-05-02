#ifndef _SENSOR_INTERFACE_
#define _SENSOR_INTERFACE_

class SensorInterface {
public:
  enum SensorState {
     unknown = -1,
     ok = 0,
     read_error = 1
  };
  virtual float temperature() { return -1; }
  virtual float humidity() { return -1; }
  virtual float preasure() { return -1; }
  virtual SensorState state() { return unknown; }
};

#endif
