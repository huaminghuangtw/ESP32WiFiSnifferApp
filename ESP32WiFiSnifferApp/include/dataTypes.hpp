#pragma once

/* External header files */
#include <stdint.h>
#include <vector>
#include <array>

/* Project heaader files */
//...



// For storing a MAC address.
typedef struct
{
  uint8_t mac[6];

} __attribute__((packed)) MacAddr;


// Store mac header info (management type). Check this for the details: https://hackmag.com/security/esp32-sniffer/
typedef struct
{
  int16_t fctl;                         // Frame control
  int16_t duration;                     // Duration
  MacAddr da;                           // Destination address
  MacAddr sa;                           // Source address
  MacAddr bssid;                        // Basic Service Set ID
  int16_t seqctl;                       // Sequence Control: Fragment Number and Sequence Number
  unsigned char payload[];

} __attribute__((packed)) WifiMgmtHdr;


// Datatype for storing all the relevant data about a device captured by one esp32.
typedef struct
{
    uint32_t timestamp; // The local time when the packet is received. It is precise only if modem sleep or light sleep is not enabled. unit: microsecond
                        // uint32_t is sufficient to store 1.19 hours of time uint64_t is enough for 5124095576 hours (but double the size of course...).
    MacAddr mac;        // MAC address stands from 6 two digit hexadecimal numbers.
                        // Each of it can be stored in an 8 bit variable.
                        // The whole address therefore can be stored in a 6 long array of them.
    int8_t rssi;        // Signal strength in dBm.

} __attribute__((packed)) wifiDeviceData;
// Because of "packed" the size will be 11 bytes no matter what, but since "timestamp" is exactly one word (32 bits on ESP32 as far as I know) and the
// memory is 32 bit aligned it can be read in one cycle, while if it would be after "mac" or "rssi" it would get distributed over two 32 bit memory
// chunks and therefore could be read only in to reading cycles. So this way it will be slightly faster.


// Define maximum number of group member devices. Can be anything but needs to be predetermined.
const int MAX_NUMBER_OF_SLAVES = 10;


// Type for storing configuration data.
typedef struct
{
  char ssid[64];                                     // Max ssid length has to be changed if exceeds 64 character.
  char pswd[64];                                     // Max password length has to be changed if exceeds 64 character.
  MacAddr slave_MACs[MAX_NUMBER_OF_SLAVES];          // Slave device MACs. If more than 10 devices are used "MAX_NUMBER_OF_SLAVES" needs to be modified.
  float slave_x_coords[MAX_NUMBER_OF_SLAVES];        // x coordinates of the slave devices. Buffer for 10 values. See above.
  float slave_y_coords[MAX_NUMBER_OF_SLAVES];        // y coordinates of the slave devices. Buffer for 10 values. See slave device MACs above.
  float master_coords[2];                            // x and y coordinate of the master device.
  MacAddr master_MAC;                                // MAC address of the master device.
  MacAddr photo_device_MAC;                          // MAC address of the device used at signal strength map creation.
  bool am_i_master;                                  // A device is either master=1 or slave=0.
  bool demo;                                         // Indicates whether it is a demo/validation run or a normal one.
  uint8_t number_of_slaves;                          // The number of slave devices in the group. Since "slave_MACs" won't be always completely filled,
                                                     // the unfilled MAC spots will all be "00:00:00:00:00:00"/undefined, but they are not valid device MACs.
  int rssi_1m;                                       // 1m RSSI. should be measured beforehand.
  float path_loss_exp;                               // Path loss exponent. Should be measured beforhand.

} __attribute__((packed)) configData;


// Type for storing the data collected from all slave devices.
typedef struct
{
  char timestamp[30];                                // Timestamp in text based format in milliseconds (e.g., 2001-08-23 14:55:02.583) for the recordings were made.
  int8_t rssis[MAX_NUMBER_OF_SLAVES] = {0};          // For the RSSIs received from each slave device per datapacket.
  MacAddr mac;                                       // For all of the MACs of the slave devices sending data to the master.

} __attribute__((packed)) combinedWiFiDeviceData;


// Type for storing the data collected from individual slave device.
typedef struct
{
  uint32_t packetID;
  int8_t rssi;                           
  int8_t slaveID;
  char mac[50] = {0};   // Make it large enough in case garbage were received by LoRa transmission.
  
} __attribute__((packed)) wifiDevicePayload;