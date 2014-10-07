#include "Arduino.h"

extern "C" {
#include "light_ws2812.h"
}

#define LED_COUNT 30
#define SERIAL_BUFFER_SIZE 32

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2

struct cRGB led[LED_COUNT];

uint8_t serialdata[SERIAL_BUFFER_SIZE];
uint8_t serialcount = 0;
uint8_t processserial = 0;


int ledPin = 13;    // LED connected to digital pin 13


void init_leds() {
	for(int i=0; i < LED_COUNT; i++) {
		led[i].r=0;
		led[i].g=0;
		led[i].b=0;
	}
}

void set_led(uint8_t index, cRGB value) {
	if (index < LED_COUNT) {
		led[index] = value;
	}
}

void setup() {
	// initialize the digital pin as an output:
	pinMode(ledPin, OUTPUT);
	Serial.begin(38400); // 0.2% error rate at 8 MHZ
	init_leds();
    ws2812_setleds(led, LED_COUNT);
}

uint8_t parse_set_led_command() {
	// 0: '\\'
	// 1: '1' (set led command)
	// 2: <channel> (0 - led count)
	// 3: <value for R>
	// 4: <value for G>
	// 5: <value for B>

	uint8_t index = serialdata[2];
	if (serialcount >= 6) {
		// invalid command
		if (index < LED_COUNT) {
			cRGB value = {serialdata[3], serialdata[4], serialdata[5]};
			set_led(index, value);
		}
		return 1;
	}
	else {
		// invalid command
	}
	return 0;
}

int main(void) {

	init();
	setup();

	Serial.println("Welcome to Statusdisplay!");

	digitalWrite(ledPin, HIGH);   // set the LED on
	delay(1000);                  // wait for a second
	digitalWrite(ledPin, LOW);    // set the LED off
	delay(1000);                  // wait for a second

	while (true) {
		if (Serial.available() > 0) {
			uint8_t data = Serial.read();

			// if busy then ignore all incoming serial data
			if (processserial) {
				continue;
			}

			// check for line feed
			if (data == 0x0a) {
				processserial = 1;
			}
			else {
				if (serialcount < SERIAL_BUFFER_SIZE) {
					serialdata[serialcount++] = data;
				}
			}
		}
		if (processserial) {
			uint8_t ret = 0;
			// every valid command begins with '\'
			if (serialdata[0] == '\\') {
				switch (serialdata[1]) {
				case '1': ret = parse_set_led_command(); break;
				}
			}
			// clear serial data
			serialcount = 0;
			processserial = 0;
			if (ret) {
				  ws2812_setleds(led, LED_COUNT);
			}
		}
	}
}
