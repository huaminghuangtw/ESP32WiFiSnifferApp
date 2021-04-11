/* Header file belonging to this implementation */
#include "interactWebServer.hpp"

/* External header files */
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPmDNS.h>
#include <SD.h>

/* Project header files */
#include "globalVariables.hpp"
#include "interactSD.hpp"


// Main sources for understanding how the asynchronous websocket server works:
// https://m1cr0lab-esp32.github.io/remote-control-with-websocket/
// https://github.com/me-no-dev/ESPAsyncWebServer


// Define global variable ledState.
bool ledState = 0;
//bool startToConvert = false;
//uint32_t downloadRequestWithClientID = 0; // IDs start from 1 only...
bool startToStreamData = false;
uint32_t streamRequestWithClientID = 0;
int loggedDataCounter = 1;


void notifyWebSocketClients( AsyncWebSocket* wsserver )
{
	wsserver->textAll( String(ledState) );

} // notifyWebSocketClients


void handleWebSocketMessage( AsyncWebSocket* wsserver, AsyncWebSocketClient *wsclient, void *arg, uint8_t *data, size_t len)
{
	AwsFrameInfo *info = (AwsFrameInfo*)arg;
	if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
	{
		data[len] = 0;
		if (strcmp((char*)data, "toggle") == 0)
		{
			ledState = !ledState;
			notifyWebSocketClients( wsserver );
			Serial.println( "LED status has been changed..." );
		}
		else if (strcmp((char*)data, "streamData") == 0)
		{
			// If the Start/Stop button was pressed it enables/disables streaming
			// in the for cycle of the "hostWebserver" taskfunction by switching
			// the boolean value of "startToStreamData".
			Serial.println( "Streaming was started/halted." );
			startToStreamData = !startToStreamData;
			streamRequestWithClientID = wsclient->id();
			Serial.print( "This was the client id: " );
			Serial.println( streamRequestWithClientID );  
		}
	}

} // handleWebSocketMessage


void handleWebSocketEvent( AsyncWebSocket *wsserver, AsyncWebSocketClient *wsclient,
                           AwsEventType type, void *arg, uint8_t *data, size_t len )
{
	switch (type)
	{
		case WS_EVT_CONNECT:
			Serial.printf( "WebSocket client #%u connected from %s\n",
							wsclient->id(), wsclient->remoteIP().toString().c_str() );
			break;
		case WS_EVT_DISCONNECT:
			Serial.printf( "WebSocket client #%u disconnected\n", wsclient->id() );
			break;
		case WS_EVT_DATA:
			handleWebSocketMessage( wsserver, wsclient, arg, data, len );
			break;
		case WS_EVT_PONG:
		case WS_EVT_ERROR:
			break;
	}

} // handleWebSocketEvent


void addWebSocket( AsyncWebSocket& wsserver, AsyncWebServer& server, AwsEventHandler wsHandler )
{
	wsserver.onEvent( wsHandler );
	server.addHandler( &wsserver );

} // addWebSocket


void cleanUpWebSocketClients( AsyncWebSocket& wsserver )
{
	wsserver.cleanupClients();

} // cleanUpWebSocketClients


String templateProcessor( const String& var )
{
	if (var == "STATE")
	{
		if (ledState)
		{
			return "ON";
		}
		else
		{
			return "OFF";
		}
	}
	else
	{
		return "PROBLEM";
	}

} // templateProcessor


void initWebServer( AsyncWebServer& server )
{
	// Route for root / web page
	// Arguments detailed here properly: https://techtutorialsx.com/2017/12/01/esp32-arduino-asynchronous-http-webserver/
	// #1 path as string: URI. On which route the server should listen to requests?
	// #2 enum of type "WebRequestMethod": specifies the type of HTTP request.
	// #3 lambda function:
	server.on( "/",
				HTTP_GET,
				[](AsyncWebServerRequest *request)
				{ request->send( SD, "/server/layout.htm", "text/html", false, templateProcessor ); }
			);

	server.serveStatic( "/", SD, "/server/" );

	// Start server
	server.begin();

} // initWebServer


void addMDNS( const char* mDNSname )
{
	if (!MDNS.begin( mDNSname ))
	{
		Serial.println( F("Error starting mDNS...") );
		for(;;);
	}

} // addMDNS