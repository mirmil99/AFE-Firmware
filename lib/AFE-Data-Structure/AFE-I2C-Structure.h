/* AFE Firmware for smart home devices, Website: https://afe.smartnydom.pl/ */

#ifndef _AFE_I2C_Structure_h
#define _AFE_I2C_Structure_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

struct I2CPORT {
  uint8_t SDA;
  uint8_t SCL;
};

#endif
