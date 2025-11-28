// Add preprocessors

#include <avr/io.h>
#include <stdint.h>

#include "buzzer.h"

// Tones calculated using the specified formula for Simon, then set to the largest PER value

#define TONE1_PER 18520
#define TONE2_PER 22004
#define TONE3_PER 13860
#define TONE4_PER 37036

// Base octave is 0
// Base octave can be reduced by 2
#define MIN_OCTAVE -2
// Base octave can be increased by 2
#define MAX_OCTAVE 2

// Declare buzzer variables

static uint8_t selected_tone = 0;
static int8_t octave = 0;
static uint8_t buzzer_playing = 0;

// Plays tone based on current tone
void play_tone(const uint8_t tone)
{
    // Array of base tones
    static const uint16_t base_periods[4] = {TONE1_PER, TONE2_PER, TONE3_PER, TONE4_PER};
    // Per is calculated by bitshifting
    uint16_t per = base_periods[tone] >> (octave + 2); 

    // Set the PER for the buffers
    TCA0.SINGLE.PERBUF = per;
    // 50% duty cycle
    TCA0.SINGLE.CMP0BUF = per >> 1;

    // Assign variables
    selected_tone = tone;
    buzzer_playing = 1;
}  

// Sets buzzer to 0% and stops buzzer playing variable
void stop_tone(void)
{
    TCA0.SINGLE.CMP0BUF = 0;
    buzzer_playing = 0;
}

// Increase octave if the buzzer is already playing
void increase_octave(void)
{
    // Octave must be able to be increased
    if (octave < MAX_OCTAVE) 
    {
        octave++;

        if (buzzer_playing) 
        {
            play_tone(selected_tone);
        }
            
    }

}

// Decrease octave if the buzzer is already playing
void decrease_octave(void)
{
    // Octave must be able to be reduced
    if (octave > MIN_OCTAVE) 
    {
        octave--;

        if(buzzer_playing) 
        {
            play_tone(selected_tone);
        }

    }

}