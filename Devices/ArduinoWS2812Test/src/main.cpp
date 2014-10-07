#include "Arduino.h"

extern "C" {
#include "light_ws2812.h"
}

#define LED_COUNT 30

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2

struct cRGB led[LED_COUNT];


void set_led(uint8_t color, uint8_t count, uint8_t brightness) {
	for(int i=0; i < LED_COUNT; i++) {
		led[i].r=0;
		led[i].g=0;
		led[i].b=0;
		if (i <= count) {
			switch(color) {
				case COLOR_RED : led[i].r = brightness; break;
				case COLOR_GREEN: led[i].g = brightness; break;
				case COLOR_BLUE: led[i].b = brightness; break;
			}
		}
	}
}

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

	  while(1) {
		  for(int i=0; i < LED_COUNT;i++) {
			  set_led(COLOR_BLUE, i,50);
			   ws2812_setleds(led,LED_COUNT);
		  }
		  delay(3000);
		  for(int i=0; i < LED_COUNT;i++) {
			  set_led(COLOR_GREEN, i,50);
			   ws2812_setleds(led,LED_COUNT);
		  }
		  delay(3000);
		  for(int i=0; i < LED_COUNT;i++) {
			  set_led(COLOR_RED, i,50);
			   ws2812_setleds(led,LED_COUNT);
		  }
		  delay(3000);
	  }
}

int main(void) {

	init();
	setup();

	while (true) {
		loop();
	}
}
