// Add preprocessors and variables used in highscore.c, along with the HIGHSCORE struct

#include <stdio.h>

#ifndef HIGHSCORE_H
#define HIGHSCORE_H

// Declare maximum amount of top scores and HIGHSCORE array size
#define TOP5SCORES 5
#define HIGHSCOREARRAYMAXIMUM 32
// Maximum name size is 20, and 1 character for null terminator \0
#define NameMaximum 21
// Duration to timeout in enter name (5 seconds)
#define TIMEOUTDURATION 5000

// HIGHSCORE struct holds the name of the player, and the score they achieved
typedef struct {
    char name[NameMaximum];
    uint8_t score;
} HIGHSCORE;

// Functions used externally to print high score and enter a name

void print_highscores(void);
void name_input(char name_string);

// External variables of the HIGHSCORE array, HIGHSCORE array entries amount, and duration to end enter name

extern HIGHSCORE highscores[HIGHSCOREARRAYMAXIMUM];
extern uint8_t highscore_number;
extern uint32_t timeout_end;

#endif