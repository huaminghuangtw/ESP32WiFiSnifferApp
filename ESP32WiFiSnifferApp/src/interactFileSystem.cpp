/* Header file belonging to this implementation */
#include "interactFileSystem.hpp"

/* External heaader files */
#include <FS.h>
#include <WiFi.h>

/* Project header files */
#include "interactSD.hpp"
#include "interactSPIFFS.hpp"
#include "generalFunctions.hpp"
#include "globalVariables.hpp"
#include "dataTypes.hpp"


JsonObject getConfigFile( fs::FS &fs, DynamicJsonDocument& jsondoc, const char* path_to_file )
{
    File jsonConfigFile = fs.open( path_to_file );
    if( jsonConfigFile )
    {
        DeserializationError error = deserializeJson( jsondoc, jsonConfigFile );
        if( error )
        {
            Serial.println( F("Error parsing JSON!") );
        }
        return jsondoc.as<JsonObject>();
    }
    else
    {
        Serial.println( F("Error opening JSON config file (or it does not exist)!") );
        Serial.println( F("Empty JSON object was returned.") );
        return jsondoc.to<JsonObject>();
    }

} // getConfigFile


void printConfigFile( const char* path_to_file )
{
#if defined(Master) || defined(Slave_1) || defined(Slave_2)
    File fileToRead = SD.open(path_to_file);
#elif defined(Slave_3) || defined(Slave_4)
    File fileToRead = SPIFFS.open(path_to_file);
#endif

    if( !fileToRead )
    {
        Serial.println( F("Failed to read file!") );
        return;
    }

    Serial.println( "\n" );
    Serial.println( "------ The content of the config file that was read in ------" );
    Serial.println();
    while ( fileToRead.available() )
    {
        Serial.print( (char) fileToRead.read() );
    }
    Serial.println();
    Serial.println();
    Serial.println( "---------------- End of config file read in -----------------" );
    Serial.println( "\n" );

    delay(1000);

} // printConfigFile


void printCurrentConfigData()
{
  Serial.println( "\n" );
  Serial.println( "----- Content of the current state of config variables -----" );
  Serial.println();

  Serial.print( "SSID: " );
  Serial.println( g_espConfigData.ssid );
  Serial.print( "Password: " );
  Serial.println( g_espConfigData.pswd );

  Serial.println();
  Serial.print( "1m RSSI: " );
  Serial.println( g_espConfigData.rssi_1m );

  Serial.println();
  Serial.print( "Path loss exponent: " );
  Serial.println( g_espConfigData.path_loss_exp, 4 );
 
  Serial.println(); 
  Serial.print( "Is this a demo: ");
  if ( g_espConfigData.demo )
  {
      Serial.println( "True" );
  }
  else
  {
      Serial.println( "False" );
  }

  Serial.println();
  Serial.print( "Master device: ");
  if ( g_espConfigData.am_i_master )
  {
      Serial.println( "True" );
      Serial.print( "Master MAC: " );
      {
        // This char array must be able to accomodate a C-string.
        // C-strings have a 0 at their ending (null-terminated string), so \0 is added to every char array passed to the function.
        // Our MAC format contains 2 * 6 = 12 characters for the 6 two digit hex values and 5 colons between them.
        // Therefore we need: 12 (MACs) + 5 (colons) + 1 (0 at end) = 18 characters for buffer size.
        char forPrintingMac[18] = {0};
        MACnumberTostring( forPrintingMac, g_espConfigData.master_MAC );
        Serial.println( forPrintingMac );
      }
  }
  else
  {
      Serial.println( "False" );
  }

  Serial.println();
  Serial.print( "Number of slaves: ");
  Serial.println( g_espConfigData.number_of_slaves );

  Serial.println();
  Serial.println( "Slave MACs: " );
  for ( int i = 0; i < g_espConfigData.number_of_slaves; i++ )
  {
    // This char array must be able to accomodate a C-string.
    // C-strings have a 0 at their ending, so \0 is added to every char array passed to the function.
    // Our MAC format contains 2 * 6 = 12 characters for the 6 two digit hex values and 5 colons between them.
    // Therefore we need: 12 (MACs) + 5 (colons) + 1 (0 at end) = 18 characters for buffer size.
    char forPrintingMac[18] = {0};
    MACnumberTostring( forPrintingMac, g_espConfigData.slave_MACs[i] );
    Serial.println( forPrintingMac );
  }
  Serial.println();
  
  Serial.println( "X Coordinates of slaves: " );
  for ( int i = 0; i < g_espConfigData.number_of_slaves; i++ )
  {
    Serial.print( g_espConfigData.slave_x_coords[i], 2 );
    Serial.print( " " );
  }
  Serial.println();
  
  Serial.println();
  Serial.println( "Y Coordinates of slaves: " );
  for ( int i = 0; i < g_espConfigData.number_of_slaves; i++ )
  {
    Serial.print( g_espConfigData.slave_y_coords[i], 2 );
    Serial.print( " " );
  }
  Serial.println();

  Serial.println();
  Serial.print( "MAC of photo device: " );
  {
    // This char array must be able to accomodate a C-string.
    // C-strings have a 0 at their ending, so \0 is added to every char array passed to the function.
    // Our MAC format contains 2 * 6 = 12 characters for the 6 two digit hex values and 5 colons between them.
    // Therefore we need: 12 (MACs) + 5 (colons) + 1 (0 at end) = 18 characters for buffer size.
    char forPrintingMac[18] = {0};
    MACnumberTostring( forPrintingMac, g_espConfigData.photo_device_MAC );
    Serial.println( forPrintingMac );
  }
  Serial.println();

  Serial.println( "-------------------- End of config info --------------------" );
  Serial.println( "\n" );

  delay(1000);

} // printCurrentConfigData


