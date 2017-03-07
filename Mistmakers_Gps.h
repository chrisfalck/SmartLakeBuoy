// Mistmakers_Gps.h

#ifndef _MISTMAKERS_GPS_h
#define _MISTMAKERS_GPS_h

#include <Adafruit_GPS.h>

#define GPS_RX 7
#define GPS_TX 8

// true for debugging off for production. Enables echoing to console.
#define GPSECHO false

void useInterrupt(boolean v);

void initializeGps(Adafruit_GPS* gps);

char* getLocation(Adafruit_GPS* gps);

#endif

