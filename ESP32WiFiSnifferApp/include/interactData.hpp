#pragma once

/* External heaader files */
//...

/* Project heaader files */
#include "dataTypes.hpp"
#include "globalVariables.hpp"
#include "generalFunctions.hpp"


// Data processor for demo mode. Collect only one single device specified in photo_device_MAC field in config.js.
// The position of this device is known in advance to validate the position determination algorithm. 
void demoModeDataProcessor( const wifiDevicePayload payload, int8_t* rawRSSI );

// Data processor for normal mode. Collect all random devices passing by.
void normalModeDataProcessor( const wifiDevicePayload payload, std::vector<combinedWiFiDeviceData> &buffer );