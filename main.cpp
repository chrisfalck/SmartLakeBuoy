// Code moved out of the traditional setup() loop() .ino paradigm.
// See http://www.gammon.com.au/forum/?id=12625 for more information.
#include <Arduino.h>
#include "main.h"
#include "Mistmakers_Fona.h"
#include "Mistmakers_Gps.h"

#define newline Serial.println()

#define FONA_RX 5
#define FONA_TX 6
#define FONA_RST 4

SoftwareSerial gpsSS = SoftwareSerial(GPS_TX, GPS_RX);
Adafruit_GPS gps(&gpsSS);

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

FonaEntity fonaEntity = {
	&fonaSS,
	&fona
};

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
	char c = gps.read();
	// if you want to debug, this is a good time to do it!
#ifdef UDR0
	if (GPSECHO)
		if (c) UDR0 = c;
	// writing direct to UDR0 is much much faster than Serial.print 
	// but only one character can be written at a time. 
#endif
}

int main() {
 
	// Arduino initializations. 
	init();
	#if defined(USBCON)
		USBDevice.attach();
	#endif
	Serial.begin(115200);

	// Peripheral initializations. 
	initializeFona(&fonaEntity);

	// Looping calls. 
	while (1) {
		fonaLoop();
		Serial.flush();
	}
}

void fonaLoop() {

	// Prepare space for a message to be received.
	char message[MESSAGE_MAX_LENGTH] = "Empty";
	char number[NUMBER_MAX_LENGTH] = "Empty";
	SMSMessage messageReceived = {
		message,
		number	
	};

	// See if we have any new messages stored in the phona. 
	// If we don't, messageReceived.message will be "Empty".
	checkForMessage(&messageReceived, fonaEntity.fona);

	if (strcmp(*(messageReceived.message), "Empty") != 0) {
		Serial.println(*(messageReceived.message));
		if (sendMessageToNumber(&fonaEntity, &messageReceived, &gps)) {
			newline;
			Serial.println(F("Reply sent successfully."));
			newline;
		} else {
			newline;
			Serial.println(F("Reply not sent successfully."));
			newline;
		}
	}
}

