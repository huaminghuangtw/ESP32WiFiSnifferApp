#pragma once

/* External heaader files */
#include "WiFi.h"

/* Project heaader files */
#include "globalVariables.hpp"


// WiFi connection related helper functions
void scanWiFiNetworks();
void connectToWiFi( const char* ssid, const char* password );
void disconnectWiFi();
String translateEncryptionType( wifi_auth_mode_t encryptionType );