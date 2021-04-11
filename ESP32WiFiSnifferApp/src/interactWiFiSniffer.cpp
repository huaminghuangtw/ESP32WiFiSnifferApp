/* Header file belonging to this implementation */
#include "interactWiFiSniffer.hpp"

/* External heaader files */
//...

/* Project header files */
#include "generalFunctions.hpp"
#include "interactWebServer.hpp"



int g_curChannel = 1;



// Gets the relevant data from the received packets and puts it on the queue for sending.
void sniffForWiFiPacketData( void* buf, wifi_promiscuous_pkt_type_t type )
{
    wifiDeviceData deviceData;

    // Cast the buffer content to wifi_promiscuous_pkt* pointer and assign it to p.
    wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t*) buf;
    
    // Assign the payload part (Data or management payload) to our custom data type.
    WifiMgmtHdr *wh = (WifiMgmtHdr*) p->payload;

    // Get the source MAC address out of the management header from above.
    MacAddr mac_add = (MacAddr) wh->sa;
    
    deviceData.mac = mac_add;                       // Copy the MAC adress to the "lightweight" datatype defined for sending data.             
    deviceData.rssi = (p->rx_ctrl).rssi;            // Save rssi (dBm) value.
    deviceData.timestamp = (p->rx_ctrl).timestamp;  // Save timestamp (us) when packet was received.

    // Putting the obtained data to the queue, so the core 1 task can send/print it.
    xQueueSend( g_queueBetweenMonitorAndSend, &deviceData, portMAX_DELAY );

} // sinffer


// Setting up promiscuous mode. esp_wifi_set_promiscuous_rx_cb runs "sniffer" each time a packet is received.
void setWifiPromiscuousMode()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();          // Use the default initialization config.
    esp_wifi_init( &cfg );                                        // Allocate resorces to WiFi driver, etc, according to the configuration in cfg.
    esp_wifi_set_storage( WIFI_STORAGE_RAM );                     // Store related data on volatile memory (RAM).
    esp_wifi_set_mode( WIFI_MODE_NULL );                          // Mode must be NULL otherwise you can't start promiscuous mode.
    esp_wifi_start();                                             // After setups, start wifi.
    esp_wifi_set_promiscuous( true );                             // Set promiscuous mode.
    esp_wifi_set_promiscuous_filter( &cg_filt );                  // Set filtered packet types (given in cg_filt).
    esp_wifi_set_promiscuous_rx_cb( &sniffForWiFiPacketData );    // Each time a packet is received, the registered callback function will be called (sniffForWiFiPacketData here).
    esp_wifi_set_channel( g_curChannel, WIFI_SECOND_CHAN_NONE );  // Set primary channel. No secondary channel considered.
                                                                  // (Secondary channel is also 20 MHz and in 802.11n it allows for the extension
                                                                  // of the primary channel from 20 MHz to 40 MHz bandwidth.)

} // setWifiPromiscuousMode


// Change between the wifi channels, so we can monitor traffic on all of them.
void changeWifiChannel()
{
    if ( g_curChannel > maxCh )
    { 
        g_curChannel = 1;
    }
    
    // Set the channel new channel number.
    // 2nd parameter: only not NONE, if 2 channels are used together to provide 40 MHz channels
    // instead of the normal 20 MHz bandwidth. This extended case is not treated here.
    esp_wifi_set_channel( g_curChannel, WIFI_SECOND_CHAN_NONE );
    
    // Change channel after some msecs.
    // Needs to be small enough so movement of devices can be captured in time with proper resolution.
    // (Think on how fast people normally walk. 1 sec/step at most.)
    // And at the same time enough time is needed to capture all the data packets.
    // If the channel numbers of the nearby APs mostly used by people walking by are known,
    // the loop over channels can be narrowed down accordingly from the max 13.
    vTaskDelay( 400 / portTICK_PERIOD_MS );
    
    g_curChannel++;

} // changeWifiChannel


