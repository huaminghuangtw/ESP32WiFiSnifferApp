/* Header file belonging to this implementation */
#include "taskFunctionsForMaster.hpp"

/* External header files */
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

/* Project header files */
#include "globalVariables.hpp"
#include "generalFunctions.hpp"
#include "interactData.hpp"
#include "interactSD.hpp"
#include "interactSPIFFS.hpp"
#include "interactLoRa.hpp"
#include "interactOLED.hpp"
#include "interactNTP.hpp"
#include "interactWiFi.hpp"
#include "interactWiFiSniffer.hpp"
#include "interactWebServer.hpp"



/* -------------------------
   Task functions for Master
   ------------------------- */



// Function definition for task on core 0, receiving data from slave ESPs.
void receiveAndLogWiFiDeviceData( void* parameter )
{
    //Serial.print( "receiveAndLogWiFiDeviceData runs on core: " );
    //Serial.println( xPortGetCoreID() );

    LoRa.receive();   // puts the radio in continuous receive mode

    // Connect to WiFi network
    connectToWiFi( g_espConfigData.ssid, g_espConfigData.pswd );
    {
        Serial.print( F("Access web server at: ") );
        Serial.println( WiFi.localIP() );

        OLEDdisplayForIPAddress();
    }

    // Initialize NTP client
    Serial.println();
    Serial.println( F("Initializing NTP...") );
    configTime( 0, 0, NTP_SERVER );   // Syntax: configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    setenv( "TZ", TZ_INFO, 1 );       // Set time zone
    printLocalTime( timeinfo );
    
    {
        Serial.println( F("Waiting for data from slaves...\n\n") );
        OLEDdisplayForMasterInit();
    }
    
    int8_t rawRSSI[g_espConfigData.number_of_slaves] = {0};    // for demo mode
    std::vector<combinedWiFiDeviceData> buffer;                // for normal mode

    wifiDevicePayload payload;

    for(;;)
    {
        /* // For dummy data generator generateRecordAtPointLNSMmodelFixedMac(). Use this block when no (or not enough) slave devices are available to measure real data.
        {
            // Moving on line
            int RSSIc = g_espConfigData.rssi_1m;                 // RSSI value at 1 m.
            float pathLossExp = g_espConfigData.path_loss_exp;   // path loss exponent measured for esp in room, free-line-of-sight.
            uint8_t numOfMeasPoints = (4.58 - 0.02) / 0.02;
            float slope_of_line = (3.00 - 0.02) / (4.58 - 0.02);
            float x_coord = 0.02 * random( 1, numOfMeasPoints );
            float y_coord = slope_of_line * x_coord;

            combinedWiFiDeviceData combinedData = generateRecordAtPointLNSMmodelFixedMac( x_coord, y_coord, RSSIc, pathLossExp );

            Serial.print( combinedData.timestamp );
            Serial.print( ":   " );
            Serial.print( x_coord );
            Serial.print( " " );
            Serial.print( y_coord );
            Serial.print( "\n" );

            xQueueSend( g_queueBetweenHandleAndWebServer, &combinedData, 0);
        } */
        
        {
            receiveFromSlaveDevice( LoRa.parsePacket() );

            xQueueReceive( g_queueBetweenReceiveAndHandle, &payload, 1 );
            //printWiFiDevicePayload( payload );
            
            if (g_espConfigData.demo)
            {
                demoModeDataProcessor( payload, rawRSSI );
            }
            else
            {
                normalModeDataProcessor( payload, buffer );
            }
        }
        
        vTaskDelay( 1 / portTICK_PERIOD_MS );
    }

} // receiveAndLogWiFiDeviceData


// Function definition for task on core 1, hosting webserwer.
void hostWebServer( void* parameter )
{
#if defined(Master)
    //Serial.print( "hostWebServer runs on core: " );
    //Serial.println( xPortGetCoreID() );

    // Initialize LED
    pinMode( LEDPin, OUTPUT );
    digitalWrite( LEDPin, LOW );

    // Check for existence of the html file
    if ( !SD.exists( "/server/layout.htm" ) )
    {
        Serial.println( "ERROR - Can't find layout.htm file!" );
        for(;;);
    }

    // Setup the webserwer and the websocket on it.
    AsyncWebServer webserver( HTTP_PORT );
    AsyncWebSocket ws( "/ws" );

    addWebSocket( ws, webserver, handleWebSocketEvent );
    addMDNS( mDNSname );

    initWebServer( webserver );

    combinedWiFiDeviceData combinedData;

    for(;;)
    {
        cleanUpWebSocketClients( ws );
        digitalWrite( LEDPin, ledState );

        BaseType_t wasSomethingReceived = xQueueReceive( g_queueBetweenHandleAndWebServer, &combinedData, 1 );

        // If the queue was not empty and stream was requested, we should log and stream.
        if ( wasSomethingReceived && startToStreamData )
        {
            logToCsvFileOnSD( combinedData, "/server/data/logfile.csv" );
            sendCombinedDeviceDataToClient( ws, combinedData );
            printCombinedWiFiDeviceData( combinedData );
            OLEDdisplayForWebServer();
            loggedDataCounter++;
        }
        // If the queue was not empty but no stream request was done, we should only log the data.
        else if ( wasSomethingReceived )
        {
            logToCsvFileOnSD( combinedData, "/server/data/logfile.csv" );
            printCombinedWiFiDeviceData( combinedData );
            loggedDataCounter++;
        }

        // Let idle task run shortly. It is responsible of memory clean-up after a task was deleted.
        // See: https://www.freertos.org/RTOS-idle-task.html
        // Currently vTaskDelete() is not called in the code,
        // but that might be changed some time causing issues, if idle is starved of memory.
        vTaskDelay( 1 / portTICK_PERIOD_MS );
    }
#endif

} // hostWebServer
