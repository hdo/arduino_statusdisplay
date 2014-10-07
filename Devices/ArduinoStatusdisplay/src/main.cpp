#include "Arduino.h"

extern "C" {
#include "light_ws2812.h"
}

#define LED_COUNT 30
#define SERIAL_BUFFER_SIZE 32

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2

struct cRGB led[LED_COUNT]; //cRGB is organized G, R, B

uint8_t serialdata[SERIAL_BUFFER_SIZE];
uint8_t serialcount = 0;
uint8_t processserial = 0;


int ledPin = 13;    // LED connected to digital pin 13


void update_leds() {
    ws2812_setleds(led, LED_COUNT);
}

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
	Serial.begin(38400); // 0.2% error rate at 8 MHZ (see datasheet)
	init_leds();
	update_leds();
}

int16_t parse_nibble(uint8_t nibble) {
	if (nibble >= '0' && nibble <= '9') {
		return nibble - '0';
	}
	if (nibble >= 'a' && nibble <= 'f') {
		return (nibble - 'a') + 10;
	}
	if (nibble >= 'A' && nibble <= 'F') {
		return (nibble - 'A') + 10;
	}
	return -1;
}

int16_t parse_hex_byte(uint8_t n1, uint8_t n0) {
	int16_t parsed_n1 = parse_nibble(n1);
	int16_t parsed_n0 = parse_nibble(n0);
	if (parsed_n1 > -1 && parsed_n0 > -1) {
		uint8_t ret = (0x0f & (uint8_t) parsed_n1) << 4;
		ret |= (0x0f & (uint8_t) parsed_n0);
		return ret;
	}
	return -1;
}

uint8_t parse_set_led_command() {
	// 0: '\\'
	// 1-2: <command as HEX> 1 (set led command)
	// 3-4: <channel as HEX> (0 - led count)
	// 5-6: <value for R as HEX>
	// 7-8: <value for G as HEX>
	// 9-10: <value for B as HEX>
	//
	// example: \ 01 02 50 50 50 \n

	if (serialcount >= 10) {
		uint8_t led_index = parse_hex_byte(serialdata[3], serialdata[4]);

		// check whether command is valid command
		if (led_index > -1 && led_index < LED_COUNT) {
			uint8_t value_r = parse_hex_byte(serialdata[5], serialdata[6]);
			uint8_t value_g = parse_hex_byte(serialdata[7], serialdata[8]);
			uint8_t value_b = parse_hex_byte(serialdata[9], serialdata[10]);

			if (value_r > -1 && value_g > -1 && value_b > -1) {
				cRGB value = {value_g, value_r, value_b}; // see cRGB definition (G,R,B)
				set_led(led_index, value);
				return 1;
			}
		}
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
			if (serialdata[0] == '\\' && serialcount > 2) {

				int16_t command = parse_hex_byte(serialdata[1], serialdata[2]);
				if (command > -1) {
					switch (command) {
					case 1: ret = parse_set_led_command(); break;
					}
				}
			}
			// clear serial data
			serialcount = 0;
			processserial = 0;
			if (ret) {
				update_leds();
			}
		}
	}
}
