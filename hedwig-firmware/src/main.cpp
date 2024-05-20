/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief Harry Potter Hedwig Lamp Controller
 * @version 0.1
 * @date 2024-05-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef F_CPU
#define F_CPU 32000UL // 32 kHz clock speed / 0 prescaler divisor
#endif

#include <util/delay.h> // needed for our delay
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{

	/* Set the Main clock to internal 32kHz oscillator*/
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCULP32K_gc);
	/* Set the Main clock prescaler divisor to 2X and disable the Main clock prescaler */
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm); // Disable prescaler
	/* ensure 20MHz isn't forced on*/
	_PROTECTED_WRITE(CLKCTRL.OSC20MCTRLA, CLKCTRL.OSC20MCTRLA & ~CLKCTRL_RUNSTDBY_bm);

	PORTA.DIRSET = PIN6_bm | PIN3_bm;

	/* Set all pins except the LED pin to pullups*/
	PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
	PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
	PORTA.PIN2CTRL = PORT_PULLUPEN_bm;

	// Pin 3 is the input pin, connected to the switch
	PORTA.PIN7CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;

	sei();

	while (1)
	{
		_delay_ms(1000);

		PORTA.OUTCLR = PIN6_bm;
		PORTA.OUTSET = PIN3_bm;

		_delay_ms(1000);

		PORTA.OUTSET = PIN6_bm;
		PORTA.OUTCLR = PIN3_bm;
	}
}

ISR(PORTA_PORT_vect)
{
	uint8_t flags = VPORTA.INTFLAGS; // read the interrupt flags
	// writing "1" to the interruptflag bit will clear it.
	VPORTA_INTFLAGS = flags; // clear interrupt flags
	PORTA.OUTSET = PIN6_bm;
	PORTA.OUTSET = PIN3_bm;
}