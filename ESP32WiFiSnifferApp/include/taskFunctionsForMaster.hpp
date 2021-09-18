#pragma once


/* -------------------------
   Task functions for Master
   ------------------------- */


void receiveAndLogWiFiDeviceData( void* parameter );     // Function for task running on core 0.
void hostWebServer( void* parameter );                   // Function for task running on core 1.
