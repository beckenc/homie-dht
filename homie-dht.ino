#include <Homie.h>
#include "Sensor.hpp"

#define _DEBUG

static Sensor sensor;
static HomieSetting<long> publishIntervalSetting("publishInterval", "publish interval in seconds");
static HomieSetting<bool> deepSleepSetting("deepSleep", "true for using ESP deep sleep mode or false for not");

static void setupHandler() {
  publishIntervalSetting.setDefaultValue(60UL).setValidator([] (long candidate) {
    return (candidate >= 2) && (candidate < 86400); // 2sec - 1day
  });
  deepSleepSetting.setDefaultValue(true);

  sensor.setup();
}

static void loopHandler() {
  static unsigned long lastPublish = 0;
  bool deepSleep = deepSleepSetting.get();
  long publishInterval = publishIntervalSetting.get();

  if (millis() - lastPublish >= (publishInterval * 1000UL) || lastPublish == 0) {
    lastPublish = millis();
    if (!sensor.publish()) {
      lastPublish -= (publishInterval * 1000UL); // reading failed, give it another try in 2 seconds
      lastPublish += 2000UL;
    } else {
      Homie.getLogger() << "Preparing for deep sleep (" << publishInterval << " seconds)" << endl;
#ifndef _DEBUG
      Homie.prepareToSleep();
#endif
    }
  }
}

void onHomieEvent(const HomieEvent& event) {
  switch (event.type) {
    case HomieEventType::READY_TO_SLEEP:
      Homie.getLogger() << "Ready to sleep" << endl;
      // convert to microseconds
      ESP.deepSleep(publishIntervalSetting.get() * 1000000);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;

  Homie_setFirmware("HomieDht", "0.2");
  Homie_setBrand("HomieDht"); // before Homie.setup()
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler).onEvent(onHomieEvent);
#ifndef _DEBUG  
  Homie.disableLogging();
#endif
  Homie.disableLedFeedback();
  Homie.disableResetTrigger();

  Homie.setup();
}

void loop() {
  Homie.loop();
}
