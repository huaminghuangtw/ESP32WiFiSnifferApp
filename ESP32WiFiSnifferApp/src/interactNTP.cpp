/* Header file belonging to this implementation */
#include "interactNTP.hpp"

/* External heaader files */
//...

/* Project heaader files */
#include "generalFunctions.hpp"


const char* NTP_SERVER = "pool.ntp.org";
const char* TZ_INFO = "CET-1CEST,M3.5.0,M10.5.0/3"; // Time zone in Berlin, Germany
                                                    // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for timezone codes for your region


struct tm timeinfo;
struct timeval tv;
// Unix Time in seconds
time_t now;
// Unix Time in milliseconds
uint64_t time_ms;
// Unix Time in microseconds
uint64_t time_us;
// Millisecond fraction
uint64_t ms_frac;
// Microsecond fraction
uint64_t us_frac;


bool callNTPServer(int sec)
{
    {
        uint32_t start = millis();
        
        // Get the time
        //Serial.print( F("Waiting for system time to be set...") );
        do
        {
            time(&now);
            localtime_r(&now, &timeinfo);         // update fields in the strcuture tm
            if ( gettimeofday(&tv, NULL) != 0 )   // gettimeofday() return -1 for failure, 0 for success
            {
                Serial.println( F("ERROR - Failed to obtain time!") );
                ESP.restart();
                return false;
            }
            delay(10);
        } while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));

        if (timeinfo.tm_year <= (2016 - 1900))
        {
            Serial.println( F("ERROR - Failed to obtain time!") );
            ESP.restart();
            return false;
        }
    }

    return true;

} // callNTPServer


uint64_t getUnixTimeinMilliseconds()   // e.g., 1610279529468
{
    if ( callNTPServer(10) )   // wait up to 10 sec to sync
    {
        Serial.print( F("NTP Server got called: ") );
        char time_output[30];
        getTextFormatTime(time_output);
        Serial.println(time_output);

        time_ms = (uint64_t) tv.tv_sec * 1000LL + (uint64_t) tv.tv_usec / 1000LL;
        return time_ms;
    }

  return (uint64_t) 0;   // failed to synchronize time

} // getUnixTimeinMilliseconds


uint64_t getUnixTimeinMicroseconds()   // e.g., 1610279529468548
{
    if ( callNTPServer(10) )   // wait up to 10 sec to sync
    {
        Serial.print( F("NTP Server got called: ") );
        char time_output[30];
        getTextFormatTime(time_output);
        Serial.println(time_output);

        time_us = (uint64_t) tv.tv_sec * 1000000L + (uint64_t) tv.tv_usec;
        return time_us;
    }

    return (uint64_t) 0;   // failed to synchronize time

} // getUnixTimeinMicroseconds


void getTextFormatTime(char *buf)   // e.g., 2001-08-23 14:55:02
{
    if ( callNTPServer(10) )   // wait up to 10 sec to sync
    {
        strftime(buf, 30, "%F %T", localtime(&now));    // For more format specifiers of the corresponding values to represent time,
                                                        // See: http://www.cplusplus.com/reference/ctime/strftime/
    }

} // getTextFormatTime


void getTextFormatTimeinMilliseconds(char *buf)   // e.g., 2001-08-23 14:55:02.583
{
    if ( callNTPServer(10) )   // wait up to 10 sec to sync
    {
        strftime(buf, 20, "%F %T", localtime(&now));    // For more format specifiers of the corresponding values to represent time,
                                                        // See: http://www.cplusplus.com/reference/ctime/strftime/
    }

    strcat( buf, "." );

    ms_frac = (uint64_t) tv.tv_usec / 1000LL;
    char arr_ms_frac[4] = {0};
    strcpy( arr_ms_frac, uint64_to_string( ms_frac ).c_str() );
    strcat( buf, arr_ms_frac );

} // getTextFormatTimeinMilliseconds


void getTextFormatTimeinMicroseconds(char *buf)   // e.g., 2001-08-23 14:55:02.583546
{
    if ( callNTPServer(10) )   // wait up to 10 sec to sync
    {
        strftime(buf, 20, "%F %T", localtime(&now));  	// For more format specifiers of the corresponding values to represent time,
                                                        // See: http://www.cplusplus.com/reference/ctime/strftime/
    }

    strcat( buf, "." );

    us_frac = (uint64_t) tv.tv_usec;
    char arr_us_frac[4] = {0};
    strcpy( arr_us_frac, uint64_to_string( us_frac ).c_str() );
    strcat( buf, arr_us_frac );

} // getTextFormatTimeinMicroseconds


void printLocalTime(tm localTime)
{
    if ( callNTPServer(10) )   // wait up to 10 sec to sync
    {
        Serial.print("Now: ");
        char time_output[30];
        getTextFormatTime(time_output);
        Serial.println(time_output);

        // Unix Time in sec, msec, usec
        Serial.println(); 

        Serial.println("Unix Time: ");
        Serial.print("TimeVal-sec  = ");
        Serial.println(now);  

        Serial.print("TimeVal-msec = ");
        time_ms = (uint64_t) tv.tv_sec * 1000LL + (uint64_t) tv.tv_usec / 1000LL;
        print_uint64_t(time_ms);
        Serial.println();

        Serial.print("TimeVal-usec = ");
        time_us = (uint64_t) tv.tv_sec * 1000000L + (uint64_t) tv.tv_usec;
        print_uint64_t(time_us);

        Serial.println("\n\n");
    }

} // printLocalTime