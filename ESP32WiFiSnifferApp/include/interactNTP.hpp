#pragma once

/* External heaader files */
#include <time.h>

/* Project heaader files */
#include "interactWiFi.hpp"


extern const char* NTP_SERVER;
extern const char* TZ_INFO;


extern struct tm timeinfo;
extern struct timeval tv;
// Unix Time in seconds
extern time_t now;
// Unix Time in milliseconds
extern uint64_t time_ms;
// Unix Time in microseconds
extern uint64_t time_us;
// Millisecond fraction
extern uint64_t ms_frac;
// Microsecond fraction
extern uint64_t us_frac;


bool callNTPServer(int sec);
uint64_t getUnixTimeinMilliseconds();
uint64_t getUnixTimeinMicroseconds();
void getTextFormatTime(char *buf);
void getTextFormatTimeinMilliseconds(char *buf);
void getTextFormatTimeinMicroseconds(char *buf);
void printLocalTime(tm localTime);