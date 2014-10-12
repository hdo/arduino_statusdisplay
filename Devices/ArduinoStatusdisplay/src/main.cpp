#include "Arduino.h"
#include "version.h"

extern "C" {
#include "light_ws2812.h"
#include <avr/io.h>
#include <avr/interrupt.h>
}

#define LED_COUNT 30
#define SERIAL_BUFFER_SIZE 32
#define LED_NORMAL 0
#define LED_BLINK 1

struct cRGB led[LED_COUNT]; //cRGB is organized G, R, B
struct cRGB led_backup[LED_COUNT];

uint8_t led_mode[LED_COUNT];

uint8_t serialdata[SERIAL_BUFFER_SIZE];
uint8_t serialcount = 0;
uint8_t processserial = 0;


int ledPin = 13;    // LED connected to digital pin 13

volatile uint32_t ticks;
uint32_t last_heartbeat_blink = 0;
uint32_t last_led_blink = 0;
uint8_t heartbeat_led_on = 0;
uint8_t blink_on = 0;
uint8_t dirty = 0;

ISR(TIMER2_COMPA_vect, ISR_BLOCK) {

	ticks++;
}


void init_systicks() {
	TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20); // prescale 1024
	TCCR2A = _BV(WGM21);    // MODE CTC
	TIMSK2 = (1 << OCIE2A); // enable compare match interrupt

	// 8000000/1024/78 == 100HZ -> 10 ms
	OCR2A = 77; // !!! must me set last or it will not work!
}

uint32_t math_calc_diff(uint32_t value1, uint32_t value2) {
	if (value1 == value2) {
		return 0;
	}
	if (value1 > value2) {
		return (value1 - value2);
	}
	else {
		// check for overflow
		return (0xffffffff - value2 + value1);
	}
}
void update_leds() {
    ws2812_setleds(led, LED_COUNT);
}

void init_leds() {
	for(int i=0; i < LED_COUNT; i++) {
		led[i].r=0;
		led[i].g=0;
		led[i].b=0;
		led_mode[i] = LED_NORMAL;
	}
}

void set_led(uint8_t index, cRGB value) {
	if (index < LED_COUNT) {
		led[index] = value;
		led_backup[index] = value;
	}
}

void set_temp_led(uint8_t index, cRGB value) {
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
	init_systicks();
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
	// 3-4: <led index as HEX> (0 - led count), 0xFF means all leds
	// 5-6: <value for R as HEX>
	// 7-8: <value for G as HEX>
	// 9-10: <value for B as HEX>
	//
	// example: \ 01 02 50 50 50 \n

	if (serialcount > 10) {
		int16_t led_index = parse_hex_byte(serialdata[3], serialdata[4]);

		uint8_t ledmode = LED_NORMAL;
		if (serialcount > 12) {
			int16_t value_mode = parse_hex_byte(serialdata[11], serialdata[12]);
			switch(value_mode) {
			case 0 : ledmode = LED_NORMAL; break;
			case 1 : ledmode = LED_BLINK; break;
			}
		}

		// check whether command is valid command
		if (led_index > -1 && led_index < LED_COUNT) {
			int16_t value_r = parse_hex_byte(serialdata[5], serialdata[6]);
			int16_t value_g = parse_hex_byte(serialdata[7], serialdata[8]);
			int16_t value_b = parse_hex_byte(serialdata[9], serialdata[10]);

			if (value_r > -1 && value_g > -1 && value_b > -1) {
				cRGB value = {value_g, value_r, value_b}; // see cRGB definition (G,R,B)
				set_led(led_index, value);
				led_mode[led_index] = ledmode;
				return 1;
			}
		}

		if (led_index == 0xff) {
			int16_t value_r = parse_hex_byte(serialdata[5], serialdata[6]);
			int16_t value_g = parse_hex_byte(serialdata[7], serialdata[8]);
			int16_t value_b = parse_hex_byte(serialdata[9], serialdata[10]);
			if (value_r > -1 && value_g > -1 && value_b > -1) {
				cRGB value = {value_g, value_r, value_b}; // see cRGB definition (G,R,B)
				for(uint8_t i=0; i < LED_COUNT; i++) {
					set_led(i, value);
					led_mode[i] = ledmode;
				}
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

	sei(); // enable interrupts

	while (true) {

		// heart beat each second
		if (math_calc_diff(ticks, last_heartbeat_blink) > 100) {
			last_heartbeat_blink = ticks;
 			if (heartbeat_led_on == 1) {
 				heartbeat_led_on = 0;
 				digitalWrite(ledPin, HIGH);   // set the LED on
 			}
 			else {
 				heartbeat_led_on = 1;
 				digitalWrite(ledPin, LOW);   // set the LED on
 			}
		}

		if (math_calc_diff(ticks, last_led_blink) > 50) {
			last_led_blink = ticks;

			if (blink_on == 1) {
 				blink_on = 0;

 				// BLINK LED ON
				for(uint8_t i=0; i < LED_COUNT; i++) {
					if (led_mode[i] == LED_BLINK) {
						set_temp_led(i, led_backup[i]);
						dirty = 1;
					}
				}
 			}
 			else {
 				blink_on = 1;

 				// BLINK LED OFF
 				cRGB value = {0, 0, 0}; // black = led off
				for(uint8_t i=0; i < LED_COUNT; i++) {
					if (led_mode[i] == LED_BLINK) {
						set_temp_led(i, value);
						dirty = 1;
					}
				}
 			}
		}

		if (Serial.available() > 0) {
			uint8_t data = Serial.read();
			Serial.write(data);
			//Serial.print(data);

			// if busy then ignore all incoming serial data
			if (processserial) {
				continue;
			}

			// check for line feed
			if (data == 0x0a || data == 0x0d) {
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
			if (serialdata[0] == '\\' && serialcount >= 2) {

				if (serialdata[1] == 'i') {
					Serial.print("PRODUCT:");
					Serial.println(VERSION_PRODUCT_NAME);

					Serial.print("HARDWARE VERSION:");
					Serial.println(VERSION_HARDWARE_VERSION);

					Serial.print("FIRMWARE VERSION:");
					Serial.println(VERSION_FIRMWARE_VERSION);

					Serial.print("FIRMWARE BUILD:");
					Serial.println(VERSION_FIRMWARE_BUILD);

 				} else if (serialdata[1] == 'v') {
					Serial.print("FIRMWARE VERSION:");
					Serial.println(VERSION_FIRMWARE_VERSION);
 				} else if (serialdata[1] == 'h') {
					Serial.print("HARDWARE VERSION:");
					Serial.println(VERSION_HARDWARE_VERSION);
 				} else if (serialdata[1] == 'b') {
					Serial.print("FIRMWARE BUILD:");
					Serial.println(VERSION_FIRMWARE_BUILD);
 				} else {
 					int16_t command = parse_hex_byte(serialdata[1], serialdata[2]);
 					if (command > -1) {
 						switch (command) {
 						case 1: ret = parse_set_led_command(); break;
 						//case 2: update_leds(); break;
 						}
 					}
 				}
			}
			// clear serial data
			serialcount = 0;
			processserial = 0;
			if (ret) {
				dirty = 1;
			}
		}

		if (dirty) {
			dirty = 0;
			update_leds();
		}
	}
}
