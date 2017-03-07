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

#define GPS_RX 7
#define GPS_TX 8

SoftwareSerial* gpsSS = new SoftwareSerial(GPS_TX, GPS_RX);
Adafruit_GPS gps(gpsSS);

SoftwareSerial* fonaSS = new SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA* fona = new Adafruit_FONA(FONA_RST);

FonaEntity fonaEntity = {
	fonaSS,
	fona
};

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
	char message[64] = "Empty";
	char number[32] = "Empty";

	SMSMessage messageReceived = {
		message,
		number	
	};
	checkForMessage(&messageReceived, fonaEntity.fona);

	if (strcmp(*(messageReceived.message), "Empty") != 0) {
		Serial.println(*(messageReceived.message));
		if (sendMessageToNumber(fonaEntity.fona, &messageReceived)) {
			newline;
			Serial.println(F("Reply sent successfully."));
			newline;
		}
		else {
			Serial.println(F("Reply sent successfully."));
		}
	}
}

