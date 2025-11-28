// Add prototypes for functions and external variables in uart.c 
#include <stdio.h>

#include "highscore.h"

#ifndef UART_H
#define UART_H

// UART state machine
typedef enum
{
    AWAITING_COMMAND,
    AWAITING_NAME,
    AWAITING_SEED
} Serial_State;

// UART initialisation function

void uart_init(void);

// External volatile variables that are updated in the ISR UART state machine

extern volatile uint8_t uart_input;

extern volatile uint8_t seed_pending;

extern volatile uint8_t name_pending;

extern volatile uint8_t uart_name;

extern volatile uint8_t name_length;

extern volatile Serial_State serial_state;

#endif
