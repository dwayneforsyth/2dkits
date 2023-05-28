#ifndef TFT_STUFF_H
#define TFT_STUFF_H
// the contents of this fie are included before any picoc program is run
// add items here that you deem to be helpfull in your work
//
// be carefull in adding content since any errors in this file will prevent picoc from running
//
// these are 16 bit color values for rrrrrggggggrrrrr encoding
#define b16_BLACK       0x0000      /*   0,   0,   0 */
#define b16_NAVY        0x000F      /*   0,   0, 128 */
#define b16_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define b16_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define b16_MAROON      0x7800      /* 128,   0,   0 */
#define b16_PURPLE      0x780F      /* 128,   0, 128 */
#define b16_OLIVE       0x7BE0      /* 128, 128,   0 */
#define b16_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define b16_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define b16_BLUE        0x001F      /*   0,   0, 255 */
#define b16_GREEN       0x07E0      /*   0, 255,   0 */
#define b16_CYAN        0x07FF      /*   0, 255, 255 */
#define b16_RED         0xF800      /* 255,   0,   0 */
#define b16_MAGENTA     0xF81F      /* 255,   0, 255 */
#define b16_YELLOW      0xFFE0      /* 255, 255,   0 */
#define b16_WHITE       0xFFFF      /* 255, 255, 255 */
#define b16_ORANGE      0xFDA0      /* 255, 180,   0 */
#define b16_GREENYELLOW 0xB7E0      /* 176, 255,   0 */
#define b16_PINK        0xFC9F      /* 255, 160, 255 */

#endif