void configESPfromJSON( const char* path_to_file )
{
    // To calculate the size of JSON in bytes: https://arduinojson.org/v6/assistant/
    DynamicJsonDocument configDynJsonDoc( 2048 );
    JsonObject configJsonObj;

#if defined(Master) || defined(Slave_1) || defined(Slave_2)
    configJsonObj = getConfigFile( SD, configDynJsonDoc, path_to_file );
#elif defined(Slave_3) || defined(Slave_4)
    configJsonObj = getConfigFile( SPIFFS, configDynJsonDoc, path_to_file );
#endif
  
    strcpy( g_espConfigData.ssid, configJsonObj["ssid"] );
    strcpy( g_espConfigData.pswd, configJsonObj["pswd"] );
    g_espConfigData.am_i_master = configJsonObj["am_i_master"];
    g_espConfigData.rssi_1m = configJsonObj["RSSI_1m"];
    g_espConfigData.path_loss_exp = configJsonObj["path_loss_exp"];   // path loss exponent measured for esp in room, free-line-of-sight.
    g_espConfigData.demo = configJsonObj["demo"];
    
    // get MAC address of each ESP32 device and assign it to the config file
#if defined(Master)
    configJsonObj["master_MAC"] = WiFi.macAddress();
#elif defined(Slave_1)
    configJsonObj["slave_MACs"][0] = WiFi.macAddress();
#elif defined(Slave_2)
    configJsonObj["slave_MACs"][1] = WiFi.macAddress();
#elif defined(Slave_3)
    configJsonObj["slave_MACs"][2] = WiFi.macAddress();
#elif defined(Slave_4)
    configJsonObj["slave_MACs"][3] = WiFi.macAddress();
#endif

    const char* bufMasterDevice = configJsonObj["master_MAC"];
    stringToMACnumber( bufMasterDevice, g_espConfigData.master_MAC );

    g_espConfigData.master_coords[0] = configJsonObj["master_coords"]["x"];
    g_espConfigData.master_coords[1] = configJsonObj["master_coords"]["y"];

    g_espConfigData.number_of_slaves = configJsonObj["number_of_slaves"];

    const char* bufPhotoDevice = configJsonObj["photo_device_MAC"];
    stringToMACnumber( bufPhotoDevice, g_espConfigData.photo_device_MAC );


    for (uint8_t i = 0; i < g_espConfigData.number_of_slaves; i++)
    {
        const char* buf = configJsonObj["slave_MACs"][i];
        stringToMACnumber( buf, g_espConfigData.slave_MACs[i] );

        g_espConfigData.slave_x_coords[i] = configJsonObj["slave_coords"][i]["x"];
        g_espConfigData.slave_y_coords[i] = configJsonObj["slave_coords"][i]["y"];
    }

    delay(1000);

} // configESPfromJSON