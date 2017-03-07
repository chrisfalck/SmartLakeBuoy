#include "Mistmakers_Fona.h"
#include "Mistmakers_Gps.h"

#define newline Serial.println()

int sendMessageToNumber(FonaEntity* fonaEntity, SMSMessage* messageInfo, Adafruit_GPS* gps) {

	char* location;
	while (strstr(location, "lat:") == NULL) {
		location = getLocation(gps);
	}

	initializeFona(fonaEntity);

	// Attempt to send a text message.
	if (!fonaEntity->fona->sendSMS(*(messageInfo->number), location)) {
		return 0;
	} else {
		return 1;
	}
}

void initializeFona(FonaEntity* fonaEntity) {

	// Make it slow so its easy to read.
	fonaEntity->fonaSS->begin(4800);
	if (!(fonaEntity->fona->begin(*(fonaEntity->fonaSS)))) {
		Serial.println(F("Couldn't find FONA"));
		while (1);
	}

	newline;
	Serial.println("FONA Ready");
	newline;
}

void checkForMessage(SMSMessage* message, Adafruit_FONA* fona) {

	// For messages from the fona->
	char fonaInBuffer[64];

	// Fona buffer pointer. 
	char* bufPtr = fonaInBuffer;

	if (!(fona->available())) {
		return;
	}

	// This will be the slot number of the SMS.
	int slot = 0;
	int charCount = 0;

	// Read the notification into fonaInBuffer.
	do {
		*bufPtr = fona->read();
		Serial.write(*bufPtr);
		delay(1);
	} while ((*bufPtr++ != '\n') && (fona->available()) && (++charCount < (sizeof(fonaInBuffer) - 1)));

	// Add a terminal NULL to the notification string.
	*bufPtr = 0;

	// Scan the notification string for an SMS received notification.
	// If it's an SMS message, we'll get the slot number in 'slot'.
	if (1 == sscanf(fonaInBuffer, "+CMTI: \"SM\",%d", &slot)) {

		// We'll store the SMS sender number in here.
		char callerIDbuffer[32];

		// Retrieve SMS sender address/phone number.
		int receiveSuccess = fona->getSMSSender(slot, callerIDbuffer, 31);
		if (!receiveSuccess) {
			Serial.println("Didn't find SMS message in slot!");
		}

		// Declare space to hold the number we received a message from.
		char number[NUMBER_LENGTH];
		if (receiveSuccess) {
			for (int i = 0; i < NUMBER_LENGTH; ++i) { number[i] = callerIDbuffer[i]; }
		}
		else {
			char defaultNumber[12] = "13528577310";
			for (int i = 0; i < 12; ++i) { number[i] = defaultNumber[i]; }
			for (int i = 12; i < NUMBER_LENGTH; ++i) { number[i] = '\0'; }
		}

		newline;
		Serial.println("Received message from: ");
		Serial.println(number);
		newline;

		newline;
		Serial.println("Message received from sender:");
		char userMessage[MESSAGE_LENGTH];
		uint16_t userMessageLength;
		fona->readSMS(slot, userMessage, MESSAGE_LENGTH, &userMessageLength);
		newline;

		// Write the user message and number into the locations pointed to
		// on the message struct we were passed.
		*(message->message) = userMessage;
		*(message->number) = number;

		// Clean up.
		for (int i = 0; i < NUM_SMS_TO_DELETE; ++i) {
			fona->deleteSMS(i);
		}

	}
}
