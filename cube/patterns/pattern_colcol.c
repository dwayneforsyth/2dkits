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
//   This is a pattern generator. it creates a pattern for the Tower.
//   This runs on a pc, not the tower
//**********************************************************************

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "core_utils.h"
#include "core_helper.h"


/*******************************************************************************
    PURPOSE: generate pattern

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

void circle( uint8_t c, uint8_t * x, uint8_t * y) {
	const uint8_t offset[12] = {0, 1, 2, 3, 3, 3, 3, 2, 1, 0, 0, 0};

	*y = offset[c];
	*x = offset[(c+9)%12];
}

void layer( uint8_t l,  uint8_t loop ) {
	uint8_t x, y;
        circle( (loop +  0)%12, &x, &y);
        setLed( l, x, y, LED_PLUS,  LED_MINUS, LED_PLUS); //Rose
        circle( (loop +  1)%12, &x, &y);
        setLed( l, x, y, LED_PLUS,  LED_MINUS, LED_PLUS); //Magenta
        circle( (loop +  2)%12, &x, &y);
        setLed( l, x, y, LED_MINUS, LED_MINUS, LED_PLUS); //Violet
        circle( (loop +  3)%12, &x, &y);
        setLed( l, x, y, LED_MINUS, LED_MINUS, LED_PLUS); //Blue
        circle( (loop +  4)%12, &x, &y);
        setLed( l, x, y, LED_MINUS, LED_PLUS,  LED_PLUS); //Azure
        circle( (loop +  5)%12, &x, &y);
        setLed( l, x, y, LED_MINUS, LED_PLUS,  LED_PLUS); //cyan
        circle( (loop +  6)%12, &x, &y);
        setLed( l, x, y, LED_MINUS, LED_PLUS,  LED_MINUS); //Aqua
        circle( (loop +  7)%12, &x, &y);
        setLed( l, x, y, LED_MINUS, LED_PLUS,  LED_MINUS); //Green
        circle( (loop +  8)%12, &x, &y);
        setLed( l, x, y, LED_PLUS,  LED_PLUS,  LED_MINUS); //Chart
        circle( (loop +  9)%12, &x, &y);
        setLed( l, x, y, LED_PLUS,  LED_PLUS,  LED_MINUS); //Yellow
        circle( (loop + 10)%12, &x, &y);
        setLed( l, x, y, LED_PLUS,  LED_MINUS, LED_MINUS); //Orange
        circle( (loop + 11)%12, &x, &y);
        setLed( l, x, y, LED_PLUS,  LED_MINUS, LED_MINUS); //Red
}

void colcol1( void ) {

    createPattern("colcol1.pat","Color Col1");

    patternFrame.cycles = 1;
    patternFrame.delay = 1;

    patternFrame.cycles = 16;
    patternFrame.delay = 1;

    for (uint16_t loop = 0; loop < 12; loop++) {
        layer( 0, loop );
        layer( 1, loop );
        layer( 2, loop );
        layer( 3, loop );
        layer( 4, loop );
        layer( 5, loop );
        layer( 6, loop );
        layer( 7, loop );
        writeFrame();
    }
    finishPattern();
}

void colcol2( void ) {

    createPattern("colcol2.pat","Color Col2");

    patternFrame.cycles = 1;
    patternFrame.delay = 1;

    patternFrame.cycles = 16;
    patternFrame.delay = 1;

    for (uint16_t loop = 0; loop < 12; loop++) {
        layer( 0, loop );
        layer( 1, loop+1 );
        layer( 2, loop+2 );
        layer( 3, loop+3 );
        layer( 4, loop+4 );
        layer( 5, loop+5 );
        layer( 6, loop+6 );
        layer( 7, loop+7 );
        writeFrame();
    }
    finishPattern();
}

void main( void ) {
    colcol1();
    colcol2();
}

