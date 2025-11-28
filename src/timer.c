// Add preprocessors

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "timer.h"
#include "display.h"

// Define minimum and maximum playback delay

#define MIN_PLAYBACK_DELAY 250LL
#define MAXIMUM_PLAYBACK_DELAY 1750LL

// Declare Volatile variables as these variables will be updated in ISR for TCB0 and TCB1

volatile uint16_t elapsed_time = 0; 

volatile uint8_t pb_debounced_state = 0xFF; 

uint16_t playback_delay = MIN_PLAYBACK_DELAY;

// Update playback delay using ADC
void update_playback_delay(void)
{  
    // ADC Conversion code
    ADC0.COMMAND = ADC_START_IMMEDIATE_gc;
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    // Receive the ADC0 Result
    uint16_t result = ADC0.RESULT;
    // Activate interrupts for ADC
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    
    // Playback delay has a span of 250ms to 2000 ms
    playback_delay = MIN_PLAYBACK_DELAY + ((result * MAXIMUM_PLAYBACK_DELAY) >> 8);
}

// Push button debouncing vertical counter 
void pb_debounce(void) 
{
    static uint8_t count0 = 0;
    static uint8_t count1 = 0;
    uint8_t pb_sample = PORTA.IN;              // Sample PB state
    uint8_t pb_changed = pb_sample ^ pb_debounced_state; // Detect change to PB
    // Increment if PB state changed, reset otherwise
    count1 = (count1 ^ count0) & pb_changed;
    count0 = ~count0 & pb_changed;
    // If changed state stable for three samples, update PB state
    pb_debounced_state ^= (count1 & count0);
}

// Interrupt for TCB0
ISR(TCB0_INT_vect)
{
    elapsed_time++;   
    TCB0.INTFLAGS = TCB_CAPT_bm;   
}

// Interrupt for TCB1
ISR(TCB1_INT_vect)
{
    pb_debounce();
    // Swap display digit is for multiplexing
    swap_display_digit();
    TCB1.INTFLAGS = TCB_CAPT_bm;
}
