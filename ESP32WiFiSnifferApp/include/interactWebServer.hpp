#pragma once

/* External header files */
#include <ESPAsyncWebServer.h>

/* Project heaader files */
//...



extern bool ledState;
extern int loggedDataCounter;



// Sends an update to all clients connected to the websocket about the current state.
void notifyWebSocketClients( AsyncWebSocket *wserver );

// Handles the different possible websocket events that can occur.
void handleWebSocketEvent( AsyncWebSocket *wsserver, AsyncWebSocketClient *wsclient,
                           AwsEventType type, void *arg, uint8_t *data, size_t len );

// Adds a websocket to the asynchronous webserver.                      
void addWebSocket( AsyncWebSocket& wsserver, AsyncWebServer& server, AwsEventHandler wsHandler );

// Initializes and starts the asynchronous webserver.
void initWebServer( AsyncWebServer& server );

// Sets an mDNS (local) domain name for the server.
void addMDNS( const char* mDNSname );

// Cleans up resources after uncorrectly closed or lost connections.
void cleanUpWebSocketClients( AsyncWebSocket& wsserver );