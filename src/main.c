// Add preprocessors, primarily including external functions and variables from other files into main.c to enable the Simon state machine

#include <avr/interrupt.h>

#include "display.h"
#include "display_macros.h"
#include "game_state.h"
#include "outputs.h"
#include "lfsr.h"
#include "timer.h"
#include "initialisation.h"
#include "display.h"
#include "timer.h"
#include "uart.h"
#include "highscore.h"

void state_machine(void);

int main(void)
{
    // Enable interrupts, initialise all peripherals and disable interrupts

    cli();
    adc_init();
    pb_init();
    spi_init();
    pwm_init();
    timer_init();
    uart_init();
    sei();

    state_machine();

    // The program should not reach this point
    while (1)
        ;
}

// Declare global variables used in the Simon game
Game_State game_state = SIMON_GENERATE;
uint8_t pb_pressed = 4;
uint8_t pb_released = 0;
uint16_t sequence_length = 1;
uint8_t sequence_index = 0;
uint8_t uart_pressed = 0;

void state_machine(void)
{

     // Disable outputs to ensure off state before state machine is enabled

    disable_outputs();

    sequence_length = 1;

    // Initialise LFSR by assigning first STATE_LFSR to initial seed
    
    init_LFSR();

    sequence_index = 0;

    // Get the first step which bit ands 0b11 with the current STATE_LFSR

    uint8_t step = get_step();

    // Pushbutton state
    uint8_t pb_previous_state, pb_current_state = 0xFF;

    // Pushbutton edge flags
    uint8_t pb_falling_edge, pb_rising_edge = 0x00;

    while (1)
    {
        // Push button debouncing code
        uint8_t pb_changed;

        pb_previous_state = pb_current_state;
        pb_current_state = pb_debounced_state;

        pb_changed = pb_previous_state ^ pb_current_state;
        pb_falling_edge = pb_changed & ~pb_current_state;
        pb_rising_edge = pb_changed & pb_current_state;

        // Simon state machine that uses state variable game_state
        switch (game_state)
        {
        case SIMON_GENERATE:

            // Save start sequence if game has just started at sequence_index 0
            if (sequence_index == 0)
            {
                // Seed_pending is true when seed UART gameplay has successfully created a new seed
                if (seed_pending)
                {
                    // Activate newly created stored seed at the start of the game, and initialise
                    seed = stored_seed;
                    init_LFSR();
                    seed_pending = 0;
                }
                // Retrieve the starting STATE_LFSR to be used for reset_LFSR();
                get_start_sequence();
            }

            // If Simon has not generated and played enough tones yet
            if (sequence_index < sequence_length)
            {
                // Generate next LFSR_STATE with the algorithm, retrieve step, increment sequence and enable outputs
                next_LFSR();
                step = get_step();
                sequence_index++;
                enable_outputs(step);
                // Update playback delay at a safe time (after enabling outputs)
                update_playback_delay();
                elapsed_time = 0;
                game_state = SIMON_PLAY_ON;
            }

            // If Simon has generated and played enough tones
            else if (sequence_index == sequence_length)
            {
                // Save end sequence before moving forward for advance_LFSR();
                get_end_sequence();
                // Reset sequence to the beginning for the user to start playing from the start
                reset_LFSR();
                sequence_index = 0;
                game_state = AWAITING_INPUT;
            }

            break;

        case SIMON_PLAY_ON:

            // Simon must be enabled for half of the playback delay duration
            if (elapsed_time >= (playback_delay >> 1))
            {
                // Disable Simon after half of the playback delay duration
                disable_outputs();
                game_state = SIMON_PLAY_OFF;
            }

            break;

        case SIMON_PLAY_OFF:

            // Simon must be turned off for the full playback delay duration
            if (elapsed_time >= playback_delay)
            {
                game_state = SIMON_GENERATE;
            }

            break;

        case AWAITING_INPUT:

            // Handle UART Input for a button press
            if (uart_input <= 3)
            {
                // Assign the uart button press to push button pressed
                pb_pressed = uart_input;
                // Enable outputs based on what button was pressed
                enable_outputs(pb_pressed);
                uart_input = 4;
                elapsed_time = 0;
                // Assign variable that states a UART button has been pressed, so it must be released
                uart_pressed = 1;
                game_state = HANDLE_INPUT;
                break;
            }

            pb_pressed = 4;

            // Check for falling edge on any push button and assign variable to determine which push button pressed
            if (pb_falling_edge & PIN4_bm)
                pb_pressed = 0;
            else if (pb_falling_edge & PIN5_bm)
                pb_pressed = 1;
            else if (pb_falling_edge & PIN6_bm)
                pb_pressed = 2;
            else if (pb_falling_edge & PIN7_bm)
                pb_pressed = 3;

            // If any push button pressed, continue
            if (pb_pressed != 4)
            {
                // Enable outputs based on which push button was pressed
                enable_outputs(pb_pressed);
                elapsed_time = 0;
                game_state = HANDLE_INPUT;
            }

            break;

        case HANDLE_INPUT:

            // Handle UART release, when it is acknowledged that a UART button has been pressed
            if (uart_pressed)
            {
                // Simon tone and play must be enabled for half of playback delay
                if (elapsed_time >= (playback_delay >> 1))
                {
                    // Disable outputs, move forward in the LFSR and increase the sequence index for next user press
                    disable_outputs();
                    next_LFSR();
                    step = get_step();
                    sequence_index++;
                    uart_pressed = 0;
                    game_state = EVALUATE_INPUT;
                    break;
                }
            }

            // If any push button is released trigger flag (game assumes any push button release is fine)
            if (pb_rising_edge & (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm))
                pb_released = 1;

            // If push button is released and half of playback delay to guess
            if (pb_released && (elapsed_time >= (playback_delay >> 1)))
            {
                // Disable outputs, move forward in the LFSR and increase the sequence index for next user press
                disable_outputs();
                next_LFSR();
                step = get_step();
                sequence_index++;
                // Clear pb released
                pb_released = 0;
                game_state = EVALUATE_INPUT;
            }

            break;

        case EVALUATE_INPUT:

            // Incorrect button was pressed
            if (step != pb_pressed)
            {
                // Display fail pattern (two horizontal bars in the middle)
                update_display(SEGS_BAR, SEGS_BAR);
                // Display fail message and score
                printf("GAME OVER\n%u\n", sequence_length);
                elapsed_time = 0;
                game_state = FAIL;
            }

            // If correct button is pressed and sequence length is longer than sequence index, then user must continue to play by pressing buttons
            else if (step == pb_pressed && sequence_index < sequence_length)
            {
                game_state = AWAITING_INPUT;
            }

            // If correct button is pressed and sequence equals sequence length, user has guessed all Simon prompts and succeeds
            else if (step == pb_pressed && sequence_index == sequence_length)
            {
                // Display success pattern (all segments lit up)
                update_display(SEGS_ALL, SEGS_ALL);
                // Update playback delay at a safe time (after enabling displays)
                update_playback_delay();
                //Display success message and score
                printf("SUCCESS\n%u\n", sequence_length);
                elapsed_time = 0;
                reset_LFSR();
                sequence_index = 0;
                game_state = SUCCESS;
            }

            break;

        case SUCCESS:

            // Ensure success pattern is displayed for playback delay duration
            if (elapsed_time >= playback_delay)
            {
                // Increase the sequence length (representing maximum cap) for next playthrough
                sequence_length++;
                // Turn display off
                update_display(SEGS_OFF, SEGS_OFF);
                // Update playback delay at a safe time (after enabling displays)
                update_playback_delay();
                game_state = SIMON_GENERATE;
            }

            break;

        case FAIL:

            // Ensure fail pattern is dsisplayed for playback delay duration
            if (elapsed_time >= playback_delay)
            {
                // Display final score of user
                display_score(sequence_length);
                elapsed_time = 0;
                game_state = DISP_SCORE;
            }

            break;

        case DISP_SCORE:

            // Ensure display score pattern is displayed for playback delay duration
            if (elapsed_time >= playback_delay)
            {
                // Turn display off
                update_display(SEGS_OFF, SEGS_OFF);
                elapsed_time = 0;
                // Display prompt to enter name
                printf("Enter name: \n");
                // Enable variable for user to enter their name using UART
                uart_name = 1;
                // Snapshot the current elapsed time into timeout_end variable to create a 5 second timeout duration
                timeout_end = (elapsed_time + TIMEOUTDURATION);
                game_state = ENTER_NAME;
            }

            break;

        case ENTER_NAME:
        {
            // Name_pending variable ensures user has successfully entered a name correctly in the UART ISR state machine, and pressed the enter key '\n'
            if (name_pending)
            {
                name_pending = 0;
                uart_name = 0;
                // Print highscore array, which is a struct with char name and uint8_t score
                print_highscores();
                // Advance the LFSR as user has failed the Simon game, and reset variables for new game
                advance_LFSR();
                sequence_index = 0;
                sequence_length = 1;
                game_state = SIMON_GENERATE;
                break;
            }

            // User did not submit an input, or did not press the enter key '\n' for the 5 second timeout duration
            if (elapsed_time >= timeout_end)
            {
                // Save whatever the user has input already as their name for the highscore struct array
                highscores[highscore_number].name[name_length] = '\0';
                highscores[highscore_number].score = sequence_length;
                // Ensure array is not full before adding
                if (highscore_number < HIGHSCOREARRAYMAXIMUM) 
                {
                    highscore_number++;
                }
                // Reset character amount input by user to 0
                name_length = 0;
                print_highscores();
                // Advance the LFSR as user has failed the Simon game, and reset variables for new game
                advance_LFSR();
                sequence_index = 0;
                sequence_length = 1;
                // Exit the UART state for entering a name due to timeout, so UART commands can be used again
                uart_name = 0;
                serial_state = AWAITING_COMMAND;
                game_state = SIMON_GENERATE;
            }
            break;
        }
        default:
        // Default state case incase state machine malfunctions to restart the game to default state
            game_state = SIMON_GENERATE;
            disable_outputs();
            init_LFSR();
            break;
        }
    }
}
