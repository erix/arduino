/*
 * IRanalyzer.pde
 * based on http://arduino.cc/playground/Code/InfraredReceivers
 *
 * modified by Darell Tan
 * see http://irq5.wordpress.com/2012/07/27/infrared-remote-control-protocols-part-1/
 *
 * modified by Erik Simko for Mitsubishi AC remote
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#define PIN				2
#define READ_IR        ((PIND & _BV(2)) >> 2)  // IR pin
#define TIMER_RESET    TCNT1 = 0
#define TIMER_OVF      (TIFR1 & _BV(TOV1))
#define TIMER_CLR_OVF  TIFR1 |= _BV(TOV1)

#define PRESCALE_MULTIPLIER 4
#define PRESCALER 0x03 // 64 prescaler

// Bit timings
#define MAX_SILENCE    5000
#define MARK           550
#define SPACE_0        550
#define SPACE_1        1350

// Global buffer we are goint to work on
#define SAMPLE_SIZE    400
unsigned int timerValues[SAMPLE_SIZE];

// resets the buffer
void resetBuffer() {
	for (int i = 0; i < SAMPLE_SIZE; i++)
		timerValues[i] = 0;
}

// waits for IR transmission to begin
void waitForBegin() {
	while(READ_IR == HIGH);
}

// sample the IR pin for raw timing values
// returns the number of samples
unsigned int readSamples() {
	int i = 0;
	bool cont = true;  // still continue sampling
	char pin_value = LOW;

	while( cont && (i < SAMPLE_SIZE) ) {
		// reset timer, clear overflow flag
		TIMER_RESET;
		TIMER_CLR_OVF;

		while( READ_IR == pin_value ) {
			// if the pin has not changed, check for overflow
			if (TIMER_OVF) {
				cont = false;
				break;
			} else if( (READ_IR == HIGH) && ((TCNT1 * PRESCALE_MULTIPLIER) > MAX_SILENCE) ) {
				// too long silence, the transmission has ended
				cont = false;
				break;
			}
		}
		if (cont) timerValues[i++] = TCNT1 * 4;
		pin_value = !pin_value;
	}
	return i;
}

// decodes a byte from timing samples
// returns the decoded byte
byte decodeByte(unsigned int *values) {
	byte b = 0;
	for(int i = 0; i < 8; i++) {
		unsigned int time0 = values[2 * i];
		unsigned int time1 = values[2 * i + 1];

		if( time0 < MARK ) {
			// we have correct MARK
			// check for space length
			if( time1 < SPACE_0) {
				// 0 bit
			} else if( time1 < SPACE_1) {
				// 1 bit -> build up the byte
				b |= 1 << i;
			}
		}
	}
	return b;
}

// helper function to print integer as hex values
void print_hex(int v, int num_places)
{
	int mask=0, n, num_nibbles, digit;

	for (n=1; n<=num_places; n++)
	{
		mask = (mask << 1) | 0x0001;
	}
	v = v & mask; // truncate v to specified number of places

	num_nibbles = num_places / 4;
	if ((num_places % 4) != 0)
	{
		++num_nibbles;
	}

	do
	{
		digit = ((v >> (num_nibbles-1) * 4)) & 0x0f;
		Serial.print(digit, HEX);
	} while(--num_nibbles);

}

// converts the sample timings to HEX values and prints it to serial
void timingsToHex(int samplesCount) {
	int i = 0;
	byte b = 0;

	while (i < samplesCount) {
		if (timerValues[i] > 1500) {
			// skip the header
			i++;
			continue;
		}

		// each bit is has 2 timing values: MARK and SPACE
		if( (i + 16) < samplesCount) {
			// we have at least 16 samples left to process a byte
			b = decodeByte(timerValues + i);
			print_hex(b, 8);
			i += 16; // consumed 16 samples
		} else {
			//Stop decoding
			Serial.println();
			break;
		}
	}
}

// setup
void setup() {
	Serial.begin(9600);
	Serial.println("Analyze IR Remote");

	// set up the timers
	TCCR1A = 0x00;
	TCCR1B = PRESCALER;
	TIMSK1 = 0x00;

	pinMode(PIN, INPUT);  // IR pin here
}

// main loop
void loop() {
	unsigned int samplesCount = 0;
	resetBuffer();
	waitForBegin();
	samplesCount = readSamples();
	timingsToHex(samplesCount);
}

