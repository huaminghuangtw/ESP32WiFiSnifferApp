#pragma once

/* External heaader files */
#include <esp_wifi.h>
#include <vector>

/* Project heaader files */
#include "dataTypes.hpp"


/* -----------------------------------------------------------------------------------------
   All global variables should be declared here.
   Their definition must be given in (only) one of the source files.
  
   Naming convention:
   A global variable should be indicated with the "g_" prefix.
   A constant global variable with the "cg_" prefix.
  
   Non-constant global variables (with "extern" in front to make them visible in other files)
   from here are defined mostly in "generalFunctions.cpp".
   ------------------------------------------------------------------------------------------ */


// Set pins for specific board
#if defined(Master) || defined(Slave_1) || defined(Slave_2)
    // set OLED pins
    #define OLED_ADDRESS 0x3c
    #define OLED_SDA 21
    #define OLED_SCL 22
    #define LEDPin 25

    // set SD card pins
    #define SDCARD_SCK 14
    #define SDCARD_MISO 2 
    #define SDCARD_MOSI 15
    #define SDCARD_CS 13

    // set LoRa pins
    #define LORA_SCK 5
    #define LORA_MISO 19
    #define LORA_MOSI 27
    #define LORA_SS 18
    #define LORA_RST 23
    #define LORA_DIO0 26
#elif defined(Slave_4)
    // set OLED pins
    #define OLED_ADDRESS 0x3c
    #define OLED_SDA 4
    #define OLED_SCL 15
    #define OLED_RST 16
    #define LEDPin 25

    // set LoRa pins
    #define LORA_SCK 5
    #define LORA_MISO 19
    #define LORA_MOSI 27
    #define LORA_SS 18
    #define LORA_RST 14
    #define LORA_DIO0 26
#elif defined(Slave_3)
    // set OLED pins
    #define OLED_ADDRESS 0x3c
    #define OLED_SDA 21
    #define OLED_SCL 22
    #define OLED_RST -1
    #define LEDPin 14

    // set LoRa pins
    #define LORA_SCK 5
    #define LORA_MISO 19
    #define LORA_MOSI 27
    #define LORA_SS 18
    #define LORA_RST 14
    #define LORA_DIO0 26
#endif


#if defined(Master)
  #define localAddress 0
#elif defined(Slave_1)
  #define localAddress 1
  #define destinationAddress 0
#elif defined(Slave_2)
  #define localAddress 2
  #define destinationAddress 0
#elif defined(Slave_3)
  #define localAddress 3
  #define destinationAddress 0
#elif defined(Slave_4)
  #define localAddress 4
  #define destinationAddress 0
#endif


// User-defined constants
#define MAX_MACS_ON_SCREEN 5
#define SCREEN_WIDTH 128   // OLED display width, in pixels
#define SCREEN_HEIGHT 64   // OLED display height, in pixels 
#define maxCh 11
#define HTTP_PORT 80


extern QueueHandle_t g_queueBetweenMonitorAndSend;
extern QueueHandle_t g_queueBetweenReceiveAndHandle;
extern QueueHandle_t g_queueBetweenHandleAndWebServer;


// Declare how many items should fit onto the queue
const uint8_t cg_queueSizeMonitorAndSend = 100;
const uint8_t cg_queueSizeBetweenReceiveAndHandle = 100;
const uint8_t cg_queueSizeHandleAndWebServer = 100;


// Current WiFi channel the ESP is listening to
extern int g_curChannel;


// Global variables for download request response
//extern bool startToConvert;
//extern uint32_t downloadRequestWithClientID;
extern bool startToStreamData;
extern uint32_t streamRequestWithClientID;


// mDNS name
const char mDNSname[] = "themostawesomesniffer";


// Global variables for reading in config data from JSON on SD / SPIFFS
const char cg_path_to_config_JSON[] = "/server/config.js";
extern configData g_espConfigData;


// Filter out "management" and "data" wifi packet types only
const wifi_promiscuous_filter_t cg_filt = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
};
