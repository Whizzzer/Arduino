/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik EKblad
 *
 * DESCRIPTION
 * Example sketch showing how to measue light level using a LM393 photo-resistor
 * http://www.mysensors.org/build/light
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <MySensors.h>

#define CHILD_ID 5
#define MOISTURE_SENSOR_ANALOG_PIN A0

unsigned long SLEEP_TIME = 300000; // Sleep time between reads (in milliseconds)

MyMessage msg(CHILD_ID, V_HUM);
int lastmoistureLevel;


void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Soil Moisture Sensor", "1.0");

  // Register all sensors to gateway (they will be created as child devices)
  present(CHILD_ID, S_MOISTURE);
}

void loop()
{
  int16_t moistureLevel = (1023-analogRead(MOISTURE_SENSOR_ANALOG_PIN))/10.23;
  Serial.println(moistureLevel);
  if (moistureLevel != lastmoistureLevel) {
    send(msg.set(moistureLevel));
    lastmoistureLevel = moistureLevel;
  }
  sleep(SLEEP_TIME);
}
