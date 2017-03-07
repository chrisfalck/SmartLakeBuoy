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

// true for debugging off for production. Enables echoing to console.
#define GPSECHO false

SoftwareSerial gpsSS = SoftwareSerial(GPS_TX, GPS_RX);
Adafruit_GPS gps(&gpsSS);

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

FonaEntity fonaEntity = {
	&fonaSS,
	&fona
};


// GPS setup.
boolean usingInterrupt = false;
void useInterrupt(boolean v) {
	if (v) {
		// Timer0 is already used for millis() - we'll just interrupt somewhere
		// in the middle and call the "Compare A" function above
		OCR0A = 0xAF;
		TIMSK0 |= _BV(OCIE0A);
		usingInterrupt = true;
	}
	else {
		// do not call the interrupt function COMPA anymore
		TIMSK0 &= ~_BV(OCIE0A);
		usingInterrupt = false;
	}
}

int main() {
 
	// Arduino initializations. 
	init();
	#if defined(USBCON)
		USBDevice.attach();
	#endif
	Serial.begin(115200);

	// Peripheral initializations. 
	//initializeFona(&fonaEntity);
	gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
	gps.sendCommand(PGCMD_ANTENNA);
	useInterrupt(true);
	delay(1000);

	// Looping calls. 
	while (1) {
		//fonaLoop();
		gpsLoop();
		Serial.flush();
	}
}

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

void gpsLoop() {
	gps.begin(9600);
	delay(3000);
	if (gps.newNMEAreceived()) {
		if (!gps.parse(gps.lastNMEA())) return;
	}
	Serial.print("\nTime: ");
	Serial.print(gps.hour, DEC); Serial.print(':');
	Serial.print(gps.minute, DEC); Serial.print(':');
	Serial.print(gps.seconds, DEC); Serial.print('.');
	Serial.println(gps.milliseconds);
	Serial.print("Date: ");
	Serial.print(gps.day, DEC); Serial.print('/');
	Serial.print(gps.month, DEC); Serial.print("/20");
	Serial.println(gps.year, DEC);
	//Serial.print("Fix: "); Serial.print((int)gps.fix);
	//Serial.print(" quality: "); Serial.println((int)gps.fixquality);
	if (gps.fix) {
		//Serial.print("Location: ");
		//Serial.print(gps.latitude, 4); Serial.print(gps.lat);
		//Serial.print(", ");
		//Serial.print(gps.longitude, 4); Serial.println(gps.lon);
		Serial.print("Location: ");
		Serial.print(gps.latitudeDegrees, 4);
		Serial.print(", ");
		Serial.println(gps.longitudeDegrees, 4);
		//Serial.print("Speed (knots): "); Serial.println(gps.speed);
		//Serial.print("Angle: "); Serial.println(gps.angle);
		//Serial.print("Altitude: "); Serial.println(gps.altitude);
		//Serial.print("Satellites: "); Serial.println((int)gps.satellites);
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

