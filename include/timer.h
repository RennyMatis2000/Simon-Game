// Add prototypes for variables and functions in timer.c used externally

#include <stdint.h>

#ifndef TIMER_H
#define TIMER_H

// Volatile variables used for push button debouncing

extern volatile uint16_t elapsed_time;

extern volatile uint8_t pb_debounced_state;

// Updates the playback delay throughout the Simon state machine game

void update_playback_delay(void);

extern uint16_t playback_delay;

#endif
