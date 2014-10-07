#include "Arduino.h"

int ledPin = 13;    // LED connected to digital pin 13

// The setup() method runs once, when the sketch starts
void setup() {
	// initialize the digital pin as an output:
	pinMode(ledPin, OUTPUT);
}

// the loop() method runs over and over again,
// as long as the Arduino has power

void loop() {
	digitalWrite(ledPin, HIGH);   // set the LED on
	delay(1000);                  // wait for a second
	digitalWrite(ledPin, LOW);    // set the LED off
	delay(1000);                  // wait for a second
}

int main(void) {

	init();
	setup();

	while (true) {
		loop();
	}
}
