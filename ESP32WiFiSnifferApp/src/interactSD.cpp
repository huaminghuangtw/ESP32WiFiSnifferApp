#if defined(Master) || defined(Slave_1) || defined(Slave_2)


/* Header file belonging to this implementation */
#include "interactSD.hpp"

/* External heaader files */
#include <SD.h>

/* Project header files */
#include "interactOLED.hpp"
#include "dataTypes.hpp"
#include "generalFunctions.hpp"
#include "globalVariables.hpp"


SPIClass sdSPI;



void initializeSDCard()
{
    while (true)   // Loop until SD card gets initialized.
    {
      Serial.println();
      Serial.println( "Initializing SD card..." );
      
      vTaskDelay( 1000 / portTICK_PERIOD_MS );

      sdSPI = SPIClass(HSPI);
      sdSPI.begin(SDCARD_SCK, SDCARD_MISO, SDCARD_MOSI, -1);
      if ( !SD.begin(SDCARD_CS, sdSPI) )
      {
        Serial.println( "ERROR - SD card initialization failed!" );
        continue;
      }

      // Check whether SD card is attached.
      uint8_t cardType = SD.cardType(); 
      if ( cardType == CARD_NONE )
      {
        Serial.println( "No SD card detected. Please attach SD card!" );
        continue;
      }

      break;      
    }

    Serial.println( "Successfully initialize SD card!" );
    OLEDdisplayForSDCardInit();

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf( "SD card Size: %lluMB\n", cardSize );

} // initializeSDCard


void logSDFileHeader( std::string headerText, const char* path_to_file )
{
    writeFile( SD, path_to_file, headerText.c_str() );

} // logSDFileHeader


void writeFile( SDFileSystemClass &fs, const char* path, const char* message )
{
    Serial.printf( "Writing file: %s\n", path );
    File file = fs.open(path, FILE_WRITE);

    if(!file)
    {
      Serial.println( "Failed to open file for writing." );
      return;
    }

    if(file.print(message))
    {
      Serial.println( "File written." );
    } else {
      Serial.println( "Write failed." );
    }

    file.close();

} // writeFile


void appendFile( SDFileSystemClass &fs, const char* path, const char* message )
{
    // Serial.printf("Appending to file: %s\n", path);
    File file = fs.open(path, FILE_APPEND);

    if(!file)
    {
      Serial.println("Failed to open file for appending.");
      return;
    }

    if(file.print(message))
    {
      //Serial.println("Message appended");
    } else {
      Serial.println("Append failed.");
    }

    file.close();

} // appendFile


void logSDCard( String dataMessage, const char* path_to_file )
{
  appendFile(SD, path_to_file, dataMessage.c_str());

} // logSDCard


void logToCsvFileOnSD( wifiDeviceData& data, const char* path_to_file )
{
  char dataMessage[50] = "";

  strcat( dataMessage, uint32_to_string( data.timestamp ).c_str() ); 
  strcat( dataMessage, "," );  

  // This char array must be able to accomodate a C-string.
  // C-strings have a 0 at their ending, so \0 is added to every char array passed to the function.
  // Our MAC format contains 2 * 6 = 12 characters for the 6 two digit hex values and 5 colons between them.
  // Therefore we need: 12 (MACs) + 5 (colons) + 1 (0 at end) = 18 characters for buffer size.
  char forPrintingMAC[18] = {0};
  MACnumberTostring( forPrintingMAC, data.mac );
  strcat( dataMessage, forPrintingMAC ); 

  strcat( dataMessage, "," ); 

  char stringRSSI[5] = {0};
  RSSITostring( stringRSSI, data.rssi );
  strcat( dataMessage, stringRSSI ); 

  strcat( dataMessage, "\n" );  

  appendFile(SD, path_to_file, dataMessage);

} // logToCsvFileOnSD


void logToCsvFileOnSD( combinedWiFiDeviceData& data, const char* path_to_file )
{
  char dataMessage[100] = "";

  strcat( dataMessage, data.timestamp ); 
  strcat( dataMessage, "," );  

  // This char array must be able to accomodate a C-string.
  // C-strings have a 0 at their ending, so \0 is added to every char array passed to the function.
  // Our MAC format contains 2 * 6 = 12 characters for the 6 two digit hex values and 5 colons between them.
  // Therefore we need: 12 (MACs) + 5 (colons) + 1 (0 at end) = 18 characters for buffer size.
  char forPrintingMAC[18] = {0};
  MACnumberTostring( forPrintingMAC, data.mac );
  strcat( dataMessage, forPrintingMAC ); 

  for ( int i = 0; i < g_espConfigData.number_of_slaves; i++ )
  {
    strcat( dataMessage, "," );

    char stringRSSI[5] = {0};
    RSSITostring( stringRSSI, data.rssis[i] );
    strcat( dataMessage, stringRSSI ); 
  }

  strcat( dataMessage, "\n" );  

  appendFile(SD, path_to_file, dataMessage);

} // logToCsvFileOnSD


void readFile( SDFileSystemClass &fs, const char* path )
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file)
    {
        Serial.println("Failed to open file for reading.");
        return;
    }

    Serial.println("Read from file: ");
    while (file.available())
    {
        Serial.write(file.read());
    }

} // readFile


void listDir( SDFileSystemClass &fs, const char* dirname, uint8_t levels )
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root)
    {
        Serial.println("Failed to open directory.");
        return;
    }
    if(!root.isDirectory())
    {
        Serial.println("Not a directory.");
        return;
    }

    File file = root.openNextFile();
    while(file)
    {
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels)
            {
                listDir( fs, file.name(), levels - 1 );
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }

} // listDir


void logToBinFileOnSD( wifiDeviceData deviceData, const char* path_to_file )
{
  File binLogFile = SD.open( path_to_file , FILE_APPEND );
  binLogFile.write( (const uint8_t*) &deviceData, sizeof(deviceData) );
  binLogFile.close();

} // logBinFileToSD


void readBinFileFromSD( wifiDeviceData& deviceData, const char* path_to_file )
{
  File binLogFile = SD.open( path_to_file, FILE_READ );
  binLogFile.read( (uint8_t*) &deviceData, sizeof( wifiDeviceData ) );

} // readBinFileFromSD


void convertBinToCsvFile( const char* path_to_bin, const char* path_to_csv )
{
  File binLogFile = SD.open( path_to_bin, FILE_READ );

  // If we have an old version of this file, we should remove it first.
  if ( SD.exists( path_to_csv ) )
  {
    SD.remove( path_to_csv );
  }

  // Determine the number of recorded wifiDeviceData datapackets in the binary.
  uint32_t numOfRecords = ( binLogFile.size() ) / sizeof( wifiDeviceData );

  wifiDeviceData bufferDeviceData;

  // Add a header at the top of the csv.
  logSDFileHeader( "Time [usec],MAC,RSSI [dBm]\n", path_to_csv );

  // Copy the content of the binary file to the csv file.
  for ( size_t i = 0; i < numOfRecords; i++ )
  {
    binLogFile.read( (uint8_t*) &bufferDeviceData, sizeof( wifiDeviceData ) );
    logToCsvFileOnSD( bufferDeviceData, path_to_csv );
  }

} // convertBinToTextFile


#endif




