// Add prototypes for output functions

#include <stdint.h>

// Enable and disable outputs are used externally in the Simon state machine

void enable_outputs(uint8_t step_value);
void disable_outputs(void);
