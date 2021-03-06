/* AFE Firmware for smart home devices, Website: https://afe.smartnydom.pl/ */

/* Initializing MQTT */
void MQTTInit() {
  if (Device.getMode() != AFE_MODE_ACCESS_POINT &&
      Device.configuration.api.mqtt) {
    MQTTConfiguration = Data.getMQTTConfiguration();
    Mqtt.begin();
#ifdef DEBUG
    Serial << endl << "API: MQTT initialized";
#endif
  }
}

/* Method is launched after MQTT Message is received */
void MQTTMessagesListener(char *topic, byte *payload, unsigned int length) {
  if (length >= 1) {
#ifdef DEBUG
    Serial << endl << endl << "--------- Got MQTT request ---------";
    Serial << endl << "Topic: " << topic;
    Serial << endl << "Message: ";
    for (uint8_t _i = 0; _i < length; _i++) {
      Serial << char(payload[_i]);
    }
    Serial << endl << "Processing: ";
#endif

#ifdef AFE_CONFIG_FUNCTIONALITY_RELAY /* Relay processing */
    for (uint8_t i = 0; i < Device.configuration.noOfRelays; i++) {

#ifdef AFE_CONFIG_HARDWARE_GATE
      /* For the Relay assigned to a gate code below is not needed for execution
       */
      if (Relay[i].gateId == AFE_HARDWARE_ITEM_NOT_EXIST) {
#endif

        if (strcmp(topic, Relay[i].getMQTTCommandTopic()) == 0) {
#ifdef DEBUG
          Serial << "YES";
#endif

          if ((char)payload[1] == 'n' && length == 2) { // on
            Relay[i].on();
            MQTTPublishRelayState(i);
            DomoticzPublishRelayState(i);
          } else if ((char)payload[1] == 'f' && length == 3) { // off
            Relay[i].off();
            MQTTPublishRelayState(i);
            DomoticzPublishRelayState(i);
          } else if ((char)payload[1] == 'e' && length == 3) { // get
            MQTTPublishRelayState(i);
          } else if ((char)payload[1] == 'o' && length == 6) { // toggle
            Relay[i].get() == AFE_RELAY_ON ? Relay[i].off() : Relay[i].on();
            MQTTPublishRelayState(i);
            DomoticzPublishRelayState(i);
          }
          return;
        }

#ifdef AFE_CONFIG_FUNCTIONALITY_THERMOSTAT
        else {

          sprintf(_mqttTopic, "%sthermostat/cmd", Relay[i].getMQTTTopic());

          if (strcmp(topic, _mqttTopic) == 0) {
            if ((char)payload[0] == 'o' && length == 2) { // on
              Relay[i].Thermostat.on();
              Mqtt.publish(Relay[i].getMQTTTopic(), "thermostat/state",
                           Relay[i].Thermostat.enabled() ? "on" : "off");
            } else if ((char)payload[0] == 'o' && length == 3) { // off
              Relay[i].Thermostat.off();
              Mqtt.publish(Relay[i].getMQTTTopic(), "thermostat/state",
                           Relay[i].Thermostat.enabled() ? "on" : "off");
            } else if ((char)payload[0] == 't' && length == 6) { // toggle
              Relay[i].Thermostat.enabled() ? Relay[i].Thermostat.off()
                                            : Relay[i].Thermostat.on();
              Mqtt.publish(Relay[i].getMQTTTopic(), "thermostat/state",
                           Relay[i].Thermostat.enabled() ? "on" : "off");
            } else if ((char)payload[0] == 'g' && length == 3) { // get
              Mqtt.publish(Relay[i].getMQTTTopic(), "thermostat/state",
                           Relay[i].Thermostat.enabled() ? "on" : "off");
            }
#ifdef AFE_CONFIG_FUNCTIONALITY_HUMIDISTAT
          } else {
            /* Checking if Hunidistat related message has been received  */
            sprintf(_mqttTopic, "%shumidistat/cmd", Relay[i].getMQTTTopic());

            if (strcmp(topic, _mqttTopic) == 0) {
              if ((char)payload[0] == 'o' && length == 2) { // on
                Relay[i].Humidistat.on();
                Mqtt.publish(Relay[i].getMQTTTopic(), "humidistat/state",
                             Relay[i].Humidistat.enabled() ? "on" : "off");
              } else if ((char)payload[0] == 'o' && length == 3) { // off
                Relay[i].Humidistat.off();
                Mqtt.publish(Relay[i].getMQTTTopic(), "humidistat/state",
                             Relay[i].Humidistat.enabled() ? "on" : "off");
              } else if ((char)payload[0] == 't' && length == 6) { // toggle
                Relay[i].Humidistat.enabled() ? Relay[i].Humidistat.off()
                                              : Relay[i].Humidistat.on();
                Mqtt.publish(Relay[i].getMQTTTopic(), "humidistat/state",
                             Relay[i].Humidistat.enabled() ? "on" : "off");
              } else if ((char)payload[0] == 'g' && length == 3) { // get
                Mqtt.publish(Relay[i].getMQTTTopic(), "humidistat/state",
                             Relay[i].Humidistat.enabled() ? "on" : "off");
              }
            }
#endif
          }
        }
#endif

#ifdef AFE_CONFIG_HARDWARE_GATE
        /* Closing the condition for skipping relay if assigned to a gate */
      }
#ifdef DEBUG
      else {
        Serial << endl
               << "Excluding relay: " << i
               << " as it's assigned to a Gate: " << Relay[i].gateId;
      }
#endif
#endif
    }
#endif

#ifdef AFE_CONFIG_HARDWARE_PIR
    for (uint8_t i = 0; i < sizeof(Device.configuration.isPIR); i++) {
      if (Device.configuration.isPIR[i]) {
#ifdef DEBUG
        Serial << endl << " - Checking if PIR[" << i << "] request ";
#endif
        sprintf(_mqttTopic, "%scmd", Pir[i].getMQTTTopic());
        if (strcmp(topic, _mqttTopic) == 0) {
#ifdef DEBUG
          Serial << "YES";
#endif
          if ((char)payload[1] == 'e' && length == 3) { // get
            MQTTPublishPIRState(i);
          }
          return;
        }
      }
    }
#endif

#ifdef AFE_CONFIG_HARDWARE_HPMA115S0
    for (uint8_t i = 0; i < Device.configuration.noOfHPMA115S0s; i++) {
      if (strcmp(topic, ParticleSensor[i].mqttCommandTopic) == 0 &&
          (char)payload[1] == 'e' && length == 3) { // get
        MQTTPublishParticleSensorData(i);
      }
    }

#endif

#ifdef AFE_CONFIG_HARDWARE_BMEX80
    for (uint8_t i = 0; i < Device.configuration.noOfBMEX80s; i++) {
      if (strcmp(topic, BMEX80Sensor[i].mqttCommandTopic) == 0 &&
          (char)payload[1] == 'e' && length == 3) { // get
        MQTTPublishBMEX80SensorData(i);
      }
    }
#endif

#ifdef AFE_CONFIG_HARDWARE_BH1750
    for (uint8_t i = 0; i < Device.configuration.noOfBH1750s; i++) {
      if (strcmp(topic, BH1750Sensor[i].mqttCommandTopic) == 0 &&
          (char)payload[1] == 'e' && length == 3) { // get
        MQTTPublishBH1750SensorData(i);
      }
    }
#endif

#ifdef AFE_CONFIG_HARDWARE_ADC_VCC
    if (Device.configuration.isAnalogInput) {
#ifdef DEBUG
      Serial << endl << " - Checking if Analog Input request ";
#endif
      if (strcmp(topic, AnalogInput.mqttCommandTopic) == 0) {
#ifdef DEBUG
        Serial << "YES";
#endif
        if ((char)payload[1] == 'e' && length == 3) { // get
          MQTTPublishAnalogInputData();
        }
      }
    }
#endif

#ifdef AFE_CONFIG_HARDWARE_CONTACTRON
    /* Contactrons */
    for (uint8_t i = 0; i < Device.configuration.noOfContactrons; i++) {
      if (strcmp(topic, Contactron[i].getMQTTCommandTopic()) == 0 &&
          (char)payload[1] == 'e' && length == 3)) { // get
          MQTTPublishContactronState(i);
        }
    }
#endif

#ifdef AFE_CONFIG_HARDWARE_GATE
    for (uint8_t i = 0; i < Device.configuration.noOfGates; i++) {
      if (strcmp(topic, Gate[i].getMQTTCommandTopic()) == 0) {
        if ((char)payload[0] == 't' && length == 6) { // toggle
          Gate[i].toggle();
        } else if ((char)payload[0] == 'g' && length == 3) { // get
          MQTTPublishGateState(i);
        }
      }
    }
#endif

    /* Control over APIs Turning On/Off them
    #ifdef AFE_CONFIG_FUNCTIONALITY_API_CONTROL

    #ifdef DEBUG
        Serial << endl << " - Checking if API control request ";
    #endif
        sprintf(_mqttTopic, "%sconfiguration/api/http/cmd",
                MQTTConfiguration.mqtt.topic);

        if (strcmp(topic, _mqttTopic) == 0) {
    #ifdef DEBUG
          Serial << "Yes: HTTP";
    #endif
          if ((char)payload[1] == 'n' && length == 2) { // on
            Data.saveAPI(API_HTTP, true);
            Device.begin();
          } else if ((char)payload[1] == 'f' && length == 3) { // off
            Data.saveAPI(API_HTTP, false);
            Device.begin();
          }
          return;
        }
        sprintf(_mqttTopic, "%sconfiguration/api/domoticz/cmd",
                MQTTConfiguration.mqtt.topic);

        if (strcmp(topic, _mqttTopic) == 0) {
    #ifdef DEBUG
          Serial << "Yes: Domoticz";
    #endif
          if ((char)payload[1] == 'n' && length == 2) { // on
            Data.saveAPI(API_DOMOTICZ, true);
            Device.begin();
            DomoticzInit();

          } else if ((char)payload[1] == 'f' && length == 3) { // off
            Data.saveAPI(API_DOMOTICZ, false);
            Device.begin();
            Domoticz.disconnect();
          }
          return;
        }

        sprintf(_mqttTopic, "%sconfiguration/api/mqtt/cmd",
                MQTTConfiguration.mqtt.topic);

        if (strcmp(topic, _mqttTopic) == 0) {
    #ifdef DEBUG
          Serial << "Yes: MQTT";
    #endif
          if ((char)payload[1] == 'f' && length == 3) { // off
            Data.saveAPI(API_MQTT, false);
            Device.begin();
            Mqtt.disconnect();
          }
          return;
        }
    #endif
    */

    /*
    #ifdef DEBUG
        Serial << endl << " - Checking device level requests ";
    #endif
        sprintf(_mqttTopic, "%scmd", MQTTConfiguration.mqtt.topic);

        if (strcmp(topic, _mqttTopic) == 0) {
          if ((char)payload[2] == 'b' && length == 6) {
    #ifdef DEBUG
            Serial << "Yes: reboot";
    #endif
            Device.reboot(AFE_MODE_NORMAL);
          }
          else if ((char)payload[2] == 'n' && length == 17) {
    #ifdef DEBUG
            Serial << "Yes: Configuration Mode";
    #endif
            PASSWORD password = Data.getPasswordConfiguration();
            if (!password.protect) {
              Device.reboot(AFE_MODE_CONFIGURATION);
            }
          }
        }
    #ifdef AFE_CONFIG_TEMPERATURE
        else if ((char)payload[2] == 't' && length == 14) {
    #ifdef DEBUG
          Serial << "Yes: temperature";
    #endif
          char temperatureString[6];
          dtostrf(Sensor.getTemperature(), 2, 2, temperatureString);
          Mqtt.publish("temperature", temperatureString);
        }
    #endif

    #ifdef AFE_CONFIG_HARDWARE_DHXX

        else if ((char)payload[2] == 't' && length == 11) {
    #ifdef DEBUG
          Serial << "Yes: humidity";
    #endif
          char humidityString[6];
          dtostrf(Sensor.getHumidity(), 2, 2, humidityString);
          Mqtt.publish("humidity", humidityString);
        }

        else if ((char)payload[3] == 'H' && length == 12) {
    #ifdef DEBUG
          Serial << "Yes: heat index";
    #endif
          char heatIndex[6];
          dtostrf(Sensor.getHeatIndex(), 2, 2, heatIndex);
          Mqtt.publish("heatIndex", heatIndex);
        }

        else if ((char)payload[3] == 'D' && length == 12) {
    #ifdef DEBUG
          Serial << "Yes: dewPoint";
    #endif
          char heatIndex[6];
          dtostrf(Sensor.getDewPoint(), 2, 2, heatIndex);
          Mqtt.publish("dewPoint", heatIndex);
        }
    #endif
    */
  }

