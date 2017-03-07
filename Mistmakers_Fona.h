#ifndef _MISTMAKERS_FONA_h
#define _MISTMAKERS_FONA_h

#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include "Adafruit_GPS.h"

// Declare typedefs so we dont' have to keep saying struct. 
typedef struct FonaEntity FonaEntity;
typedef struct SMSMessage SMSMessage;

// Standard message struct.
struct SMSMessage {
	char* message[140];
	char* number[32];
};

// Fona struct holding the fona instance and fona serial.
struct FonaEntity {
	SoftwareSerial* fonaSS;
	Adafruit_FONA* fona;
};

// Send a message struct.
int sendMessageToNumber(FonaEntity* fonaEntity, SMSMessage* messageInfo, Adafruit_GPS* gps);

// Initialize the fona. 
void initializeFona(FonaEntity* fonaEntity);

// Poll for a message on each loop iteration. 
void checkForMessage(SMSMessage* message, Adafruit_FONA* fona);

#endif