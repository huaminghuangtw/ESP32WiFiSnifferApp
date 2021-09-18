#if defined(Slave_3) || defined(Slave_4)


#pragma once


/* External heaader files */
#include "FS.h"
#include "SPIFFS.h"
/* To use 64-bit (long long) integers with ArduinoJson, you must set ARDUINOJSON_USE_LONG_LONG to 1.
   See https://arduinojson.org/v6/api/config/use_long_long/ */
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

/* Project heaader files */
//...


/* --------------------------------------------------------------------------------------------------------------
   Use SPIFFS with ESP32
     *Before uploading sketch to the board:
         Step #1: Create a folder called "data" (it should be on the same level as src folder) and put files here.
         Step #2: Upload files to SPIFFS by either running "Upload Filesystem Image" task in PlatformIO IDE's GUI
                  or use command "pio run -t uploadfs" in terminal.
         Step #3: Finally upload your sketch to the board.
   -------------------------------------------------------------------------------------------------------------- */


void initializeSIPFFS();
void listDir( fs::FS &fs, const char * dirname, uint8_t levels );
void readFile( fs::FS &fs, const char * path );
void writeFile( fs::FS &fs, const char * path, const char * message );
void appendFile( fs::FS &fs, const char * path, const char * message );
void logSPISSF( std::string dataMessage, const char* path_to_file );
void logSPIFFSFileHeader( std::string headerText );
void renameFile( fs::FS &fs, const char * path1, const char * path2 );
void deleteFile( fs::FS &fs, const char * path );
void testFileIO( fs::FS &fs, const char * path );


#endif