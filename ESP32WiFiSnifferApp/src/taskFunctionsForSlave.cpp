/* Header file belonging to this implementation */
#include "taskFunctionsForSlave.hpp"

/* External heaader files */
//...

/* Project header files */
#include "globalVariables.hpp"
#include "generalFunctions.hpp"
#include "interactSD.hpp"
#include "interactSPIFFS.hpp"
#include "interactLoRa.hpp"
#include "interactOLED.hpp"
#include "interactWiFiSniffer.hpp"



/* ------------------------
   Task functions for Slave
   ------------------------ */



// Function definition for task running on core 0.
// Monitors wifi packets while continuously changing channels.
// Captured data is put on to the queue.
void monitorWiFiDevices( void* parameter )
{
  //Serial.print( "monitorWiFiDevices runs on core: " );
  //Serial.println( xPortGetCoreID() );

  for(;;)
  {
    setWifiPromiscuousMode();

    if (g_espConfigData.demo)
    {
      // (For demonstration purpose) Only detect devices in the specified channel of connected WiFi AP. Then the discovery of desired device is much faster! 
      esp_wifi_set_channel( 2, WIFI_SECOND_CHAN_NONE );

      vTaskDelay( 200 / portTICK_PERIOD_MS );   // stay on one channel for 200 ms to be sure not to miss any beancon message since according to WiFi standard a beacon message has to be sent every 100 msec.
                                                // See: https://en.wikipedia.org/wiki/Beacon_frame
    }
    else
    {
      for( size_t i = 0; i < maxCh; ++i )
      {
         changeWifiChannel();
      }
    }
  }

} // monitorWiFiDevices


// Function definition for task running on core 1.
// This function logs the captured WiFi packet data to the SD card
// and sends it to the master device for further processing/submission.
void sendAndLogWiFiDeviceData( void* parameter )
{
  //Serial.print( "sendWiFiDeviceData runs on core: " );
  //Serial.println( xPortGetCoreID() );

  OLEDdisplayForLoRaSenderInit();

  wifiDeviceData data;

  for(;;)
  {
    // Give some time for the scan to take palce.
    delay( 10 );

    Serial.println();
    Serial.println( F("New reading starts: ") );
    Serial.println( F("------------------------------") );

    {
      xQueueReceive( g_queueBetweenMonitorAndSend, &data, portMAX_DELAY );

#if defined(Slave_1) || defined(Slave_2)
      // Log data to binary file
      logToBinFileOnSD( data, "/logfile/binlog.dat" );
      // Log data to CSV file
      logToCsvFileOnSD( data, "/logfile/logfile.csv" );
#endif

      sendToMasterDevice( data );

      printWiFiDeviceData( data );
      
      OLEDdisplayForLoRaSender( data );      

      delay( random(50) );  // Generate a random time interval for each slave sending packets, avoiding data collisions on reveiving side.
    }

    Serial.println( F("------------------------------") );
    Serial.println( F("End of a round of receiving...") );
    Serial.println();
  }

} // sendWiFiDeviceData