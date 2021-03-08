#if defined(Master) || defined(Slave_1) || defined(Slave_2)


#pragma once

/* External heaader files */
#include <SD.h>
/* To use 64-bit (long long) integers with ArduinoJson, you must set ARDUINOJSON_USE_LONG_LONG to 1.
   See https://arduinojson.org/v6/api/config/use_long_long/ */
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

/* Project heaader files */
#include "dataTypes.hpp"
#include "globalVariables.hpp"
#include "generalFunctions.hpp"


// Initialize SD card
void initializeSDCard();

// Write to the SD card
void writeFile( SDFileSystemClass &fs, const char* path, const char* message );

// Append data to the SD card
void appendFile( SDFileSystemClass &fs, const char* path, const char* message );

// Write the chip readings to the SD card
void logSDCard( std::string dataMessage, const char* path_to_file );

// Read a file from the SD card
void readFile( SDFileSystemClass &fs, const char* path );

// List dir from the SD card
void listDir( SDFileSystemClass &fs, const char* dirname, uint8_t levels );

// Add header to the file
void logSDFileHeader( std::string headerText, const char* path_to_file );

// Log acquired device data to binary logfile on the SD card
void logToBinFileOnSD( wifiDeviceData deviceData, const char* path_to_file );

// Read logged device data from binary logfile on the SD card
void readBinFileFromSD( wifiDeviceData& deviceData, const char* path_to_file );

// Convert binary file to csv file
void convertBinToCsvFile( const char* path_to_bin, const char* path_to_csv );

// Log to a csv file, overloaded function for different purposes.
void logToCsvFileOnSD( wifiDeviceData& data, const char* path_to_file );
void logToCsvFileOnSD( combinedWiFiDeviceData& data, const char* path_to_file );


#endif
