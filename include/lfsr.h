// Add prototypes for functions in lfsr.c and variables used externally

#include <stdint.h>

// LFSR functions for the Simon state machine

void init_LFSR(void);

// Returns the step after step calculation

uint8_t get_step(void);

void reset_LFSR(void);

void next_LFSR(void);

void advance_LFSR(void);

void get_start_sequence(void);

void get_end_sequence(void);

// External seed variables that are updated in the Simon state machine and the seed change UART function

extern uint32_t seed;

extern volatile uint32_t stored_seed;
