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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <blinkie.h>

typedef struct {
   int len;
   int colorRed;
   int colorGreen;
   int colorBlue;
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
    int r,g,b;
    for (int l = 1; l < NUM_LAYER; l++) {
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                getLed(l,x,y,  &r, &g, &b);
                setLed(l-1,x,y, r, g, b);
}   }   }   }
                 

/*******************************************************************************
    PURPOSE: generate pattern

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

#define LOOPS 200

void main( void ) {

    drop_t top[4][4];
    for (int x = 0; x < 4; x++) {
        for (int y=0; y < 4; y++) {
            top[x][y].len = 0;
            setLed((NUM_LAYER-1),x,y, 0, 0, 0);
        }
    }

    for (int loop = 0; loop < LOOPS; loop++) {

        if (loop < (LOOPS - 8)) {
            int drops = rand()%4;
            for (int drop=0; drop < drops; drop++) {
                int x = rand()%4;
                int y = rand()%4;
                top[x][y].len = rand()%4;
                top[x][y].colorRed = rand()%16;
                top[x][y].colorGreen = rand()%16;
                top[x][y].colorBlue = rand()%16;
            }
        }

        shiftDown();

        for (int x = 0; x < 4; x++) {
            for (int y=0; y < 4; y++) {
                if (top[x][y].len == 0) {
                    setLed((NUM_LAYER-1),x,y, 0, 0, 0);
                } else {
                    setLed((NUM_LAYER-1),x,y, top[x][y].colorRed, top[x][y].colorGreen, top[x][y].colorBlue);
                    top[x][y].len--;
                }
            }
        }
        if (endFrame(150)) { return 0; }
    }
}