// Parse WiFi device data to a string vector
std::vector<std::string> parseWiFiDeviceData( wifiDeviceData& data )
{
    std::vector<std::string> WiFiDeviceData(3);
    std::vector<std::ostringstream> os(2);
    
    os[0] << data.timestamp;
    WiFiDeviceData[0] = os[0].str();        // Timestamp

    // This char array must be able to accomodate a C-string.
    // C-strings have a 0 at their ending, so \0 is added to every char array passed to the function.
    // Our MAC format contains 2 * 6 = 12 characters for the 6 two digit hex values and 5 colons between them.
    // Therefore we need: 12 (MACs) + 5 (colons) + 1 (0 at end) = 18 characters for buffer size.
    char forPrintingBSSID[18] = {0};
    MACnumberTostring( forPrintingBSSID, data.mac );
    WiFiDeviceData[1] = forPrintingBSSID;   // MAC Address

    os[1] << data.rssi;
    WiFiDeviceData[2] = os[1].str();        // RSSI
    
    return WiFiDeviceData;

} // parseWiFiDeviceData


// Print WiFi device data to serial terminal
void printWiFiDeviceData( std::vector<std::string> data )
{
    Serial.print( F("Timestamp in usecs: ") );
	Serial.println( data[0].c_str() );
    Serial.print( F("MAC Address: ") );
	Serial.println( data[1].c_str() );
	Serial.print( F("RSSI: ") );
	Serial.print( data[2].c_str() );
    Serial.println( F(" dBm") );
    Serial.println();

} // printWiFiDeviceData


// Print WiFi device payload to serial terminal
void printWiFiDeviceData( wifiDeviceData& data )
{
    char stringTimestamp[20] = {0};
    timestampTostring( stringTimestamp, (unsigned long) data.timestamp );
    char stringMACaddress[18] = {0};
    MACnumberTostring( stringMACaddress, data.mac );

    Serial.print( F("Timestamp in usecs: ") );
	Serial.println( stringTimestamp );
    Serial.print( F("MAC Address: ") );
	Serial.println( stringMACaddress );
	Serial.print( F("RSSI: ") );
	Serial.print( data.rssi );
    Serial.println( F(" dBm") );
    Serial.println();

} // printWiFiDeviceData


// Print WiFi device payload to serial terminal
void printWiFiDevicePayload( wifiDevicePayload& payload )
{
    Serial.print( F("Received packet from ") );
    if (payload.slaveID == 1)
    {
        Serial.print( F("Slave #1:  ") );
    }
    else if (payload.slaveID == 2)
    {
        Serial.print( F("Slave #2:  ") );
    }
    else if (payload.slaveID == 3)
    {
        Serial.print( F("Slave #3:  ") );
    }
    else if (payload.slaveID == 4)
    {
        Serial.print( F("Slave #4:  ") );
    }
    Serial.println( payload.packetID );

    Serial.print( F("MAC Address: ") );
	Serial.println( payload.mac );

	Serial.print( F("RSSI: ") );
	Serial.print( payload.rssi );
	Serial.println( F(" dBm") );
    
    Serial.println();

} // printWiFiDeviceData


void printCombinedWiFiDeviceData( combinedWiFiDeviceData& combinedData )
{
    Serial.print( F("Last received packet #") );
    Serial.print( String(loggedDataCounter) );
    Serial.print( F(" at: ") );
    Serial.print( combinedData.timestamp );
    Serial.println();

    Serial.print( F("MAC Address: ") );
    char strMacAddr[18] = {0};
    MACnumberTostring( strMacAddr, combinedData.mac );
	Serial.println( strMacAddr );

	for ( size_t i = 0; i < g_espConfigData.number_of_slaves; ++i )
    {
        Serial.print( F("RSSI ") );
        Serial.print( i + 1 );
        Serial.print( F(" = ") );
        Serial.print( combinedData.rssis[i] );
        Serial.println( F(" dBm") );
    }

    Serial.println();

} // printCombinedWiFiDeviceData