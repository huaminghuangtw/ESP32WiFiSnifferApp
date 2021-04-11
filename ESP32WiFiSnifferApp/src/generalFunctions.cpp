/* Header file belonging to this implementation */
#include "generalFunctions.hpp"

/* External header files */
#include <stdint.h>

/* Project header files */
#include "globalVariables.hpp"
#include "dataTypes.hpp"
#include "interactNTP.hpp"
#include "interactOLED.hpp"
#include "interactNTP.hpp"


// Declared global variables ( from "globalVariables.hpp" ) are defined here.
QueueHandle_t g_queueBetweenMonitorAndSend = xQueueCreate( cg_queueSizeMonitorAndSend, sizeof( wifiDeviceData ) );
QueueHandle_t g_queueBetweenReceiveAndHandle = xQueueCreate( cg_queueSizeBetweenReceiveAndHandle, sizeof( wifiDevicePayload ) );
QueueHandle_t g_queueBetweenHandleAndWebServer = xQueueCreate( cg_queueSizeHandleAndWebServer, sizeof( combinedWiFiDeviceData ) );

configData g_espConfigData;



// Functions which could not be categorized (yet) should be defined here.

void stringToMACnumber( const char* charmac, MacAddr& mac )
{
    sscanf( charmac, "%hhX:%hhX:%hhX:%hhX:%hhX:%hhX",
            &(mac.mac[0]), &(mac.mac[1]), &(mac.mac[2]),
            &(mac.mac[3]), &(mac.mac[4]), &(mac.mac[5]) );

} // stringToMACnumber


void MACnumberTostring( char* stringMac, MacAddr& mac )
{
    // stringMac should point to an array of 18 characters.
    /* This char array must be able to accomodate a C-string.
       C-strings have a 0 at their ending, so \0 is added to every char array passed to the function.
       Our MAC format contains 2 * 6 = 12 characters for the 6 two digit hex values and 5 colons between them.
       Therefore we need: 12 (MACs) + 5 (colons) + 1 (0 at end) = 18 characters for buffer size. */
    sprintf( stringMac, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac.mac[0], mac.mac[1], mac.mac[2],
             mac.mac[3], mac.mac[4], mac.mac[5] );   // Formatting the data into the standard "MAC style".

} // MACnumberTostring


void timestampTostring( char* stringTimestamp, unsigned long timestamp )
{
    sprintf( stringTimestamp, "%lu", timestamp );

} // timestampTostring


void packetCounterTostring( char* stringPacketCounter, unsigned int& packetCounter )
{
    sprintf( stringPacketCounter, "%u", packetCounter );

} // packetCounterTostring


void RSSITostring( char* stringRSSI, int8_t& RSSI )
{
    sprintf( stringRSSI, "%d", RSSI ); 

} // RSSITostring


std::string uint64_to_string( uint64_t value ) 
{
    std::ostringstream os;
    os << value;
    return os.str();

} // uint64_to_string


std::string uint32_to_string( uint32_t value ) 
{
    std::ostringstream os;
    os << value;
    return os.str();

} // uint32_to_string


void print_uint64_t(uint64_t num)
{
    char rev[128]; 
    char *p = rev + 1;

    while (num > 0)
    {
        *p++ = '0' + ( num % 10 );
        num /= 10;
    }
    p--;

    // Print the number which is now in reverse
    while (p > rev)
    {
        Serial.print(*p--);
    }

} // print_uint64_t


JsonObject convertCombinedWifiDeviceDataToJSON( StaticJsonDocument<512>& staticJsonDoc, combinedWiFiDeviceData& deviceData )
{
    staticJsonDoc["time"] = deviceData.timestamp;

    char stringMac[18] = {0};
    MACnumberTostring( stringMac, deviceData.mac );
    staticJsonDoc["MAC"] = stringMac;

    JsonArray rssis = staticJsonDoc.createNestedArray("RSSIs");
    for ( int i = 0; i < g_espConfigData.number_of_slaves; i++ )
    {
        rssis.add( deviceData.rssis[i] );
    }

    return staticJsonDoc.as<JsonObject>();

} // convertCombinedWifiDeviceDataToJSON


void sendCombinedDeviceDataToClient( AsyncWebSocket& wsserver, combinedWiFiDeviceData& combinedData )
{
    StaticJsonDocument<512> staticJsonDoc;
    JsonObject staticJsonObject;

    staticJsonObject = convertCombinedWifiDeviceDataToJSON( staticJsonDoc, combinedData );
    
    // Safely larger than the number of characters the json will contain.
    char buffer[256];
    serializeJson( staticJsonObject, buffer );
    wsserver.text( streamRequestWithClientID, buffer );

} // sendCombinedDeviceDataToClient


float distanceFromSniffingESP( float x_free, float y_free, float x_esp, float y_esp )
{
    return sqrt( (x_free - x_esp) * (x_free - x_esp) + (y_free - y_esp) * (y_free - y_esp) );

} // distanceFromSniffingESP


combinedWiFiDeviceData generateRecordAtPointLNSMmodelFixedMac( float x_free, float y_free, int RSSIc, float path_loss_exp )
{
    combinedWiFiDeviceData combinedData;

    char now[30];
    getTextFormatTimeinMilliseconds(now);
    strcpy( combinedData.timestamp, now );

    // Data will be generated for a single arbitrary defined MAC address.
    combinedData.mac.mac[0] = 10;
    combinedData.mac.mac[1] = 10;
    combinedData.mac.mac[2] = 10;
    combinedData.mac.mac[3] = 10;
    combinedData.mac.mac[4] = 10;
    combinedData.mac.mac[5] = 10;

    for ( int i = 0; i < g_espConfigData.number_of_slaves; i++ )
    {
        // The function uses the Log-normal shadowing radio propagation model: https://core.ac.uk/download/pdf/30312076.pdf
        // dc = 1m where RSSIc was measured. RSSI = RSSIc - 10 * path_loss_exp * log(dist[meter] / dc = 1 meter)
        combinedData.rssis[i] = (int8_t) round( RSSIc - 10 * path_loss_exp * log10( distanceFromSniffingESP( x_free,    // X coord of tracked device.
                                                                                                             y_free,    // Y coord of tracked device.
                                                                                                             g_espConfigData.slave_x_coords[i],
                                                                                                             g_espConfigData.slave_y_coords[i]) ) );
    }

    return combinedData;

} // generateRecordAtPointLNSMmodel


// Function for generating random test recordings for the receiveAndLogWifiDeviceData() taskfunction when no slave devices are available.
combinedWiFiDeviceData generateRandomRecordsFromSlaves()
{
    combinedWiFiDeviceData combinedData;

    char now[30];
    getTextFormatTimeinMilliseconds(now);
    strcpy( combinedData.timestamp, now );

    MacAddr mac_add;
    for ( int j = 0; j < 6; j++ )
    {
        mac_add.mac[j] = random( 0, 255 );
    }
    combinedData.mac = mac_add;

    for ( int i = 0; i < g_espConfigData.number_of_slaves; i++ )
    {
        combinedData.rssis[i] = -1 * random( 0, 100 );
    }

    return combinedData;

} // generateRecordsFromSlaves