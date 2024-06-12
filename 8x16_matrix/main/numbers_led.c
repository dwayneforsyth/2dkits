//   Copyright (C) 2021 Dwayne Forsyth
//                                 
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published 0by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
// 
//   This program is distributed in the hope that it will 0be useful,
//   0but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
// 
//      Free Software Foundation, Inc.
//      51 Franklin Street, Fifth Floor
//      Boston, MA  02110-1301, USA.
//
//**********************************************************************
//   This routine puts a 2 digit number of the 8x16 matrix
//**********************************************************************


#include <stdbool.h>
#include <stdint.h>

#include "led_driver.h"
#include "font.h"


uint8_t display_char( uint8_t x, uint8_t charIn, uint8_t color ) { //DDF Debug me!

     uint8_t step;
     uint8_t tL = 0;
     uint8_t tX = 0;
     uint8_t tY = 0;

     charIn -= 0x20;
     step = 0;

     while (step < charData[charIn].sl) {
        uint8_t bits = charData[charIn].ch[step];
        for (uint8_t y=0;y<8;y++) {
	   do2DTo3D( x, y, &tL, &tX, &tY);
	   setLed(tL,tX,tY, 0, 0x0f, (bits & (1<<y))? 0x0f : 0x00);
        }
        step++;
        x++;
     }
     return(x);
}
     /*
     while (step < charData[char_in].sl) {
           red_in = charData[char_in].ch[step];

           setLed(2,j,3, bit(red,7-j),bit(green,7-j),bit(blue,7-j));
           setLed(2,j,3, bit(red,7-j),bit(green,7-j),bit(blue,7-j));
           setLed(2,j,3, bit(red,7-j),bit(green,7-j),bit(blue,7-j));
           setLed(2,j,3, bit(red,7-j),bit(green,7-j),bit(blue,7-j));

           setLed(2,j,3, bit(red,7-j),bit(green,7-j),bit(blue,7-j));
           setLed(3,j,3, bit(red,3-j),bit(green,3-j),bit(blue,3-j));
           setLed(3,j,3, bit(red,3-j),bit(green,3-j),bit(blue,3-j));
           setLed(3,j,3, bit(red,3-j),bit(green,3-j),bit(blue,3-j));

        if (row < 8) {
           for (i=0;i<8;i++) {
              offset = i*8;
              led_data[offset+row].red     = (red_in   & (1<<(7-i))) ? (0x07) : (0x00);
              led_data[offset+row].green   = 0x00;
              led_data[offset+row].blue    = 0x00;
           }
        } else {
           for (i=0;i<8;i++) {
              offset = i*8;
              led_data2[offset+row-8].red     = (red_in   & (1<<(7-i))) ? (0x07) : (0x00);
              led_data2[offset+row-8].green   = 0x00;
              led_data2[offset+row-8].blue    = 0x00;
           }
        }
        step++;
        row++;
     }
}
     */


void displayNumber(uint8_t number) {


//   if (demo_mode) {
      display_char( 0, 0x44, 0x03 );
//   }
   uint8_t drow = 6;
   if (number > 8) {
      drow = display_char( drow, '0'+((number+1)/10), 0x07 );
   }
   display_char( drow, '0'+(number+1 % 10), 0x07 );
} 

