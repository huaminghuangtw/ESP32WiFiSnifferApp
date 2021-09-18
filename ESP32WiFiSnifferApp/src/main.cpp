/* External header files */
#include <esp_task_wdt.h>

/* Project header files */
#include "taskFunctionsForMaster.hpp"
#include "taskFunctionsForSlave.hpp"
#include "generalFunctions.hpp"
#include "interactFileSystem.hpp"
#include "interactSD.hpp"
#include "interactSPIFFS.hpp"
#include "interactLoRa.hpp"
#include "interactOLED.hpp"



void setup()
{
    {
        // 1. Initialize serial monitor.
        Serial.begin( 115200 );
        while( !Serial )   // wait for serial port to be available
        delay(500);
        Serial.println();
        Serial.println( "Serial was started." );

        // 2. Initialize OLED display.
        initializeOLED();

        // Use SD card to configure ESP board & store captured packet data locally.
        // If the board doesn't have a SD card slot mounted, use SPI file system (SPIFFS) instead.
        // The relevant files must be put in the "data" folder (the same level as src folder)
    #if defined(Master) || defined(Slave_1) || defined(Slave_2)
        // 3. Initialize SD card.
        initializeSDCard();
    #elif defined(Slave_3) || defined(Slave_4)
        // 3. Initialize SPI file system.
        initializeSIPFFS();
    #endif
    
        // 4. Initialize LoRa radio.
        initializeLoRa();
    }
    
    // Print the config file in the initial format. (just to compare whether info was read in properly.)
    printConfigFile( cg_path_to_config_JSON );

    // Configure ESP board from JSON config file.
    configESPfromJSON( cg_path_to_config_JSON );

    // Print the config data read in from JSON.
    printCurrentConfigData();


    // Size of a "word" is CPU architecture dependent. Short explanation can be found here:
    // https://stackoverflow.com/questions/19821103/what-does-it-mean-by-word-size-in-computer
    // So the size of a word on ESP32 is 32 bit (4 bytes) since it has a 32-bit CPU.
    // Here only "safely large" stack sizes were allocated for the tasks, no exact calculation was done.


    if( !g_espConfigData.am_i_master )   // If the current device is a slave, the monitor and sender tasks will be created.
    {
        // Check whether the queue was created properly.
        if( g_queueBetweenMonitorAndSend == NULL )
        {
            Serial.println( "An error occured when creating the queue between monitor and sender task." );
        }
            
        // Task pinned to core 0. Monitors wifi traffic, captures data and puts it on the queue.
        xTaskCreatePinnedToCore( monitorWiFiDevices,                // Task function
                                 "TaskForMonitoringWiFiDevices",    // Task name
                                 10000,                             // Stack size in words: So it is 10000 * 4 bytes here. Not precise just large enough.
                                 NULL,                              // Parameter passed as input to task
                                 1,                                 // Task priority
                                 NULL,                              // Task handle
                                 0 );                               // Core ID

        // Task pinned to core 1. Receives data from the queue. Logs to SD card and sends it to master.
        xTaskCreatePinnedToCore( sendAndLogWiFiDeviceData,          // Task function
                                 "TaskForSendingWiFiDeviceData",    // Task name
                                 10000,                             // Stack size in words
                                 NULL,                              // Parameter passed as input to task
                                 1,                                 // Task priority
                                 NULL,                              // Task handle
                                 1 );                               // Core ID
    }
    else   // If the current device is the master, the webserver and receiver tasks will be created.
    {
        // Check whether the queue was created properly.
        if( g_queueBetweenReceiveAndHandle == NULL )
        {
            Serial.println( "An error occured when creating the queue between receiver and handler task." );
            for(;;);  // if failed, do nothing.
        }

        if( g_queueBetweenHandleAndWebServer == NULL )
        {
            Serial.println( "An error occured when creating the queue between handler and webserver task." );
            for(;;);  // if failed, do nothing.
        }
        
        // OLED display
        OLEDdisplayForSlaveDevicesMac();
        OLEDdisplayForLoRaReceiverInit();

        // Task pinned to core 1. Hosts the webserver for data access.
        xTaskCreatePinnedToCore( hostWebServer,                     // Task function
                                 "TaskForHostingWebServer",         // Task name
                                 10000,                             // Stack size in words: So it is 10000 * 4 bytes here. Not precise, just large enough.
                                 NULL,                              // Parameter passed as input to task
                                 1,                                 // Task priority
                                 NULL,                              // Task handle
                                 1 );                               // Core ID: Server needs to run here, since watchdogs are disabled on core 1 (Arduino framework basis setup).
                                                                    // Therefore longer data exchange with server clients does not trigger them to reboot the system...
                                                                    // More info on watchdogs: https://www.embedded.com/introduction-to-watchdog-timers/

        // Task pinned to core 0. Receives data from slaves. Provides it to the webserver.
        xTaskCreatePinnedToCore( receiveAndLogWiFiDeviceData,       // Task function
                                 "TaskForSendingWiFiDeviceData",    // Task name
                                 10000,                             // Stack size in words
                                 NULL,                              // Parameter passed as input to task
                                 1,                                 // Task priority
                                 NULL,                              // Task handle
                                 0 );                               // Core ID
    }

} // setup


void loop()
{
    // "loop" runs on core 1, if left empty it starves other tasks on this core of CPU time.
    // No proper fix for this was found, just this solution everywhere, where Free RTOS and esp-idf was combined with Arduino code.
    // Due to some issue with using Arduino framework over ESP boards: https://github.com/espressif/arduino-esp32/issues/595
    delay( 1 );

} // loop