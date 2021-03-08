/* Header file belonging to this implementation */
#include "interactOLED.hpp"

/* External heaader files */
//...

/* Project header files */
#include "interactLoRa.hpp"
#include "interactWiFi.hpp"
#include "generalFunctions.hpp"
#include "interactNTP.hpp"
#include "interactWebServer.hpp"
#include "globalVariables.hpp"


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, LEDPin, 100000UL, 100000UL);


void initializeOLED()
{
   Serial.println();
   Serial.println( "Initializing OLED..." );
   
   // Initialize LED
   pinMode(OLED_RST, OUTPUT);
   digitalWrite(OLED_RST, LOW);
   delay(20);
   digitalWrite(OLED_RST, HIGH);

#if defined(Slave_4)
   Wire.begin(OLED_SDA, OLED_SCL);   // This line ONLY FOR Heltec WiFi LoRa 32 V2!!!
#else
   Wire.begin();
#endif

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
   if( !display.begin( SSD1306_SWITCHCAPVCC, OLED_ADDRESS, false, false ) )   // address 0x3C for 128 x 32
   { 
     Serial.println( "SSD1306 allocation failed." );
     for(;;);   // if failed, do nothing.
   }
   
   Serial.println( "OLED initialization OK!" );
   OLEDdisplayForOLEDInit();

} // initializeOLED()


void OLEDdisplayForOLEDInit()
{
   display.clearDisplay();
   display.setTextColor(WHITE);
   display.setTextSize(1); 
   display.setCursor(0,0);
   display.print("OLED Init. OK!");
   display.display();
   delay(2000);

} // OLEDdisplayForOLEDInit


void OLEDdisplayForSDCardInit()
{
   display.setCursor(0,10);
   display.print("SD Card Init. OK!");
   display.display();
   delay(2000);

} // OLEDdisplayForSDCardInit


void OLEDdisplayForSPIFFSInit()
{
   display.setCursor(0,10);
   display.print("SPIFFS Init. OK!");
   display.display();
   delay(2000);

} // OLEDdisplayForSPIFFSInit


void OLEDdisplayForLoRaInit()
{
   display.setCursor(0,20);
   display.print("LoRa Init. OK!");
   display.display();

} // OLEDdisplayForLoRaInit


void OLEDdisplayForLoRaSenderInit()
{
   display.clearDisplay();
   display.setTextColor(WHITE);
   display.setTextSize(1);
   display.setCursor(0,0);
   display.print("LORA SENDER");
   display.display();
   delay(2000);

} // OLEDdisplayForLoRaSender


void OLEDdisplayForLoRaReceiverInit()
{
   display.clearDisplay();
   display.setTextColor(WHITE);
   display.setTextSize(1);
   display.setCursor(0,0);
   display.print("LORA RECEIVER");
   display.display();
   delay(2000);

} // OLEDdisplayForLoRaReceiverInit


void OLEDdisplayForLoRaSender( wifiDeviceData data )
{
   display.clearDisplay();
   display.setTextColor(WHITE);
   display.setTextSize(1);
   display.setCursor(0,0);
   display.print("LoRa packet sent!");
   display.setCursor(0,30);

#if defined(Slave_1)
   display.print("Packet ID:" + String(packetCounter1));
#elif defined(Slave_2)
   display.print("Packet ID:" + String(packetCounter2));
#elif defined(Slave_3)
   display.print("Packet ID:" + String(packetCounter3));
#elif defined(Slave_4)
   display.print("Packet ID:" + String(packetCounter4));
#endif
   
   char stringMACaddress[18] = {0};
   MACnumberTostring( stringMACaddress, data.mac );

   display.setCursor(0,40);
   display.print("MAC:");
   display.print( stringMACaddress );
   display.setCursor(0,50);
   display.print("RSSI:");
   display.print( data.rssi );
   display.print(" dBm");
   display.display();

#if defined(Slave_1)
   packetCounter1++;
#elif defined(Slave_2)
   packetCounter2++;
#elif defined(Slave_3)
   packetCounter3++;
#elif defined(Slave_4)
   packetCounter4++;
#endif

} // OLEDdisplayForLoRaSender


void OLEDdisplayForLoRaReceiver( int slaveID )
{
   display.clearDisplay();
   display.setTextColor(WHITE);
   display.setTextSize(1);
   display.setCursor(0,0);
   display.print("Master Device");
   display.setCursor(0,10);

   if (slaveID == 1)
   {
      display.print("Received packet from Slave #1!");
   }
   else if (slaveID == 2)
   {
      display.print("Received packet from Slave #2!");
   }
   else if (slaveID == 3)
   {
      display.print("Received packet from Slave #3!");
   }
   else if (slaveID == 4)
   {
      display.print("Received packet from Slave #4!");
   }
   
   display.display();

} // OLEDdisplayForLoRaReceiver


int getMaxPages()
{
   int maxPages = g_espConfigData.number_of_slaves / MAX_MACS_ON_SCREEN;
   if ( g_espConfigData.number_of_slaves % MAX_MACS_ON_SCREEN != 0 )
   {
      maxPages++;
   }
   return maxPages;

} // getMaxPages


void OLEDdisplayForSlaveDevicesMac()
{
   int currentPage = 1;
   int rowIndex = 1;

   for ( size_t i = 0; i < g_espConfigData.number_of_slaves; ++i )
   {
      if ( rowIndex == MAX_MACS_ON_SCREEN || currentPage == 1 )
      {
         rowIndex = 1;

         delay(2000);
         display.clearDisplay();
         display.setTextColor(WHITE);
         display.setTextSize(1);
         
         display.setCursor(0,0);
         display.println("Active slaves: ");

         display.setCursor(100,0);
         display.println( String(currentPage) + "/" + String(getMaxPages()) );
         currentPage++;
      }
      
      display.setCursor(0, (rowIndex * 10) + 10);

      char forPrintingMac[18] = {0};
      MACnumberTostring( forPrintingMac, g_espConfigData.slave_MACs[i] );
      display.println(forPrintingMac);
      display.display();

      rowIndex++;
   }

   delay(2000);

} // OLEDdisplayForSlaveDevicesMac


void OLEDdisplayForWiFiConnection()
{
   display.clearDisplay();
   display.setTextColor(WHITE);
   display.setTextSize(1); 
   display.setCursor(0,0);
   display.print( "Connecting to " + String(g_espConfigData.ssid) + "..." );
   display.display();

} // OLEDdisplayForWiFiConnection


void OLEDdisplayForIPAddress()
{
   display.clearDisplay();
   display.setTextColor(WHITE);
   display.setTextSize(1); 
   display.setCursor(0,0);
   display.print( "Access web server at: " );
   display.setCursor(0,10);
   display.print( WiFi.localIP() );
   display.display();

} // OLEDdisplayForIPAddress


void OLEDdisplayForMasterInit()
{
   display.setCursor(0,30);
   display.print( "Waiting for data from slaves..." );
   display.display();

} // OLEDdisplayForMasterInit


void OLEDdisplayForWebServer()
{
   OLEDdisplayForIPAddress(); 
   display.setCursor(0,30);
   display.print( "New data logged!" );
   display.setCursor(0,40);
   display.print( "#" + String(loggedDataCounter) );
   display.setCursor(0,50);
   char time_output[30];
   getTextFormatTime(time_output);
   display.print( time_output );
   display.display();

} // OLEDdisplayForWebServer