// Add preprocessors

#include <stdio.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "buzzer.h"
#include "game_state.h"
#include "timer.h"
#include "lfsr.h"
#include "buzzer.h"
#include "outputs.h"
#include "highscore.h"

// Enable standard I/O calls to stdout 
int uart_putc(char c, FILE *stream)
{

    while (!(USART0.STATUS & USART_DREIF_bm))
        ; // wait for data register empty
    USART0.TXDATAL = c;
    return c;
}

// Enable standard I/O calls to stdout 
static FILE stdio = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);

// Initialise UART in uart.c for stdio
void uart_init(void)
{

    PORTB.DIRSET = PIN2_bm; // enable PB2 as an output (UART TX)

    USART0.BAUD = 1389;                           // 9600 BAUD @ 3.33MHz
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm; // enable TX/RX

    USART0.CTRLA = USART_RXCIE_bm; // Enable receive complete interrupt

    // Setup the standard output stream
    stdout = &stdio;
}

// Resets the game by disabling all outputs and setting variables back to default, moving the state machine back to the beginning
void reset_game(void)
{
    disable_outputs();
    init_LFSR();
    sequence_index = 0;
    sequence_length = 1;
    elapsed_time = 0;
    game_state = SIMON_GENERATE;
}

// Converts a hexchar to an integer
static uint8_t hexchar_to_int(char c)
{
    if ('0' <= c && c <= '9') // 0 to 9
        return c - '0';
    else if ('a' <= c && c <= 'f') // 10 to 15
        return 10 + c - 'a';
    else
        return 16; // Invalid if 16
}

// Declare volatile variables used in the UART state machine
volatile uint8_t uart_input = 4;
volatile uint8_t seed_pending = 0;
volatile uint8_t name_pending = 0;
volatile uint8_t name_length = 0;
volatile uint8_t uart_name = 0;
// Seed payload parsing variables
static uint8_t seed_chars_received = 0;
static uint8_t seed_payload_valid = 1;
static uint32_t seed_payload = 0;

// Initialise UART state machine variable
volatile Serial_State serial_state = AWAITING_COMMAND;

// Interrupt for UART
ISR(USART0_RXC_vect)
{

    // Read received data and clear interrupt flag
    char rx_data = USART0.RXDATAL;

    switch (serial_state)
    {
    case AWAITING_COMMAND:

        // If user is prompted to enter a name
        if (uart_name)
        {
            // Reset variables used for entering a name
            uart_name = 0;
            name_length = 0;
            name_pending = 0;
            uart_name = 0;
            serial_state = AWAITING_NAME;
            // First byte will be acknowledged as a name input, and not a command, to be finished in AWAITING_NAME state
            name_input(rx_data);
            return;
        }

        // Reset the game if UART buttons for reset are pressed
        if (rx_data == '0' || rx_data == 'p')
        {
            reset_game();
        }
        // Handle new seed command from UART buttons
        else if (rx_data == '9' || rx_data == 'o')
        {
            // Reset seed payload flags
            seed_payload_valid = 1;
            seed_chars_received = 0;
            seed_payload = 0;
            seed_pending = 0;

            // Transition to state that allows entry of new seed
            serial_state = AWAITING_SEED;
        }

        // Increase the octave with UART buttons
        else if (rx_data == ',' || rx_data == 'k')
        {
            increase_octave();
        }
        // Decrease the octave with UART buttons
        else if (rx_data == '.' || rx_data == 'l')
        {
            decrease_octave();
        }

        // Handle user UART gameplay input, mapping each UART button press to a variable
        if (rx_data == '1' || rx_data == 'q')
            uart_input = 0;
        else if (rx_data == '2' || rx_data == 'w')
            uart_input = 1;
        else if (rx_data == '3' || rx_data == 'e')
            uart_input = 2;
        else if (rx_data == '4' || rx_data == 'r')
            uart_input = 3;

        break;

    // After the first byte, the rest of the name is input in this state
    case AWAITING_NAME:
    {
        // Name input for bytes two and onwards
        name_input(rx_data);
        break;
    }

    // State to handle seed inputs
    case AWAITING_SEED:
    {
        // User input is parsed to convert hexchar to integer
        uint8_t parsed_result = hexchar_to_int(rx_data);

        // If parsed result is not 16 continue
        if (parsed_result != 16)
        {
            // Valid inputs to parse into a seed
            seed_payload = (seed_payload << 4) | parsed_result;
        }
        // Seed payload has received an invalid input and must conclude
        else 
        {
            seed_payload_valid = 0;
        }

        // If seed payload has received 8 characters
        if (++seed_chars_received == 8)
        {
            // These 8 characters are valid
            if (seed_payload_valid)
            {
                // New seed has been created, store seed into an external variable
                stored_seed = seed_payload;
                seed_pending = 1;
            }

            // Reset seed payload variables
            seed_chars_received = 0;
            seed_payload_valid = 1;
            seed_payload = 0;
            serial_state = AWAITING_COMMAND;
        }

        break;
    }

    // Default state for handling UART if errors arise
    default:
        serial_state = AWAITING_COMMAND;
        break;
    }
}
