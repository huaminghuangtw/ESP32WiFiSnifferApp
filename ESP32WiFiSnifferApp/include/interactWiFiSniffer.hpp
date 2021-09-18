#pragma once

/* External heaader files */
#include <esp_wifi.h>
#include <Arduino.h>

/* Project heaader files */
#include "dataTypes.hpp"
#include "globalVariables.hpp"


void sniffForWiFiPacketData( void* buf, wifi_promiscuous_pkt_type_t type );
void setWifiPromiscuousMode();
void changeWifiChannel();
std::vector<std::string> parseWiFiDeviceData( wifiDeviceData& data );
void printWiFiDeviceData( std::vector<std::string> data );
void printWiFiDeviceData( wifiDeviceData& data );
void printWiFiDevicePayload( wifiDevicePayload& payload );
void printCombinedWiFiDeviceData( combinedWiFiDeviceData& combinedData );