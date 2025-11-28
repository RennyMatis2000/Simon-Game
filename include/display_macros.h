// Preprocessors to be used for displaying on the seven segment display

#ifndef DISPLAY_MACROS_H
#define DISPLAY_MACROS_H

// Number segments from 0-15 (0-9, then a-f is 10 to 15)

#define SEGS_ZERO   0x08    // 0: 0000001  00001000
#define SEGS_ONE    0x6B    // 1: 1001111  01101011
#define SEGS_TWO    0x44    // 2: 0010010  01000100
#define SEGS_THREE  0x41    // 3: 0000110  01000001
#define SEGS_FOUR   0x23    // 4: 1001100  00100011
#define SEGS_FIVE   0x11    // 5: 0100100  00010001
#define SEGS_SIX    0x10    // 6: 0100000  00010000
#define SEGS_SEVEN  0x4B    // 7: 0001111  01001011
#define SEGS_EIGHT  0x00    // 8: 0000000  00000000
#define SEGS_NINE   0x01    // 9: 0000100  00000001
#define SEGS_A      0x02    // A: 0001000  00000010
#define SEGS_B      0x30    // B: 1100000  00110000
#define SEGS_C      0x1C    // C: 0110001  00011100
#define SEGS_D      0x60    // D: 1000010  01100000
#define SEGS_E      0x14    // E: 0110000  00010100
#define SEGS_F      0x16    // F: 0111000  00010110

// Segments for success, fail, and off

#define SEGS_ALL    0x00   //     0000000  00000000
#define SEGS_BAR    0x77   //     1111110  01110111
#define SEGS_OFF    0x7F   //     1111111  01111111

// Display segments for xFABGCDE seven segment display

#define DISP_SEG_F 0b00111111
#define DISP_SEG_A 0b01011111
#define DISP_SEG_B 0b01101111
#define DISP_SEG_G 0b01110111
#define DISP_SEG_C 0b01111011
#define DISP_SEG_D 0b01111101
#define DISP_SEG_E 0b01111110

// Display on, off, left, right, and display on the left hand side

#define DISP_ON  0b00000000
#define DISP_OFF 0b01111111

#define DISP_BAR_LEFT (DISP_SEG_E & DISP_SEG_F)
#define DISP_BAR_RIGHT (DISP_SEG_B & DISP_SEG_C)

#define DISP_LHS (1 << 7) // Bitshift to show on left display

#endif
