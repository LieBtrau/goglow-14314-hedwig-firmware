/*
 * ATtiny402LowPowerRunning.c
 * These settings (F_CPU 32000UL, 0 prescaler, all GPIOs apart from LED pin set to pullup inc UPDI, force OSC20M off)
 * Result in a processor current draw of 16uA!
 * Created: 05/05/2019 20:46:56
 * Author : Simon, https://hackaday.io/project/165439/logs
 */ 

#ifndef F_CPU
#define F_CPU 32000UL // 32 kHz clock speed / 0 prescaler divisor
#endif

#include <util/delay.h>	// needed for our delay
#include <avr/io.h>

int main (void)
{

	/* Set the Main clock to internal 32kHz oscillator*/
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCULP32K_gc);
	/* Set the Main clock prescaler divisor to 2X and disable the Main clock prescaler */
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm); // Disable prescaler
	/* ensure 20MHz isn't forced on*/
	_PROTECTED_WRITE(CLKCTRL.OSC20MCTRLA, CLKCTRL.OSC20MCTRLA & ~CLKCTRL_RUNSTDBY_bm);

	/* Configure Port A, Pin 6 as an output (remember to connect LED to PB6 and use a resistor in series to GND)*/
	PORTA.DIRSET = PIN6_bm;
	
	/* Set all pins except the LED pin to pullups*/
	PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
	PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
	PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
	PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
	//PORTA.PIN6CTRL = PORT_PULLUPEN_bm; // LED pin
	PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
	
	
	while (1)
	{
		// PAUSE 1000 milliseconds
		_delay_ms(1000);
		
		// LED off 
		PORTA.OUTCLR = PIN6_bm;
		
		// PAUSE 2000 milliseconds
		_delay_ms(1000);
		
		// LED on
		PORTA.OUTSET = PIN6_bm;
	}
}