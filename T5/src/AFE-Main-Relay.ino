/* Initializing Relays */
void initRelay() {
  Relay[0].begin(0);
  Relay[0].setRelayAfterRestoringPower();
  Relay[0].setTimerUnitToSeconds(false);
}

/* Method checks if any relay should be automatically turned off */
void mainRelay() {
  if (Relay[0].autoTurnOff()) {
    Led.on();
    Mqtt.publish(Relay[0].getMQTTTopic(), "state", "off");
    Led.off();
  }
}
