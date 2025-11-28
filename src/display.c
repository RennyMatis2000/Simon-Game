// Add preprocessors

#include <avr/interrupt.h>
#include <avr/io.h>
#include <display.h>

#include "display.h"
#include "display_macros.h"

// Declare left and right display for seven segment display
volatile uint8_t left_byte  = SEGS_OFF;
volatile uint8_t right_byte = SEGS_OFF;

// Number segments
uint8_t number_segs [] = {
    SEGS_ZERO, SEGS_ONE, SEGS_TWO, SEGS_THREE, SEGS_FOUR, SEGS_FIVE, SEGS_SIX, SEGS_SEVEN, 
    SEGS_EIGHT, SEGS_NINE, SEGS_A, SEGS_B, SEGS_C, SEGS_D, SEGS_E, SEGS_F 
};

// Find decimal digits to display users score as decimals 
void find_dec_digit(uint16_t num, uint8_t *hundreds, uint8_t *tens, uint8_t *units) 
{
    *hundreds = 0;    
    *tens = 0;
    *units = num;
    // Convert to hundreds
    while (*units > 99) {
        (*hundreds)++;
        (*units) -= 100;
    }
 
    // Convert to tens
    while (*units > 9) {
        (*tens)++;
        (*units) -= 10;
    }

    // Display leading zeros only if the score exceeds 99 for left display
    if ((*tens == 0) && (*hundreds == 0))
    {
        left_byte = SEGS_OFF;
    }
    else 
    {
        left_byte = number_segs[*tens];
    }

    // Display units as score on right display
    right_byte = number_segs[*units];
}

// Write data to the SPI
void display_write(uint8_t data) 
{
    SPI0.DATA = data;              
}

// Multiplexing code
void swap_display_digit(void)
{
    static int digit = 0;
    if (digit) {
        display_write(left_byte | DISP_LHS);
    } else {
        display_write(right_byte);
    }
    digit = !digit;
}

// Update display to show seven segment display on the left and right
void update_display(const uint8_t left, const uint8_t right)
{
    left_byte = left;
    right_byte = right;
}

// Display the score by using find_dec_digit pointers and update_display for left and right
void display_score(uint8_t score) 
{
    uint8_t h, t, u;
    find_dec_digit(score, &h, &t, &u);
    update_display(left_byte, right_byte);
}

// Rising edge ISR to display outputs to the seven segment display
ISR(SPI0_INT_vect) 
{

  // Rising edge on DISP_LATCH
  PORTA.OUTCLR = PIN1_bm;   
  PORTA.OUTSET = PIN1_bm;
  // Clear interrupt flag        
  SPI0.INTFLAGS = SPI_IF_bm;          
}

