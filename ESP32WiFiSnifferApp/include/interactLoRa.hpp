#pragma once

/* External heaader files */
#include <SPI.h>
#include <LoRa.h>

/* Project heaader files */
#include "dataTypes.hpp"


extern unsigned int packetCounter1;
extern unsigned int packetCounter2;
extern unsigned int packetCounter3;
extern unsigned int packetCounter4;


void initializeLoRa();
void sendToMasterDevice( wifiDeviceData data );
void receiveFromSlaveDevice( int packetSize );