#ifdef DEBUG
  Serial << endl << "------------------------------------" << endl;
#endif
} /* End of topics listener */

#ifdef AFE_CONFIG_HARDWARE_RELAY
/* Metod publishes Relay state (used eg by HTTP API) */
void MQTTPublishRelayState(uint8_t id) {
  if (Device.configuration.api.mqtt) {
    Mqtt.publishTopic(Relay[id].getMQTTStateTopic(),
                      Relay[id].get() == AFE_RELAY_ON ? "on" : "off");
  }
}
#endif

/* Metod publishes Relay state (used eg by HTTP API) */
void MQTTPublishSwitchState(uint8_t id) {
  if (Device.configuration.api.mqtt) {
    Mqtt.publishTopic(Switch[id].getMQTTStateTopic(),
                      Switch[id].getPhisicalState() ? "open" : "closed");
  }
}

#ifdef AFE_CONFIG_TEMPERATURE
/* Metod publishes temperature */
void MQTTPublishTemperature(float temperature) {
  if (Device.configuration.api.mqtt) {
    // Mqtt.publish("temperature", temperature);
  }
}
#endif

/* Humidity, HeatIndex, DewPoint */
#ifdef AFE_CONFIG_HUMIDITY
/* Metod publishes humidity */
void MQTTPublishHumidity(float humidity) {
  if (Device.configuration.api.mqtt) {
    //  Mqtt.publish("humidity", humidity);
  }
}
/* Metod publishes HeatIndex */
void MQTTPublishHeatIndex(float heatIndex) {
  if (Device.configuration.api.mqtt) {
    //    Mqtt.publish("heatIndex", heatIndex);
  }
}
/* Metod publishes Dew point */
void MQTTPublishDewPoint(float dewPoint) {
  if (Device.configuration.api.mqtt) {
    // Mqtt.publish("dewPoint", dewPoint);
  }
}
#endif

