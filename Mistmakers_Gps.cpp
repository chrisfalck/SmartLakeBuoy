#include "Mistmakers_Gps.h"

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

void initializeGps(Adafruit_GPS* gps) {
	gps->sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	gps->sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
	gps->sendCommand(PGCMD_ANTENNA);
	useInterrupt(true);
}

char* getLocation(Adafruit_GPS* gps) {
	initializeGps(gps);

	gps->begin(9600);
	delay(1000);

	boolean shouldContinue = true;
	if (gps->newNMEAreceived()) {
		if (!gps->parse(gps->lastNMEA())) return;
	}
	if (gps->fix) {

		Serial.print("\nTime: ");
		Serial.print(gps->hour, DEC); Serial.print(':');
		Serial.print(gps->minute, DEC); Serial.print(':');
		Serial.print(gps->seconds, DEC); Serial.print('.');
		Serial.println(gps->milliseconds);
		Serial.print("Date: ");
		Serial.print(gps->day, DEC); Serial.print('/');
		Serial.print(gps->month, DEC); Serial.print("/20");
		Serial.println(gps->year, DEC);
		Serial.print("Location: ");
		Serial.print(gps->latitudeDegrees, 4);
		Serial.print(", ");
		Serial.println(gps->longitudeDegrees, 4);

		float latDeg = gps->latitudeDegrees;
		float longDeg = gps->longitudeDegrees;

		char location[32];
		char tempStr[16];

		// http://stackoverflow.com/questions/905928/using-floats-with-sprintf-in-embedded-c
		// Format the lat and long floats so we can use them as strings.

		// Latitude to string.
		char *tmpSign = (latDeg < 0) ? "-" : "";
		float tmpVal = (latDeg < 0) ? -latDeg : latDeg;
		int tmpInt1 = tmpVal;                  // Get the integer.
		float tmpFrac = tmpVal - tmpInt1;      // Get fraction.
		int tmpInt2 = trunc(tmpFrac * 10000);  // Turn into integer.
		// Print as parts, note that you need 0-padding for fractional bit.
		sprintf(tempStr, "lat: %s%d.%04d\n", tmpSign, tmpInt1, tmpInt2);
		strcpy(location, tempStr);

		// Repeat for longitude.
		tmpSign = (longDeg < 0) ? "-" : "";
		tmpVal = (longDeg < 0) ? -longDeg : longDeg;
		tmpInt1 = tmpVal;
		tmpFrac = tmpVal - tmpInt1;
		tmpInt2 = trunc(tmpFrac * 10000);
		sprintf(tempStr, "long: %s%d.%04d", tmpSign, tmpInt1, tmpInt2);
		strcat(location, tempStr);

		Serial.println("Read from GPS:");
		Serial.println(location);

		return location;
	}
	else {
		Serial.println("No gps fix.");
		return nullptr;
	}
}