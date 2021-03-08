#pragma once

/* External heaader files */
#include <FS.h>
/* To use 64-bit (long long) integers with ArduinoJson, you must set ARDUINOJSON_USE_LONG_LONG to 1.
   See https://arduinojson.org/v6/api/config/use_long_long/ */
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

/* Project heaader files */
//...


JsonObject getConfigFile( fs::FS &fs, DynamicJsonDocument& jsondoc, const char* path_to_file );
void printConfigFile( const char* path_to_file );
void printCurrentConfigData();
void configESPfromJSON( const char* path_to_file );