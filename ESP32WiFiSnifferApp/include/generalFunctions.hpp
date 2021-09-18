#pragma once

/* External heaader files */
#include <Arduino.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
/* To use 64-bit (long long) integers with ArduinoJson, you must set ARDUINOJSON_USE_LONG_LONG to 1.
   See https://arduinojson.org/v6/api/config/use_long_long/ */
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

/* Project heaader files */
#include "dataTypes.hpp"
#include "globalVariables.hpp"


// Functions which could not be categorized (yet) should be declared here.
void stringToMACnumber( const char* charmac, MacAddr& mac );
void MACnumberTostring( char* stringMac, MacAddr& mac );
void timestampTostring( char* stringTimestamp, unsigned long timestamp );
void packetCounterTostring( char* stringPacketCounter, unsigned int& packetCounter );
void RSSITostring( char* stringRSSI, int8_t& RSSI );
std::string uint32_to_string( uint32_t value );
std::string uint64_to_string( uint64_t value );
void print_uint64_t(uint64_t num);
JsonObject convertCombinedWifiDeviceDataToJSON( StaticJsonDocument<512>& staticJsonDoc, combinedWiFiDeviceData& deviceData );
void sendCombinedDeviceDataToClient( AsyncWebSocket& wsserver, combinedWiFiDeviceData& combinedData );
combinedWiFiDeviceData generateRandomRecordsFromSlaves();
combinedWiFiDeviceData generateRecordAtPointLNSMmodelFixedMac( float x_free, float y_free, int RSSIc, float path_loss_exp );