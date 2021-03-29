#include "Pulses.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
void startPulses(int pin, uint32_t freq, uint8_t precision) {
#if defined(MDUINO) || defined(MDUINO_PLUS) || defined (ARDBOX) || defined(ARDBOX_HF) || defined(ARDBOX_HF_PLUS) || defined(SPARTAN)
	stopPulses(pin);

	if (freq == 0) {
		return;
	}

	if (precision < 1) {
		precision = 1;
	}

	uint8_t tccr;
	uint16_t prescaler;
	switch (precision) {
		case 1:
			tccr = 5;
			prescaler = 1024;
			break;

		case 2:
			tccr = 4;
			prescaler = 256;
			break;

		case 3:
			tccr = 3;
			prescaler = 64;
			break;

		case 4:
			tccr = 2;
			prescaler = 8;
			break;

		case 5:
		default:
			tccr = 1;
			prescaler = 1;
			break;
	}

	uint16_t cmp = (uint16_t) ((8000000 / (prescaler * freq)) - 1);

	switch (digitalPinToTimer(pin)) {
		case TIMER0A:
			TCCR0A |= 0b01000010;
			TCCR0B |= 0b10000000 | tccr;
			OCR0A = cmp;
			break;

		case TIMER0B:
			TCCR0A |= 0b00010010;
			TCCR0B |= 0b01000000 | tccr;
			OCR0A = cmp;
			break;

		case TIMER1A:
			TCCR1A |= 0b01000000;
			TCCR1B |= 0b00001000 | tccr;
			OCR1A = cmp;
			break;

		case TIMER1B:
			TCCR1A |= 0b00010000;
			TCCR1B |= 0b00001000 | tccr;
			OCR1A = cmp;
			break;

		case TIMER1C:
			TCCR1A |= 0b00000100;
			TCCR1B |= 0b00001000 | tccr;
			OCR1A = cmp;
			break;

#if defined(MDUINO) || defined(MDUINO_PLUS)
		case TIMER2A:
			TCCR2A |= 0b01000010;
			TCCR2B |= 0b10000000 | tccr;
			OCR2A = cmp;
			break;

		case TIMER2B:
			TCCR2A |= 0b00010010;
			TCCR2B |= 0b01000000 | tccr;
			OCR2A = cmp;
			break;
#endif // MDUINO || MDUINO_PLUS

		case TIMER3A:
			TCCR3A |= 0b01000000;
			TCCR3B |= 0b00001000 | tccr;
			OCR3A = cmp;
			break;

		case TIMER3B:
			TCCR3A |= 0b00010000;
			TCCR3B |= 0b00001000 | tccr;
			OCR3A = cmp;
			break;

		case TIMER3C:
			TCCR3A |= 0b00000100;
			TCCR3B |= 0b00001000 | tccr;
			OCR3A = cmp;
			break;

#if defined(MDUINO) || defined(MDUINO_PLUS)
		case TIMER4A:
			TCCR4A |= 0b01000000;
			TCCR4B |= 0b00001000 | tccr;
			OCR4A = cmp;
			break;

		case TIMER4B:
			TCCR4A |= 0b00010000;
			TCCR4B |= 0b00001000 | tccr;
			OCR4A = cmp;
			break;

		case TIMER4C:
			TCCR4A |= 0b00000100;
			TCCR4B |= 0b00001000 | tccr;
			OCR4A = cmp;
			break;

		case TIMER5A:
			TCCR5A |= 0b01000000;
			TCCR5B |= 0b00001000 | tccr;
			OCR5A = cmp;
			break;

		case TIMER5B:
			TCCR5A |= 0b00010000;
			TCCR5B |= 0b00001000 | tccr;
			OCR5A = cmp;
			break;

		case TIMER5C:
			TCCR5A |= 0b00000100;
			TCCR5B |= 0b00001000 | tccr;
			OCR5A = cmp;
			break;
#endif // MDUINO || MDUINO_PLUS
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void stopPulses(int pin) {
#if defined(MDUINO) || defined(MDUINO_PLUS) || defined (ARDBOX) || defined(ARDBOX_HF) || defined(ARDBOX_HF_PLUS) || defined(SPARTAN)
	switch (digitalPinToTimer(pin)) {
		case TIMER0A:
			TCCR0A &= 0b00111100;
			TCCR0B &= 0b01110000;
			break;

		case TIMER0B:
			TCCR0A &= 0b11001100;
			TCCR0B &= 0b10110000;
			break;

		case TIMER1A:
			TCCR1A &= 0b00111100;
			TCCR1B &= 0b11100000;
			break;

		case TIMER1B:
			TCCR1A &= 0b11001100;
			TCCR1B &= 0b11100000;
			break;

		case TIMER1C:
			TCCR1A &= 0b11110000;
			TCCR1B &= 0b11100000;
			break;

#if defined(MDUINO) || defined(MDUINO_PLUS)
		case TIMER2A:
			TCCR2A &= 0b00111100;
			TCCR2B &= 0b01110000;
			break;

		case TIMER2B:
			TCCR2A &= 0b11001100;
			TCCR2B &= 0b10110000;
			break;
#endif // MDUINO || MDUINO_PLUS

		case TIMER3A:
			TCCR3A &= 0b00111100;
			TCCR3B &= 0b11100000;
			break;

		case TIMER3B:
			TCCR3A &= 0b11001100;
			TCCR3B &= 0b11100000;
			break;

		case TIMER3C:
			TCCR3A &= 0b11110000;
			TCCR3B &= 0b11100000;
			break;

#if defined(MDUINO) || defined(MDUINO_PLUS)
		case TIMER4A:
			TCCR4A &= 0b00111100;
			TCCR4B &= 0b11100000;
			break;

		case TIMER4B:
			TCCR4A &= 0b11001100;
			TCCR4B &= 0b11100000;
			break;

		case TIMER4C:
			TCCR4A &= 0b11110000;
			TCCR4B &= 0b11100000;
			break;

		case TIMER5A:
			TCCR5A &= 0b00111100;
			TCCR5B &= 0b11100000;
			break;

		case TIMER5B:
			TCCR5A &= 0b11001100;
			TCCR5B &= 0b11100000;
			break;

		case TIMER5C:
			TCCR5A &= 0b11110000;
			TCCR5B &= 0b11100000;
			break;
#endif // MDUINO || MDUINO_PLUS
	}
#endif
}
