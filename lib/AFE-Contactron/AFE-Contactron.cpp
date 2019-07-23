/* AFE Firmware for smart home devices, Website: https://afe.smartnydom.pl/ */

#include "AFE-Contactron.h"

AFEContactron::AFEContactron(){};

void AFEContactron::begin(uint8_t id, AFEDevice *_Device,
                          AFEDataAccess *_Data) {
  Data = _Data;
  Device = _Device;
  configuration = Data->getContactronConfiguration(id);
  pinMode(configuration.gpio, INPUT_PULLUP);
  if (configuration.ledID != AFE_HARDWARE_ITEM_NOT_EXIST) {
    ContactronLed.begin(configuration.ledID);
  }
  _initialized = true;
}

boolean AFEContactron::get() {
  boolean currentState = digitalRead(configuration.gpio);
  boolean _return;
  if (configuration.type == CONTACTRON_NO) {
    if (currentState) {
      ContactronLed.on();
      _return = CONTACTRON_OPEN;
    } else {
      ContactronLed.off();
      _return = CONTACTRON_CLOSED;
    }
  } else {
    if (currentState) {
      ContactronLed.off();
      _return = CONTACTRON_CLOSED;
    } else {
      ContactronLed.on();
      _return = CONTACTRON_OPEN;
    }
  }

  return _return;
}

boolean AFEContactron::changed() {
  if (_changed) {
    _changed = false;
    return true;
  } else {
    return false;
  }
}

void AFEContactron::listener() {
  if (_initialized) {
    boolean currentState = digitalRead(configuration.gpio);
    unsigned long time = millis();

    if (currentState != state) { // contactron stage changed

      if (startTime == 0) { // starting timer. used for contactron bouncing
        startTime = time;
      }

      if (time - startTime >= configuration.bouncing) {
        state = currentState;
        _changed = true;
      }

    } else if (currentState == state && startTime > 0) {
      startTime = 0;
    }
  }
}

const char *AFEContactron::getMQTTCommandTopic() {
  if (strlen(configuration.mqtt.topic) > 0) {
    sprintf(mqttCommandTopic, "%s/cmd", configuration.mqtt.topic);
  } else {
    mqttCommandTopic[0] = '\0';
  }
  return mqttCommandTopic;
}

const char *AFEContactron::getMQTTStateTopic() {
  if (strlen(configuration.mqtt.topic) > 0) {
    sprintf(mqttStateTopic, "%s/state", configuration.mqtt.topic);
  } else {
    mqttStateTopic[0] = '\0';
  }
  return mqttStateTopic;
}

unsigned long AFEContactron::getDomoticzIDX() {
  return configuration.domoticz.idx;
}
