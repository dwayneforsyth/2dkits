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
#include <stdio.h>

#include "led_driver.h"
#include "pattern_engine.h"
#include "font.h"


uint8_t display_char( uint8_t x, uint8_t charIn, uint8_t color ) { //DDF Debug me!

     uint8_t step;
     uint8_t tL = 0;
     uint8_t tX = 0;
     uint8_t tY = 0;

     printf("display %c\n",charIn);
     charIn -= 0x20;
     step = 0;
     while (step < charData[charIn].sl) {
        uint8_t bits = charData[charIn].ch[step];
        for (uint8_t y=0;y<8;y++) {
	   do2DTo3D( x, y, &tL, &tX, &tY);
	   setLed3(tL,tX,tY, 0x0, 0x0, (bits & (1<<(7-y)))? 0x0f : 0x00);
        }
        step++;
        x++;
     }
     return(x);
}

void displayNumber(uint8_t number) {

    allLedsOff3();

    if (getDemoMode()) {
      display_char( 0, 0x44, 0x03 );
    }
    uint8_t drow = 6;
    if (number > 8) {
        printf("l %d %c\n", number+1,  '0'+((number+1)/10));
        drow = display_char( drow, '0'+((number+1)/10), 0x07 );
    }
    drow++;
    drow = display_char( drow, '0'+((number+1) % 10), 0x07 );
} 