#if defined(T3_CONFIG)
/* Metod publishes Relay state (used eg by HTTP API) */
void MQTTPublishPIRState(uint8_t id) {
  if (Device.configuration.api.mqtt) {
    Mqtt.publish(Pir[id].getMQTTTopic(), "state",
                 Pir[id].motionDetected() ? "open" : "closed");
  }
}
#endif

#ifdef AFE_CONFIG_HARDWARE_CONTACTRON
void MQTTPublishContactronState(uint8_t id) {
  if (Device.configuration.api.mqtt) {
    Mqtt.publishTopic(Contactron[id].getMQTTStateTopic(),
                      Contactron[id].get() == AFE_CONTACTRON_OPEN
                          ? AFE_MQTT_CONTACTRON_OPEN
                          : AFE_MQTT_CONTACTRON_CLOSED);
  }
}
#endif

#ifdef AFE_CONFIG_HARDWARE_GATE
void MQTTPublishGateState(uint8_t id) {
  if (Device.configuration.api.mqtt) {
    uint8_t gateState = Gate[id].get();
    Mqtt.publishTopic(Gate[id].getMQTTStateTopic(),
                      gateState == AFE_GATE_OPEN
                          ? AFE_MQTT_GATE_OPEN
                          : gateState == AFE_GATE_CLOSED
                                ? AFE_MQTT_GATE_CLOSED
                                : gateState == AFE_GATE_PARTIALLY_OPEN
                                      ? AFE_MQTT_GATE_PARTIALLY_OPEN
                                      : AFE_MQTT_GATE_UNKNOWN);
  }
}
#endif

