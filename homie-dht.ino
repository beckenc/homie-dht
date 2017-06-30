#include <Homie.h>
#include "Sensor.hpp"

static Sensor sensor;
static HomieSetting<long> publishIntervalSetting("publishInterval", "publish interval in seconds");

static void setupHandler() {
  sensor.setup();
}

static void loopHandler() {
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish >= (publishIntervalSetting.get() * 1000UL) || lastPublish == 0) {
    lastPublish = millis();
    if(!sensor.publish()) {
      lastPublish -= (publishIntervalSetting.get() * 1000UL);
      lastPublish += 2000UL;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;

  Homie_setFirmware("HomieDht", "0.1");
  Homie_setBrand("HomieDht"); // before Homie.setup()
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  Homie.disableLedFeedback();
  Homie.disableResetTrigger();

  publishIntervalSetting.setDefaultValue(30UL).setValidator([] (long candidate) {
    return (candidate >= 2) && (candidate < 86400); // 2sec - 1day
  });

  Homie.setup();
}

void loop() {
  Homie.loop();
}
