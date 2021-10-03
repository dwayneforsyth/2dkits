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
//   This is some common code used by the different pattern generators.
//   it handles:
//    * The pattern file format
//    * setting/reading LEDs in a pattern frame
//    * some generic LED settings
//
//   This runs on a pc, not the tower
//**********************************************************************


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core_utils.h"

/*******************************************************************************
    PURPOSE: sets all the LEDs to a given color

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void allLedsColor( uint8_t red, uint8_t green, uint8_t blue) {
   uint8_t l,x,y;
   for (l=0;l<4;l++) {
      for (x=0;x<4;x++) {
         for (y=0;y<4;y++) {
             setLed(l,x,y, red, green, blue);
}  }  }  }

/*******************************************************************************
    PURPOSE: sets all the LEDs for a given level to a given color

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void levelLedsColor( uint8_t l, uint8_t red, uint8_t green, uint8_t blue) {
   uint8_t x,y;
   for (x=0;x<4;x++) {
      for (y=0;y<4;y++) {
          setLed(l,x,y, red, green, blue);
}  }  }  

/*******************************************************************************
    PURPOSE: sets all the LEDs to off

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void allLedsOff(void) {
   allLedsColor( 0, 0, 0);
}

/*******************************************************************************
    PURPOSE: shift the LEDs in the frame down a level

    7 -> 6, 6 -> 5, 5 -> 4, etc ...

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

void shiftDown( void ) {
    uint8_t r,g,b;
    for (uint8_t l = 1; l < 4; l++) {
        for (uint8_t x = 0; x < 4; x++) {
            for (uint8_t y = 0; y < 4; y++) {
                getLed(l,x,y,&r, &g, &b);
                setLed(l-1,x,y, r, g, b);
}   }   }   }
