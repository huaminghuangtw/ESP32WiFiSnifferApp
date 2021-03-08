#pragma once


/* ------------------------
   Task functions for Slave
   ------------------------ */


void monitorWiFiDevices( void* parameter );            // Function for task running on core 0.
void sendAndLogWiFiDeviceData( void* parameter );      // Function for task running on core 1.          