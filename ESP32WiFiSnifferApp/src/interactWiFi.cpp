/* Header file belonging to this implementation */
#include "interactWiFi.hpp"

/* External heaader files */
//...

/* Project heaader files */
#include "interactOLED.hpp"


int connectTime = 10;        
boolean connectStatus = false;


// WiFi connection related helper functions

String translateEncryptionType(wifi_auth_mode_t encryptionType)
{
    switch (encryptionType)
    {
        case (WIFI_AUTH_OPEN):
            return "Open";
        case (WIFI_AUTH_WEP):
            return "WEP";
        case (WIFI_AUTH_WPA_PSK):
            return "WPA_PSK";
        case (WIFI_AUTH_WPA2_PSK):
            return "WPA2_PSK";
        case (WIFI_AUTH_WPA_WPA2_PSK):
            return "WPA_WPA2_PSK";
        case (WIFI_AUTH_WPA2_ENTERPRISE):
            return "WPA2_ENTERPRISE";
        default:
            return String();
    }

} // translateEncryptionType


void scanWiFiNetworks()
{
    Serial.println("WiFi scan start.");
    
    int numberOfNetworks = WiFi.scanNetworks();
    
    Serial.println("WiFi Scan done.");
    
    if (numberOfNetworks == 0)
    {
        Serial.println("No networks found");
    }
    else
    {
        Serial.println("\n\n");
        Serial.print("Number of networks nearby found: ");
        Serial.println(numberOfNetworks);
        Serial.println("-----------------------");
    
        for (int i = 0; i < numberOfNetworks; i++)
        {
            Serial.println(i + 1);
            Serial.print("Network name: ");
            Serial.println(WiFi.SSID(i));
        
            Serial.print("Signal strength: ");
            Serial.println(WiFi.RSSI(i));
        
            Serial.print("MAC address: ");
            Serial.println(WiFi.BSSIDstr(i));
        
            Serial.print("Encryption type: ");
            String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
            Serial.println(encryptionTypeDescription);
            Serial.println("-----------------------");
        }
        
        Serial.println("");
        delay(5000);
    }
    
} // scanWiFiNetworks


void connectToWiFi( const char* ssid, const char* password )
{
    // Connect to your WiFi modem
    WiFi.begin(ssid, password);

    pinMode(LEDPin, OUTPUT);
    
    Serial.println();
    Serial.print( "Waiting for WiFi to connect..." );
    OLEDdisplayForWiFiConnection();

    // Check if successfully connected to WiFi network  
    while ( WiFi.status() != WL_CONNECTED )
    {
        delay(500);
        Serial.print(".");  
        connectStatus = !connectStatus;
        if (connectStatus)
        {
            digitalWrite(LEDPin,HIGH);
        }
        else
        {
            digitalWrite(LEDPin,LOW);
        }

        if( (connectTime--) == 0)
        {
            Serial.println();
            Serial.println( "ERROR - WiFi connection failed!" );
            ESP.restart();
        }
    }
    
    Serial.println();
    Serial.println((String) "Successfully connecting to " + ssid + "!");

    for (int i = 0; i < 5; i++)
    {
        digitalWrite(LEDPin,HIGH);
        delay(300);
        digitalWrite(LEDPin,LOW);
        delay(300);
    }

    delay(1000);
    digitalWrite(LEDPin,HIGH);     
    delay(1000);

} // connectToWiFiNetwork


void disconnectWiFi()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    
} // disconnectWiFi
