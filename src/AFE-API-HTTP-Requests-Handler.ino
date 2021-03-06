/* AFE Firmware for smart home devices, Website: https://afe.smartnydom.pl/ */

/* Method listens for HTTP Requests */
void HTTPRequestListener() {
  if (Device.configuration.api.http) {
    if (WebServer.httpAPIlistener()) {
#ifdef AFE_CONFIG_HARDWARE_LED
      Led.on();
#endif
      processHTTPAPIRequest(WebServer.getHTTPCommand());
#ifdef AFE_CONFIG_HARDWARE_LED
      Led.off();
#endif
    }
  }
}

/* Method creates JSON respons after processing HTTP API request, and pushes it.
 * The second one method converts float to charString before pushing response */
void sendHTTPAPIRequestStatus(HTTPCOMMAND *request, boolean status,
                              const char *value = "") {
  /*
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();

    root["device"] = request.device;
    root["name"] = request.name;
    root["command"] = request.command;

    if (!strlen(value) == 0) {
      root["value"] = value;
    }

    root["status"] = status ? "success" : "error";

  #ifdef DEBUG
    Serial << endl << "HTTP Request JSON Buffer size" << jsonBuffer.size();
  #endif

    String respond;
    root.printTo(respond);
    WebServer.sendJSON(respond);
    */

  String respond;
  respond = "{\"device\":{";
  respond += "\"type\":\"" + String(request->device) + "\"";
  if (strlen(request->name) > 0) {
    respond += ",\"name\":\"" + String(request->name) + "\"";
  }
  respond += "},\"command\":\"" + String(request->command) + "\",";

  if (!strlen(value) == 0) {
    respond += "\"data\":";
    respond += value;
    respond += ",";
  }
  respond += "\"status\":\"";
  respond += status ? "success" : "error";
  respond += "\"}";
  WebServer.sendJSON(respond);
}
/*
void sendHTTPAPIRequestStatus(HTTPCOMMAND request, boolean status,
                              String value) {

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();

  root["device"] = request.device;
  root["name"] = request.name;
  root["command"] = request.command;
  root["value"] = value;

  root["status"] = status ? "success" : "error";

#ifdef DEBUG
  Serial << endl << "HTTP Request JSON Buffer size" << jsonBuffer.size();
#endif

  String respond;
  root.printTo(respond);
  WebServer.sendJSON(respond);
}
*/

void sendHTTPAPIRequestStatus(HTTPCOMMAND *request, boolean status,
                              double value, uint8_t width = 2,
                              uint8_t precision = 2) {
  char valueString[10];
  dtostrf(value, width, precision, valueString);
  sendHTTPAPIRequestStatus(request, status, valueString);
}

#ifdef AFE_CONFIG_HARDWARE_RELAY // Not required for T5
/* Method converts Relay value to string and invokes sendHTTPAPIRequestStatus
 * method which creates JSON respons and pushes it */
void sendHTTPAPIRelayRequestStatus(HTTPCOMMAND *request, boolean status,
                                   byte value) {
  sendHTTPAPIRequestStatus(request, status,
                           value == AFE_RELAY_ON ? "on" : "off");
}
#endif

/* PIR responses */
#if defined(T3_CONFIG)
void sendHTTPAPIPirRequestStatus(HTTPCOMMAND request, boolean status,
                                 boolean value) {
  sendHTTPAPIRequestStatus(&request, status, value ? "open" : "closed");
}
#endif

/* Gate and Contactron responses */
#ifdef AFE_CONFIG_HARDWARE_GATE
/* It constructs HTTP response related to gate and calls HTTP push */
void sendHTTPAPIGateRequestStatus(HTTPCOMMAND request, boolean status,
                                  byte value) {
  sendHTTPAPIRequestStatus(request, status,
                           value == AFE_GATE_OPEN
                               ? "open"
                               : value == AFE_GATE_CLOSED
                                     ? "closed"
                                     : value == AFE_GATE_PARTIALLY_OPEN
                                           ? "partiallyOpen"
                                           : "unknown");
}
#endif

