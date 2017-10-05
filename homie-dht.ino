#include <Homie.h>
#include "Sensor.hpp"

//#define _DEBUG

static Sensor sensor;
static HomieSetting<long> publishIntervalSetting("publishInterval", "publish interval in seconds");
static HomieSetting<bool> deepSleepSetting("deepSleep", "ESP deep sleep mode");

static void setupHandler() {
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
      if (deepSleep) {
        // publishing successful. Go into deep sleep.
#ifndef _DEBUG
        Homie.getLogger() << "Preparing for deep sleep (" << publishInterval << " seconds)" << endl;
        Homie.prepareToSleep();
#endif
      }
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
  // Connect D0 to RST to wake up
  pinMode(D0, WAKEUP_PULLUP);

  Serial.begin(115200);
  Serial << endl << endl;

  Homie_setFirmware("homie-dht", "0.0.2");
  Homie_setBrand("homie-dht");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler).onEvent(onHomieEvent);
  Homie.disableLedFeedback();
  Homie.disableResetTrigger();

  deepSleepSetting.setDefaultValue(false);
  publishIntervalSetting.setDefaultValue(300UL).setValidator([] (long candidate) {
    return (candidate >= 2) && (candidate < 86400); // 2sec - 1day
  });

  Homie.setup();
}

void loop() {
  Homie.loop();
}
