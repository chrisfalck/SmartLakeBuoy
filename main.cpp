// Code moved out of the traditional setup() loop() .ino paradigm.
// See http://www.gammon.com.au/forum/?id=12625 for more information.
#include <Arduino.h>
#include "main.h"
#include "Mistmakers_Fona.h"

#define newline Serial.println()

uint8_t FONA_RX = 5; 
uint8_t FONA_TX = 6;
uint8_t FONA_RST = 4; 

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
	setupFona();

	// Looping calls. 
	while (1) {
		fonaLoop();
		Serial.flush();
	}
}

void setupFona() {
	Serial.println("Initializing FONA.");
	Serial.flush();
	initializeFona(&fonaEntity);
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