#ifdef AFE_CONFIG_HARDWARE_CONTACTRON
/* It constructs HTTP response related to contactron and calls HTTP push */
void sendHTTPAPIContactronRequestStatus(HTTPCOMMAND request, boolean status,
                                        byte value) {
  sendHTTPAPIRequestStatus(request, status,
                           value == AFE_CONTACTRON_OPEN ? "open" : "closed");
}
#endif
/*
#ifdef AFE_CONFIG_HARDWARE_BH1750
void sendHTTPAPIBH1750RequestStatus(HTTPCOMMAND request, uint8_t id) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();

  root["lux"] = BH1750Sensor[id].get();

#ifdef DEBUG
  Serial << endl << "HTTP Request JSON Buffer size" << jsonBuffer.size();
#endif

  String respond;
  root.printTo(respond);
  sendHTTPAPIRequestStatus(&request, true, respond);
}

#endif
*/
/* Method processes HTTP API request */
void processHTTPAPIRequest(HTTPCOMMAND request) {

  boolean deviceNotExist = true;

#ifdef DEBUG
  Serial << endl << endl << "-------- Got HTTP Request --------";
  Serial << endl << "Device: " << request.device;
  Serial << endl << "Name: " << request.name;
  Serial << endl << "Command: " << request.command;
  Serial << endl << "----------------------------------" << endl;

#endif

#ifdef AFE_CONFIG_HARDWARE_GATE

  /* Request related to gate */
  if (strcmp(request.device, "gate") == 0) {
    for (uint8_t i = 0; i < Device.configuration.noOfGates; i++) {
      if (strcmp(request.name, Gate[i].configuration.name) == 0) {
        deviceNotExist = false;
        if (strcmp(request.command, "toggle") == 0) {
          Gate[i].toggle();
          sendHTTPAPIGateRequestStatus(request, true, Gate[i].get());
        } else if (strcmp(request.command, "get") == 0) { // get
          sendHTTPAPIGateRequestStatus(request, true, Gate[i].get());
        } else {
          sendHTTPAPIRequestStatus(&request, false);
        }
        break;
      }
    }
    if (deviceNotExist) {
      sendHTTPAPIRequestStatus(&request, false);
    }
    return;
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_CONTACTRON
  /* Request related to contactron */
  if (strcmp(request.device, "contactron") == 0) {
    for (uint8_t i = 0; i < Device.configuration.noOfContactrons; i++) {
      if (strcmp(request.name, Contactron[i].configuration.name) == 0) {
        deviceNotExist = false;
        if (strcmp(request.command, "get") == 0) {
          sendHTTPAPIContactronRequestStatus(request, true,
                                             Contactron[i].get());
        } else {
          sendHTTPAPIRequestStatus(&request, false);
        }
        break;
      }
    }
    if (deviceNotExist) {
      sendHTTPAPIRequestStatus(&request, false);
    }
    return;
  }

#endif

#ifdef AFE_CONFIG_HARDWARE_RELAY
  // @TODO should it be included for T5????
  /* Request related to relay */
  if (strcmp(request.device, "relay") == 0) {
    uint8_t state;
    for (uint8_t i = 0; i < Device.configuration.noOfRelays; i++) {
#ifdef AFE_CONFIG_HARDWARE_GATE
      /* For the Relay assigned to a gate code below is not needed for execution
       */
      if (Relay[i].gateId == AFE_HARDWARE_ITEM_NOT_EXIST) {
#endif
        if (strcmp(request.name, Relay[i].getName()) == 0) {
          deviceNotExist = false;
          if (strcmp(request.command, "on") == 0) {
            Relay[i].on();
            MQTTPublishRelayState(i); // MQTT Listener library
            if (strcmp(request.source, "domoticz") != 0) {
              DomoticzPublishRelayState(i);
            }
            sendHTTPAPIRelayRequestStatus(
                &request, Relay[i].get() == AFE_RELAY_ON, Relay[i].get());

          } else if (strcmp(request.command, "off") == 0) { // Off
            Relay[i].off();
            MQTTPublishRelayState(i); // MQTT Listener library
            if (strcmp(request.source, "domoticz") != 0) {
              DomoticzPublishRelayState(i);
            }
            sendHTTPAPIRelayRequestStatus(
                &request, Relay[i].get() == AFE_RELAY_OFF, Relay[i].get());
          } else if (strcmp(request.command, "toggle") == 0) { // toggle
            state = Relay[i].get();
            Relay[i].toggle();
            sendHTTPAPIRelayRequestStatus(&request, state != Relay[i].get(),
                                          Relay[i].get());
            MQTTPublishRelayState(i); // MQTT Listener library
            if (strcmp(request.source, "domoticz") != 0) {
              DomoticzPublishRelayState(i);
            };
          } else if (strcmp(request.command, "get") == 0) {
            sendHTTPAPIRelayRequestStatus(&request, true, Relay[i].get());
/* Command not implemented.Info */
#ifdef AFE_CONFIG_FUNCTIONALITY_THERMOSTAT
          } else if (strcmp(request.command, "enableThermostat") == 0) {
            Relay[i].Thermostat.on();
            sendHTTPAPIRequestStatus(
                request, true, Relay[i].Thermostat.enabled() ? "on" : "off");
          } else if (strcmp(request.command, "disableThermostat") == 0) {
            Relay[i].Thermostat.off();
            sendHTTPAPIRequestStatus(
                request, true, Relay[i].Thermostat.enabled() ? "on" : "off");
          } else if (strcmp(request.command, "toggleThermostat") == 0) {
            Relay[i].Thermostat.enabled() ? Relay[i].Thermostat.off()
                                          : Relay[i].Thermostat.on();
            sendHTTPAPIRequestStatus(
                request, true, Relay[i].Thermostat.enabled() ? "on" : "off");
          } else if (strcmp(request.command, "getThermostat") == 0) {
            sendHTTPAPIRequestStatus(
                request, true, Relay[i].Thermostat.enabled() ? "on" : "off");

#endif

#ifdef AFE_CONFIG_FUNCTIONALITY_HUMIDISTAT
          } else if (strcmp(request.command, "enableHumidistat") == 0) {
            Relay[i].Humidistat.on();
            sendHTTPAPIRequestStatus(
                request, true, Relay[i].Humidistat.enabled() ? "on" : "off");
          } else if (strcmp(request.command, "disableHumidistat") == 0) {
            Relay[i].Humidistat.off();
            sendHTTPAPIRequestStatus(
                request, true, Relay[i].Humidistat.enabled() ? "on" : "off");
          } else if (strcmp(request.command, "toggleHumidistat") == 0) {
            Relay[i].Humidistat.enabled() ? Relay[i].Humidistat.off()
                                          : Relay[i].Humidistat.on();
            sendHTTPAPIRequestStatus(
                request, true, Relay[i].Humidistat.enabled() ? "on" : "off");
          } else if (strcmp(request.command, "getHumidistat") == 0) {
            sendHTTPAPIRequestStatus(
                request, true, Relay[i].Humidistat.enabled() ? "on" : "off");

#endif

          } else {
            sendHTTPAPIRequestStatus(&request, false);
          }
          break;
        }

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
    if (deviceNotExist) {
      sendHTTPAPIRequestStatus(&request, false);
    }
    return;
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_DS18B20
  /* Request related to ds18b20 */
  if (strcmp(request.device, "ds18b20") == 0) {
    strcmp(request.command, "get") == 0
        ? sendHTTPAPIRequestStatus(&request, true, Sensor.getTemperature())
        : sendHTTPAPIRequestStatus(&request, false);
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_DHXX
  /* Request related to DHT Sensor */
  if (strcmp(request.device, "dht") == 0) {
    if (strcmp(request.name, "temperature") == 0) {
      strcmp(request.command, "get") == 0
          ? sendHTTPAPIRequestStatus(&request, true, Sensor.getTemperature())
          : sendHTTPAPIRequestStatus(&request, false);
    } else if (strcmp(request.name, "humidity") == 0) {
      strcmp(request.command, "get") == 0
          ? sendHTTPAPIRequestStatus(&request, true, Sensor.getHumidity())
          : sendHTTPAPIRequestStatus(&request, false);
    } else if (strcmp(request.name, "heatIndex") == 0) {
      strcmp(request.command, "get") == 0
          ? sendHTTPAPIRequestStatus(&request, true, Sensor.getHeatIndex())
          : sendHTTPAPIRequestStatus(&request, false);
    } else if (strcmp(request.name, "dewPoint") == 0) {
      strcmp(request.command, "get") == 0
          ? sendHTTPAPIRequestStatus(&request, true, Sensor.getDewPoint())
          : sendHTTPAPIRequestStatus(&request, false);
    } else {
      sendHTTPAPIRequestStatus(&request, false);
    }
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_PIR
  if (strcmp(request.device, "pir") == 0) {
    boolean pirSendFailure = true;
    for (uint8_t i = 0; i < sizeof(Device.configuration.isPIR); i++) {
      if (Device.configuration.isPIR[i]) {
        if (strcmp(request.name, Pir[i].getName()) == 0) {
          if (strcmp(request.command, "get") == 0) { // get
            pirSendFailure = false;
            sendHTTPAPIPirRequestStatus(request, true, Pir[i].motionDetected());
          }
        }
      }
    }
    if (pirSendFailure) {
      sendHTTPAPIRequestStatus(&request, false);
    }
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_HPMA115S0
  /* Request related to gate */
  if (strcmp(request.device, "HPMA115S0") == 0 &&
      strcmp(request.command, "get") == 0) {
    for (uint8_t i = 0; i < Device.configuration.noOfHPMA115S0s; i++) {
      if (strcmp(request.name, ParticleSensor[i].configuration.name) == 0) {
        char json[80];
        ParticleSensor[i].getJSON(json);
        sendHTTPAPIRequestStatus(&request, true, json);
        deviceNotExist = false;
        break;
      }
    }
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_BMEX80
  /* BMEX80 */
  if (strcmp(request.device, "BMEX80") == 0 &&
      strcmp(request.command, "get") == 0) {
    for (uint8_t i = 0; i < Device.configuration.noOfBMEX80s; i++) {
      if (strcmp(request.name, BMEX80Sensor[i].configuration.name) == 0) {
        // @TODO optimize the char size
        char json[1000];
        BMEX80Sensor[i].getJSON(json);
        sendHTTPAPIRequestStatus(&request, true, json);
        deviceNotExist = false;
        break;
      }
    }
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_BH1750
  /* BH1750 */
  if (strcmp(request.device, "BH1750") == 0 &&
      strcmp(request.command, "get") == 0) {
    for (uint8_t i = 0; i < Device.configuration.noOfBH1750s; i++) {
      if (strcmp(request.name, BH1750Sensor[i].configuration.name) == 0) {
        char json[60];
        BH1750Sensor[i].getJSON(json);
        sendHTTPAPIRequestStatus(&request, true, json);
        deviceNotExist = false;
        break;
      }
    }
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_AS3935
  /* AS3935 */
  if (strcmp(request.device, "AS3935") == 0 &&
      strcmp(request.command, "get") == 0) {
    for (uint8_t i = 0; i < Device.configuration.noOfAS3935s; i++) {
      if (strcmp(request.name, AS3935Sensor[i].configuration.name) == 0) {
        char json[60];
        AS3935Sensor[i].getJSON(json);
        sendHTTPAPIRequestStatus(&request, true, json);
        deviceNotExist = false;
      }
    }
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_ADC_VCC
  /* Analog Input */
  if (strcmp(request.device, "ADC") == 0) {
    if (strcmp(request.command, "get") == 0) {
        // @TODO check the size of the JSON before release!
        char json[60];
        AnalogInput.getJSON(json);
        sendHTTPAPIRequestStatus(&request, true, json);
        deviceNotExist = false;
    }
  }
#endif

#ifdef AFE_CONFIG_FUNCTIONALITY_API_CONTROL
  /* Requests related to APIs */
  if (strcmp(request.device, "api") == 0) {
    uint8_t _api =
        strcmp(request.name, "http") == 0
            ? API_HTTP
            : strcmp(request.name, "mqtt") == 0
                  ? API_MQTT
                  : strcmp(request.name, "domoticz") == 0 ? API_DOMOTICZ : 9;
    uint8_t _command = strcmp(request.command, "on") == 0
                           ? 1
                           : strcmp(request.command, "off") == 0 ? 0 : 9;

    if (_api != 9 && _command != 9) {
      Data.saveAPI(_api, _command);
      Device.begin();
      if (_command) {
        if (_api == API_MQTT) {
          MQTTInit();
        } else if (_api == API_DOMOTICZ) {
          DomoticzInit();
        }
      } else {
        if (_api == API_MQTT) {
          Mqtt.disconnect();
        } else if (_api == API_DOMOTICZ) {
          Domoticz.disconnect();
        }
      }
      sendHTTPAPIRequestStatus(&request, true);
    } else {
      sendHTTPAPIRequestStatus(&request, false);
    }
  }
#endif

  if (strcmp(request.command, "reboot") == 0) { // reboot
    sendHTTPAPIRequestStatus(&request, true);
    Device.reboot(Device.getMode());
  } else if (strcmp(request.command, "configurationMode") ==
             0) { // configurationMode
    PASSWORD password = Data.getPasswordConfiguration();
    if (!password.protect) {
      sendHTTPAPIRequestStatus(&request, true);
      Device.reboot(AFE_MODE_CONFIGURATION);
    } else {
      sendHTTPAPIRequestStatus(&request, false);
    }
    /* No such device or commend not implemented */
  } else {
    sendHTTPAPIRequestStatus(&request, false);
  }
}
