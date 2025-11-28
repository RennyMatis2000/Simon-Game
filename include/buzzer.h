// Add preprocessors and prototypes for buzzer.c functions used externally

#include <stdint.h>

#ifndef BUZZER_H
#define BUZZER_H

// These functions are used to adjust the tones and octaves used in the main Simon state machine

void play_tone(const uint8_t tone);
void stop_tone(void);
void increase_octave(void);
void decrease_octave(void);

#endif
