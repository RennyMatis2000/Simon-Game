// Add preprocessors

#include <stdio.h>
#include <avr/io.h>

#include "initialisation.h"
#include "uart.h"

// ADC initialisation 
void adc_init(void)
{
    ADC0.CTRLA = ADC_ENABLE_bm;       
    // Prescaler of 2                       
    ADC0.CTRLB = ADC_PRESC_DIV2_gc;                          
    ADC0.CTRLC = (4 << ADC_TIMEBASE_gp) | ADC_REFSEL_VDD_gc; 
    ADC0.CTRLE = 64;                                        
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;                         
}

// Push button initialisation 
void pb_init(void)
{
    // Initialise all push buttons 1, 2, 3 and 4
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
}

// SPI initialisation
void spi_init(void) 
{
  PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc; // SPI pins on PC0-3

  PORTC.DIRSET = (PIN0_bm | PIN2_bm); // Set SCK (PC0) and MOSI (PC2) as outputs

  // Set DISP_Latch for initial high and output pin
  PORTA.OUTSET = PIN1_bm; 
  PORTA.DIRSET = PIN1_bm; 

  SPI0.CTRLA = SPI_MASTER_bm; 
  SPI0.CTRLB = SPI_SSD_bm; 
  SPI0.INTCTRL = SPI_IE_bm; // Enable interrupts for spi
  SPI0.CTRLA |= SPI_ENABLE_bm; 
}

// PWM Initialisation
void pwm_init()
{
    // PB0 → buzzer output (WO0), buzzer off initially and set PB0 as output
    PORTB.OUTCLR = PIN0_bm; 
    PORTB.DIRSET = PIN0_bm; 

    // Configure TCA0 for single-slope PWM, enable WO0 (PB0)
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm;

    TCA0.SINGLE.PER = 1;
    // Start with buzzer off (no duty)
    TCA0.SINGLE.CMP0 = 0;

    // Use system clock, prescaler/2  → 1.667 MHz 
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV2_gc | TCA_SINGLE_ENABLE_bm;

}

// Timer initialisation for TCB0 and TCB1
void timer_init(void)
{
    // Configure TCB0 in periodic interrupt mode, to be used for 1ms interrupts for elapsed time
    TCB0.CTRLB = TCB_CNTMODE_INT_gc; 
    // Set CCMP to 1667 as prescaler of 2 (3333 clocks @ 3.333 MHz divide 2)
    TCB0.CCMP = 1667;          
    // CAPT interrupt enable      
    TCB0.INTCTRL = TCB_CAPT_bm;      
    TCB0.CTRLA = TCB_CLKSEL_DIV2_gc | TCB_ENABLE_bm;      
    
    // Configure TCB1 to be used for 5ms interrupts for push button debouncing and multiplexing display
    TCB1.CTRLB = TCB_CNTMODE_INT_gc;
    // Set CCMP to 8333 as prescaler of 2 (1667 multiplied by 5)
    TCB1.CCMP  = 8333;
    // CAPT interrupt enable                             
    TCB1.INTCTRL = TCB_CAPT_bm;
    TCB1.CTRLA  = TCB_CLKSEL_DIV2_gc | TCB_ENABLE_bm;
}



