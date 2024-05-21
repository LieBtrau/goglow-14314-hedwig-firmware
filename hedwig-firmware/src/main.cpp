/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief Harry Potter Hedwig Lamp Controller
 * @version 0.1
 * @date 2024-05-20
 *
 * @copyright Copyright (c) 2024
 *
 * Hardware setup:
 * * Pin 3 : NC-switch to GND (PA7) (open when pressed)
 * * Pin 7 : TOP LED (PA3)
 * * Pin 5 : BOTTOM LED (PA2)
 */

#ifndef F_CPU
#define F_CPU 32000UL // 32 kHz clock speed / 0 prescaler divisor
#endif

#include <util/delay.h> // needed for our delay
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

typedef enum led_state_t
{
	LED_OFF,
	BOTTOM_LED_ON,
	TOP_LED_ON
} led_state_t;
static void setup();
static void goto_sleep();
static void do_LED_state(led_state_t state);

static volatile uint32_t counter = 0;
static volatile bool new_edge = false;

#define SWITCH_PIN_MASK PIN7_bm
#define TOP_LED_MASK PIN3_bm
#define BOTTOM_LED_MASK PIN2_bm

/**
 * @brief Pin change detection with debouncing
 *
 * @return true pin has changed state and is now stable
 * @return false pin has not changed state or is still bouncing
 */
bool button_state_changed()
{
	static enum {
		WAITING_FOR_CHANGE,
		PIN_BOUNCING,
	} state = WAITING_FOR_CHANGE;
	static uint32_t edge_start = 0;
	const uint16_t DEBOUNCE_TIME = 3; // 3 * 31.25Hz = 93.75ms

	switch (state)
	{
	case WAITING_FOR_CHANGE:
		if (new_edge)
		{
			new_edge = false;
			state = PIN_BOUNCING;
			edge_start = counter;
			return false;
		}
		break;
	case PIN_BOUNCING:
		if (new_edge)
		{
			// A new edge has been detected, restart the debounce timer
			state = WAITING_FOR_CHANGE;
			return false;
		}
		if (counter - edge_start > DEBOUNCE_TIME)
		{
			// The pin has been stable for the debounce time
			state = WAITING_FOR_CHANGE;
			return true;
		}
		break;
	default:
		state = WAITING_FOR_CHANGE;
		return false;
	}
	return false;
}

int main(void)
{
	setup();
	led_state_t led_state = LED_OFF;
	uint32_t last_button_change = 0;
	uint16_t DOUBLE_PRESS_INTERVAL = 20;
	uint16_t LIGHT_TIMEOUT = 5400; // 5400 * 31.25Hz = 2.5 minutes

	while (true)
	{
		if(counter - last_button_change > LIGHT_TIMEOUT)
		{
			last_button_change = counter;
			led_state = LED_OFF;
			do_LED_state(led_state);
		}
		if (button_state_changed())
		{
			if ((counter - last_button_change > DOUBLE_PRESS_INTERVAL) || (last_button_change == 0))
			{
				last_button_change = counter;
				// Set LED state according to input
				if (PORTA.IN & SWITCH_PIN_MASK)
				{
					led_state = TOP_LED_ON;
				}
				else
				{
					led_state = BOTTOM_LED_ON;
				}
			}
			else
			{
				// Double press detected, turn off the LED
				led_state = LED_OFF;
			}
			do_LED_state(led_state);
		}
	}
}

void setup()
{
	/* Set the Main clock to internal 32kHz oscillator*/
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCULP32K_gc);
	/* Set the Main clock prescaler divisor to 2X and disable the Main clock prescaler */
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm); // Disable prescaler
	/* ensure 20MHz isn't forced on*/
	_PROTECTED_WRITE(CLKCTRL.OSC20MCTRLA, CLKCTRL.OSC20MCTRLA & ~CLKCTRL_RUNSTDBY_bm);

	// Configure periodic interrupt
	RTC.PITCTRLA = RTC_PERIOD_CYC1024_gc; //!<	Periodic Interrupt Timer Period : 1024 cycles : 32kHz/1024 = 31.25Hz
	RTC.PITINTCTRL = RTC_PI_bm;			  //!<	Enable Periodic Interrupt
	RTC.PITCTRLA |= RTC_PITEN_bm;		  //!<	Enable Periodic Interrupt Timer

	// PWM configuration
	TCA0.SINGLE.CTRLB = TCA_SINGLE_ALUPD_bm            		//!< Auto Lock Update: disabled
	                    | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
	TCA0.SINGLE.PER = 0x200; 								//!< Top Value: 0x1000
	TCA0.SINGLE.CMP0 = 0xC0;								//!< Period for PA3 (TOP LED)	
	TCA0.SINGLE.CMP2 = 0xC0;								//!< Period for PA2 (BOTTOM LED)
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc 			//!< System Clock
	                    | 1 << TCA_SINGLE_ENABLE_bp; 		//!< Module Enable: enabled

	// Configure GPIO : PORTA
	PORTA.DIRSET = BOTTOM_LED_MASK | TOP_LED_MASK;
	/* Set all pins except the LED pin to pullups*/
	PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
	PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
	PORTA.PIN6CTRL = PORT_PULLUPEN_bm;

	// Pin 3 is the input pin, connected to the switch
	PORTA.PIN7CTRL = PORT_ISC_BOTHEDGES_gc;

	sei();
}

void do_LED_state(led_state_t state)
{
	switch (state)
	{
	case LED_OFF:
		TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP2EN_bm;
		TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP0EN_bm;
		goto_sleep();
		break;
	case BOTTOM_LED_ON:
		TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP2EN_bm;
		TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP0EN_bm;
		break;
	case TOP_LED_ON:
		TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP2EN_bm;
		TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm;
		break;
	}
}

void goto_sleep()
{
	// Enter sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	cli();
	RTC.PITCTRLA &= ~RTC_PITEN_bm; // Disable the PIT because it keeps running in power down mode and consumes 6µA of current
	sleep_enable();
	sei();
	sleep_cpu();
	sleep_disable();
	RTC.PITCTRLA |= RTC_PITEN_bm; //!<	Enable Periodic Interrupt Timer
	sei();
	_PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRE_bm); // Reset the device to wake up
}

ISR(PORTA_PORT_vect)
{
	uint8_t flags = VPORTA.INTFLAGS; // read the interrupt flags
	VPORTA_INTFLAGS = flags;		 // clear interrupt flags
	// Capture the edge start time
	if (flags & SWITCH_PIN_MASK)
	{
		new_edge = true;
	}
}

ISR(RTC_PIT_vect)
{
	RTC.PITINTFLAGS = RTC_PI_bm;
	counter++;
}
