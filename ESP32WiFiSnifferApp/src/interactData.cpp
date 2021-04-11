/* Header file belonging to this implementation */
#include "interactData.hpp"

/* External header files */
#include <functional>
#include <stdexcept>

/* Project header files */
#include "interactNTP.hpp"



// print RSSI received from each slave in the first round to check whether they are doing their job well...
bool printSlave1State = true;   
bool printSlave2State = true;
bool printSlave3State = true;
bool printSlave4State = true;



void demoModeDataProcessor( const wifiDevicePayload payload, int8_t* rawRSSI )
{
    char strPhotoDeviceMAC[18] = {0};
    MACnumberTostring( strPhotoDeviceMAC, g_espConfigData.photo_device_MAC );

    if ( strcmp( payload.mac, strPhotoDeviceMAC ) == 0 )  // check if the received MAC address equals to the photo device's MAC
    {
		//Serial.println( "MAC matched!" );

		if (payload.slaveID == 1)                
		{
			rawRSSI[0] = payload.rssi;

			if (printSlave1State)   
			{
				Serial.print( F("raw RSSI 1 = ") );
				Serial.println( rawRSSI[0] );

				printSlave1State = false;
			}                       
		}
		else if (payload.slaveID == 2)
		{
			rawRSSI[1] = payload.rssi;

			if (printSlave2State) 
			{
				Serial.print( F("raw RSSI 2 = ") );
				Serial.println( rawRSSI[1] );

				printSlave2State = false;
			}  
		}
		else if (payload.slaveID == 3)
		{
			rawRSSI[2] = payload.rssi;

			if (printSlave3State) 
			{
				Serial.print( F("raw RSSI 3 = ") );
				Serial.println( rawRSSI[2] );

				printSlave3State = false;
			}  
		}
		else if (payload.slaveID == 4)
		{
			rawRSSI[3] = payload.rssi;

			if (printSlave4State) 
			{
				Serial.print( F("raw RSSI 4 = ") );
				Serial.println( rawRSSI[3] );

				printSlave4State = false;
			}	  
		}

		if ( std::find( rawRSSI, rawRSSI + g_espConfigData.number_of_slaves, 0 ) == rawRSSI + g_espConfigData.number_of_slaves )   // check if there are as many RSSI values belonging to photo device as the number of slaves have been all collected
		{
			Serial.println( F("\nPreparing sniffed data for web server...\n") );

			combinedWiFiDeviceData combinedData;

			char now[30];
			getTextFormatTimeinMilliseconds( now );
			strcpy( combinedData.timestamp, now );

			std::copy( std::begin(g_espConfigData.photo_device_MAC.mac), std::end(g_espConfigData.photo_device_MAC.mac), std::begin(combinedData.mac.mac) );
			
			for ( int i = 0; i < g_espConfigData.number_of_slaves; i++ )
			{
				combinedData.rssis[i] = rawRSSI[i];
			}

			//printCombinedWiFiDeviceData( combinedData );

			xQueueSend( g_queueBetweenHandleAndWebServer, &combinedData, 0 );

			std::fill( rawRSSI, rawRSSI + g_espConfigData.number_of_slaves, 0 );   // re-initialize the array rawRSSI
		}
	}

} // demoModeDataProcessor


void normalModeDataProcessor( const wifiDevicePayload payload, std::vector<combinedWiFiDeviceData> &buffer )
{
    MacAddr payloadMAC;
    stringToMACnumber( payload.mac, payloadMAC );

    auto it = std::find_if( buffer.begin(), buffer.end(), [payloadMAC] ( const combinedWiFiDeviceData& existingDevice )
														{
                                                            return std::equal( std::begin(existingDevice.mac.mac), std::end(existingDevice.mac.mac), std::begin(payloadMAC.mac) );
														} );
    
    if ( it != buffer.end() )   // The incoming device already exists on buffer
    {
		int index = std::distance( buffer.begin(), it );
		
		if (payload.slaveID == 1)       
		{
			buffer[index].rssis[0] = payload.rssi;
		}
		else if (payload.slaveID == 2)
		{
			buffer[index].rssis[1] = payload.rssi;  
		}
		else if (payload.slaveID == 3)
		{
			buffer[index].rssis[2] = payload.rssi;  
		}
		else if (payload.slaveID == 4)
		{
			buffer[index].rssis[3] = payload.rssi;  
		}	

		auto indexOfZero = std::find( buffer[index].rssis, buffer[index].rssis + MAX_NUMBER_OF_SLAVES, 0 );

		if ( indexOfZero == buffer[index].rssis + g_espConfigData.number_of_slaves )   // check if there are as many RSSI values belonging to payloadMAC as the number of slaves have been all collected
		{
			Serial.println( F("\nPreparing sniffed data for web server...\n") );

			char now[30];
			getTextFormatTimeinMilliseconds( now );
			strcpy( buffer[index].timestamp, now );
			
			//printCombinedWiFiDeviceData( buffer[index] );

			xQueueSend( g_queueBetweenHandleAndWebServer, &buffer[index], 0 );

			std::fill( buffer[index].rssis, buffer[index].rssis + MAX_NUMBER_OF_SLAVES, 0 );   // re-initialize the array buffer[index].rssis
		}
    }
    else   // The incoming device doesn't exsit on buffer. Create a new combinedWiFiDeviceData object for it.
    {
		combinedWiFiDeviceData newDevice;

		std::copy( std::begin(payloadMAC.mac), std::end(payloadMAC.mac), std::begin(newDevice.mac.mac) );
		
		if (payload.slaveID == 1)       
		{
			newDevice.rssis[0] = payload.rssi;
		}
		else if (payload.slaveID == 2)
		{
			newDevice.rssis[1] = payload.rssi;  
		}
		else if (payload.slaveID == 3)
		{
			newDevice.rssis[2] = payload.rssi;  
		}
		else if (payload.slaveID == 4)
		{
			newDevice.rssis[3] = payload.rssi;  
		}

		buffer.push_back(newDevice);
    }

} // normalModeDataProcessor