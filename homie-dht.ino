#include <Homie.h>
#include "Sensor.hpp"

//#define _DEBUG

static Sensor sensor;
static HomieSetting<long> publishIntervalSetting("publishInterval", "publish interval in seconds");
static HomieSetting<bool> deepSleepSetting("deepSleep", "ESP deep sleep mode");
static unsigned long awakeTillMillis = 0;

static void setupHandler() {
  Homie.getLogger() << "homie-dht - " << __DATE__ << " - " << __TIME__ << endl;
  sensor.setup();
}

bool broadcastHandler(const String& level, const String& value) {
  if(level == "wakeup") {
    long awakeTillSec = value.toInt();
    if(0 != awakeTillSec) {
      awakeTillMillis = millis() + (awakeTillSec * 1000UL);
      Homie.getLogger() << "awake for the next " << awakeTillSec << " seconds" << endl;
    }
    return true;
  }
  return false;
}

static void loopHandler() {
  static unsigned long lastPublish = 0;
  unsigned long now = millis();
  bool deepSleep = deepSleepSetting.get();
  unsigned long publishInterval = publishIntervalSetting.get() * 1000UL;

  ESP.wdtDisable(); // disabling the software watchdog timer will cause the
                    // the HW watchdog to trigger after 6 seconds.
  if (now - lastPublish >= publishInterval || lastPublish == 0 || sensor.force()) {
    if (sensor.publish()) {
      lastPublish = now;
    } else {
      lastPublish += 2000UL;  // reading failed, give it another try in 2 seconds
      deepSleep = false;      // skip deep sleep
    }
  }
  ESP.wdtEnable(1000);
    
  if (deepSleep) {
    // publishing successful. Go into deep sleep.
    if(now >= awakeTillMillis) {
#ifndef _DEBUG
      static bool prepared = false;
      if(!prepared) {
        prepared = true;
        Homie.getLogger() << "Preparing for deep sleep (" << publishInterval / 1000UL << " seconds)" << endl;
        Homie.prepareToSleep();
      }
#endif
    }
  }
}
void onHomieEvent(const HomieEvent& event) {
  static bool readyToSleep = false;
  switch (event.type) {
    case HomieEventType::READY_TO_SLEEP:
      WiFi.disconnect();    // Calling doDeepSleep() at this point sometimes causes exceptions due to asynchronous arrival 
                            // of WIFI_DISCONNECTED. WDT wakes up the Device immediatly after the exception. So deep sleep does not work.
                            // To work around this, I decided to inherit this task to the WIFI_DISCONNECTED event.
      readyToSleep = true;
      break;
    case HomieEventType::WIFI_DISCONNECTED:
      if(readyToSleep)
        Homie.doDeepSleep(publishIntervalSetting.get() * 1000000);
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
  Homie.setSetupFunction(setupHandler).setBroadcastHandler(broadcastHandler).setLoopFunction(loopHandler).onEvent(onHomieEvent);
  Homie.disableLedFeedback();

  deepSleepSetting.setDefaultValue(false);
  publishIntervalSetting.setDefaultValue(300UL).setValidator([] (long candidate) {
    return (candidate >= 2) && (candidate < 86400); // 2sec - 1day
  });

  Homie.setup();
}

void loop() {
  Homie.loop();
}