#ifdef AFE_CONFIG_HARDWARE_HPMA115S0
void MQTTPublishParticleSensorData(uint8_t id) {
  if (Device.configuration.api.mqtt) {
    char json[80];
    ParticleSensor[id].getJSON(json);
    Mqtt.publishTopic(ParticleSensor[id].configuration.mqtt.topic, json);
  }
}
#endif

#ifdef AFE_CONFIG_HARDWARE_BMEX80
void MQTTPublishBMEX80SensorData(uint8_t id) {
  if (Device.configuration.api.mqtt) {
    // @TODO Estiamte the max JSON size
    char message[1000];
    BMEX80Sensor[id].getJSON(message);
    Mqtt.publishTopic(BMEX80Sensor[id].configuration.mqtt.topic, message);
  }
}
#endif

#ifdef AFE_CONFIG_HARDWARE_BH1750
void MQTTPublishBH1750SensorData(uint8_t id) {
  if (Device.configuration.api.mqtt) {
    char message[60];
    BH1750Sensor[id].getJSON(message);
    Mqtt.publishTopic(BH1750Sensor[id].configuration.mqtt.topic, message);
  }
}
#endif

#ifdef AFE_CONFIG_HARDWARE_AS3935
void MQTTPublishAS3935SensorData(uint8_t id) {
  if (Device.configuration.api.mqtt) {
    char message[60];
    AS3935Sensor[id].getJSON(message);
    Mqtt.publishTopic(AS3935Sensor[id].configuration.mqtt.topic, message);
  }
}
#endif

#ifdef AFE_CONFIG_HARDWARE_ADC_VCC
void MQTTPublishAnalogInputData() {
    if (Device.configuration.api.mqtt) {
        // @TODO check the size of the JSON before release!
        char message[60];
        AnalogInput.getJSON(message);
        Mqtt.publishTopic(AnalogInput.configuration.mqtt.topic, message);
    }
}
#endif
