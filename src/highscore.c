// Add preprocessors

#include "game_state.h"
#include "highscore.h"
#include "uart.h"
#include "timer.h"

// A variable that holds the timeout duration, that is continuously extended if a user inputs a character that is not newline '\n'
// This maximum value would be elapsed time + (5000 * 20)
uint32_t timeout_end = 0;

// Amount of entries stored in the HIGHSCORE struct array
uint8_t highscore_number = 0;

// Limit variable caps printing to the maximum size of the HIGHSCORE struct array, or top 5 scores
uint8_t limit = 0;

// Initialise variables used in the UART state machine and set them to default values
HIGHSCORE highscores[HIGHSCOREARRAYMAXIMUM] = {0};

// Bubble sort algorithm to ensure top scores are highest scores
void bubble_sort_highscores(HIGHSCORE *highscores, uint8_t highscore_number) 
{
    // Every scan of all the elements in HIGHSCORE struct array shifts the lowest score to the bottom
    for (uint8_t scan = 0; scan < highscore_number - 1; ++scan) 
    {
        int highscore_swapped = 0;
        // Sort through all elements in the HIGHSCORE struct array
        for (uint8_t i = 0; i < highscore_number - 1 - scan; ++i) {
            // In descending order, sort the higher scores first to the top if the left score is smaller than right
            if (highscores[i].score < highscores[i + 1].score) 
            {
                HIGHSCORE comparison_highscore = highscores[i];
                highscores[i] = highscores[i + 1];
                highscores[i + 1] = comparison_highscore;
                highscore_swapped = 1;
            }
        }
        // Exit early if nothing is swapped
        if (highscore_swapped == 0) 
        {
            break; 
        }
    }
}

// Prints highscores dependent on limit
void print_highscores(void) 
{
    // First bubblesort before printing
    bubble_sort_highscores(highscores, highscore_number);

    // Set limit to maximum size of HIGHSCORE struct array, or 5 top scores
    if (highscore_number < 5) 
    {
        limit = highscore_number;
    } 
    else 
    {
        limit = 5;
    }
    
    // Print name and score for maximum size or 5 top scores
    for (uint8_t i = 0; i < limit; ++i) 
    {
        printf("%s %u\n", highscores[i].name, highscores[i].score);
    }
}

// Handles user input for the enter name functionality
void name_input(char name_string)
{
    // If user is inputting characters and has not reached enter '\n' yet
    if (name_string == '\n')
    {
        // Once user has pressed enter, store the characters in the highscores struct array with a null terminator
        highscores[highscore_number].name[name_length] = '\0';
        // Current sequence length is saved as the score
        highscores[highscore_number].score = sequence_length;
        // Ensure entry does not exceed maximum size of the array
        if (highscore_number < HIGHSCOREARRAYMAXIMUM)
        {
            highscore_number++;
        }
        name_length = 0;
        name_pending = 1;
        serial_state = AWAITING_COMMAND;
    }
    // If name is below the maximum name and array size save the name in the array
    else if (name_length < NameMaximum && highscore_number < HIGHSCOREARRAYMAXIMUM)
    {
        highscores[highscore_number].name[name_length++] = name_string;
    }

    // Increase timeout duration deadline by 5 seconds if user inputs any character
    timeout_end = (elapsed_time + TIMEOUTDURATION); 

}
