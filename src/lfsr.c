// Add preprocessors

#include <stdint.h>

#include "lfsr.h"

// Declare LFSR variables

uint32_t STATE_LFSR = 0;

uint32_t seed = 0x12056081;

const uint32_t MASK = 0xE2025CAB;

uint32_t start_sequence = 0;

uint32_t end_sequence = 0;

// Volatile seed variable is the variable used for the UART seed gameplay functionality

volatile uint32_t stored_seed = 0;

// LFSR sorting algorithm
void next_LFSR(void)
{
    // Bit shifted out
    uint32_t STATE_LFSR_bit = (STATE_LFSR & 1); 
    // Shift right
    STATE_LFSR = STATE_LFSR >> 1;        
    // If bit was 1, XOR using mask             
    if (STATE_LFSR_bit == 1)              
    {
        STATE_LFSR ^= MASK;
    }
}

// Get current step by bitwise and on STATE_LFSR and returning it as a uint8_t
uint8_t get_step(void) 
{
    uint8_t step = (STATE_LFSR & 0b11);
    return step;
}

// Initialise the LFSR by setting the STATE_LFSR to the seed;
void init_LFSR(void) 
{
    STATE_LFSR = seed;
}

// Retrieve start sequence for resetting the sequence
void get_start_sequence(void) 
{
    start_sequence = STATE_LFSR;
}

// Retrieve end sequence for advancing the sequence
void get_end_sequence(void) 
{
    end_sequence = STATE_LFSR;
}

// Reset the game to start of LFSR step
void reset_LFSR(void) 
{
    STATE_LFSR = start_sequence;
}

// Advance the game to next LFSR step
void advance_LFSR(void) 
{
    STATE_LFSR = end_sequence;
}