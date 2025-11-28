// Add preprocessors and define the state machine and variables externally used in the Simon game

#include <stdint.h>

#ifndef GAMESTATE
#define GAMESTATE

// Simon game state machine
typedef enum {
    SIMON_GENERATE,
    SIMON_PLAY_ON,
    SIMON_PLAY_OFF,
    AWAITING_INPUT,
    HANDLE_INPUT,
    EVALUATE_INPUT,

    SUCCESS,
    FAIL,
    DISP_SCORE,
    ENTER_NAME

} Game_State;

// State machine variables used externally for the Simon state machine

extern uint16_t sequence_length;    
extern uint8_t sequence_index;
extern Game_State game_state;

#endif
