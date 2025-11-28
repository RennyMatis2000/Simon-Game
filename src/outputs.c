// Add preprocessors

#include <stdint.h>

#include "outputs.h"
#include "display_macros.h"
#include "display.h"
#include "buzzer.h"

// Enable the buzzer and display based on the current step value
void enable_outputs(uint8_t step_value)
{
    // Switch case that plays different tones and displays based on the step value
    switch (step_value)
    {
    case 0:
        update_display(DISP_BAR_LEFT, DISP_OFF);
        play_tone(0);
        break;
    case 1:
        update_display(DISP_BAR_RIGHT, DISP_OFF);
        play_tone(1);
        break;
    case 2:
        update_display(DISP_OFF, DISP_BAR_LEFT);
        play_tone(2);
        break;
    case 3:
        update_display(DISP_OFF, DISP_BAR_RIGHT);
        play_tone(3);
        break;
    }
}

// Disable outputs sets the display to nothing and stops the buzzer by setting it to 0%
void disable_outputs(void)
{
    update_display(SEGS_OFF, SEGS_OFF);
    stop_tone();
}
