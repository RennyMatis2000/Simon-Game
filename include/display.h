// Add preprocessors and prototypes for display functions used externally

#include <stdint.h>

#ifndef DISPLAY_H
#define DISPLAY_H

//Update display and update score are used to display on the seven segment display the main Simon state machine

void update_display(const uint8_t left, const uint8_t right);
void display_score(uint8_t score);
void swap_display_digit(void);

#endif
