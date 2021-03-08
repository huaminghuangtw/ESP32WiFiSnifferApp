#pragma once

/* External heaader files */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* Project heaader files */
#include "globalVariables.hpp"


// OLED related functions
void initializeOLED();
void OLEDdisplayForOLEDInit();
void OLEDdisplayForSDCardInit();
void OLEDdisplayForSPIFFSInit();
void OLEDdisplayForLoRaInit();
void OLEDdisplayForLoRaSenderInit();
void OLEDdisplayForLoRaReceiverInit();
void OLEDdisplayForLoRaSender( wifiDeviceData data );
void OLEDdisplayForLoRaReceiver( int slaveID );
int getMaxPages();
void OLEDdisplayForSlaveDevicesMac();
void OLEDdisplayForWiFiConnection();
void OLEDdisplayForIPAddress();
void OLEDdisplayForMasterInit();
void OLEDdisplayForWebServer();