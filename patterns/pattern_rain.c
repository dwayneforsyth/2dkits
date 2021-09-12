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

typedef struct {
   uint8_t len;
   uint8_t colorRed;
   uint8_t colorGreen;
   uint8_t colorBlue;
} drop_t;

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
    for (uint8_t l = 1; l < 8; l++) {
        for (uint8_t x = 0; x < 4; x++) {
            for (uint8_t y = 0; y < 4; y++) {
                getLed(l,x,y,&r, &g, &b);
		setLed(l-1,x,y, r, g, b);
	    }
	}
    }
}

/*******************************************************************************
    PURPOSE: generate pattern

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

#define LOOPS 100

void main( void ) {

    FILE *ptr = fopen("rain.pat","wb");
    strncpy(header.name,"Rain",16);
    fwrite( &header, sizeof(header), 1, ptr);
    printHeader();

    patternFrame.cycles = 8;
    patternFrame.delay = 1;

    drop_t top[4][4];
    for (int8_t x = 0; x < 4; x++) {
        for (int8_t y=0; y < 4; y++) {
	    top[x][y].len = 0;
            setLed(7,x,y, 0, 0, 0);
	}
    }

    for (uint16_t loop = 0; loop < LOOPS; loop++) {

	if (loop < (LOOPS - 8)) {
            uint8_t drops = rand()%4;    
            for (uint8_t drop=0; drop < drops; drop++) {
                uint8_t x = rand()%4;
                uint8_t y = rand()%4;
	        top[x][y].len = rand()%4;
	        top[x][y].colorRed = rand()%16;
	        top[x][y].colorGreen = rand()%16;
	        top[x][y].colorBlue = rand()%16;
            }
        }

	shiftDown();

	for (int8_t x = 0; x < 4; x++) {
	    for (int8_t y=0; y < 4; y++) {
                if (top[x][y].len == 0) {
                    setLed(7,x,y, 0, 0, 0);
		} else {
                    setLed(7,x,y, top[x][y].colorRed, top[x][y].colorGreen, top[x][y].colorBlue);
		    top[x][y].len--;
		}
	    }
	}

        writeFrame(ptr);
    }

    fclose(ptr);
}

