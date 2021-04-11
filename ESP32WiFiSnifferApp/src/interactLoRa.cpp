/* Header file belonging to this implementation */
#include "interactLoRa.hpp"

/* External heaader files */
//...

/* Project header files */
#include "globalVariables.hpp"
#include "generalFunctions.hpp"
#include "interactOLED.hpp"
#include "interactWiFiSniffer.hpp"



unsigned int packetCounter1 = 0;
unsigned int packetCounter2 = 0;
unsigned int packetCounter3 = 0;
unsigned int packetCounter4 = 0;



void initializeLoRa()
{
    Serial.println();
    Serial.println( "Initializing LoRa radio..." );

    // setup LoRa transceiver module
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    // replace the LoRa.begin(---E-) argument with your location's frequency 
    // 433E6 for Asia
    // 866E6 for Europe
    // 915E6 for North America
    int counter = 0;
    while ( !LoRa.begin(866E6) && counter < 10 )
    {   
        Serial.print(".");
        counter++;
        delay(500);
    }
    if (counter == 10)
    {
        Serial.println( "ERROR - LoRa initialization failed." );
    }

    Serial.println( "LoRa initialization OK!" );
    OLEDdisplayForLoRaInit();

    LoRa.setSignalBandwidth(250E3);

}  // initializeLoRa


void sendToMasterDevice( wifiDeviceData data )
{
    String payload = "";

#if defined(Slave_1)
    payload += String(packetCounter1) + "/";   // add packet ID
#elif defined(Slave_2)
    payload += String(packetCounter2) + "/";                  
#elif defined(Slave_3)
    payload += String(packetCounter3) + "/";                  
#elif defined(Slave_4)
    payload += String(packetCounter4) + "/";                  
#endif

    char strMacAddr[18] = {0};
    MACnumberTostring( strMacAddr, data.mac );
    payload += String(strMacAddr) + "\t";      // add WiFi device payload - MAC address

    payload += String(data.rssi) + "\n";       // add WiFi device payload - RSSI    


#if defined(Slave_1)
    Serial.print( F("Slave #1 ") );
#elif defined(Slave_2)
    Serial.print( F("Slave #2 ") );
#elif defined(Slave_3)
    Serial.print( F("Slave #3 ") );
#elif defined(Slave_4)
    Serial.print( F("Slave #4 ") );
#endif

    Serial.print( F("is sending packet: ") );

#if defined(Slave_1)
    Serial.println( packetCounter1 );
#elif defined(Slave_2)
    Serial.println( packetCounter2 );
#elif defined(Slave_3)
    Serial.println( packetCounter3 );
#elif defined(Slave_4)
    Serial.println( packetCounter4 );
#endif

    LoRa.idle();                        // put the radio in idle (standby) mode
    LoRa.beginPacket();                 // start packet          

#if defined(Slave_1) || defined(Slave_2) || defined(Slave_3) || defined(Slave_4)
    LoRa.write(destinationAddress);     // add destination address
#endif

    LoRa.write(localAddress);           // add local address

    LoRa.print(payload);                // add WiFi device payload (packetID + '/' + MAC address + '\t' + RSSI + '\n')

    LoRa.endPacket();                   // finish packet and send it
    LoRa.sleep();                       // put the radio in sleep mode.

    Serial.println( F("Packet sent.") );

} // sendToMasterDevice


void receiveFromSlaveDevice( int packetSize )
{
    if ( !packetSize )
    {
        return;
    }
    else
    {
        wifiDevicePayload WiFiDevicePayload;

        // read packet
        int recipient = LoRa.read();
        int sender = LoRa.read();
        
        String incomingPayload = "";   // payload of incoming WiFi device packet
        while ( LoRa.available() )
        {            
            incomingPayload = LoRa.readString();
            // incomingPayload format: packetID + '/' + MAC address + '\t' + RSSI + '\n'
            //Serial.println( incomingPayload );
        }
        
        if ( recipient != localAddress )
        {
            //Serial.println( "ERROR - Recipient address does not match local address!" );
            return;
        }

        WiFiDevicePayload.slaveID = sender;

        int pos1 = incomingPayload.indexOf('/');
        int pos2 = incomingPayload.indexOf('\t', pos1);
        int pos3 = incomingPayload.indexOf('\n', pos2);

        WiFiDevicePayload.packetID = (uint32_t) strtoul( incomingPayload.substring(0, pos1).c_str(), NULL, 0 );   // packetID
        strcpy( WiFiDevicePayload.mac, incomingPayload.substring(pos1 + 1, pos2).c_str() );                       // MAC Address
        WiFiDevicePayload.rssi = (int8_t) incomingPayload.substring(pos2 + 1, pos3).toInt();                      // RSSI

        // display payload information
        //printWiFiDevicePayload( WiFiDevicePayload );

        // display packet information on OLED display
        //OLEDdisplayForLoRaReceiver( sender );

        if (strlen(WiFiDevicePayload.mac) == 17)   // check if WiFiDevicePayload.mac contains garbages (must only contain "XX:XX:XX:XX:XX:XX")
        {
            xQueueSend( g_queueBetweenReceiveAndHandle, &WiFiDevicePayload, 0 );
        }      
    }

} // receiveFromSlaveDevice