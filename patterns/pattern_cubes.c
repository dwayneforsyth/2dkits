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

typedef struct cube_t {
   uint8_t x;
   uint8_t y;
   uint8_t l;
   int8_t dir;
   int8_t side;
   uint8_t delay;
   uint8_t colorRed;
   uint8_t colorGreen;
   uint8_t colorBlue;
} cube_t;

#define NUM_CUBES 5
cube_t cube[NUM_CUBES] = {
        [0]{.x = 0,
            .y = 0,
            .l = 0,
	    .dir = 1,
            .colorRed = 15,
            .colorGreen = 0,
            .colorBlue = 0,
           },
        [1]{.x = 0,
            .y = 2,
            .l = 6,
	    .dir = -1,
            .colorRed = 0,
            .colorGreen = 15,
            .colorBlue = 0,
           },
        [2]{.x = 2,
            .y = 0,
            .l = 0,
	    .dir = 1,
            .colorRed = 0,
            .colorGreen = 0,
            .colorBlue = 15,
           },
        [3]{.x = 2,
            .y = 2,
            .l = 4,
	    .dir = -1,
            .colorRed = 9,
            .colorGreen = 9,
            .colorBlue = 0,
           },
        [4]{.x = 2,
            .y = 2,
            .l = 0,
	    .dir = -1,
            .colorRed = 9,
            .colorGreen = 0,
            .colorBlue = 9,
           }};

/*******************************************************************************
    PURPOSE: draws the cubs in the towers LED frame

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

void render_cubes(void) {
   uint8_t c,x,y,l;
   allLedsColor(0, 0, 0);
   for (c=0;c<NUM_CUBES;c++) {
       for (x=cube[c].x;x<cube[c].x+2;x++) {
           for (y=cube[c].y;y<cube[c].y+2;y++) {
               for (l=cube[c].l;l<cube[c].l+2;l++) {
//                   printf("(%d,%d,%d) = (%d,%d,%d)\n",l,x,y,cube[c].colorRed,cube[c].colorGreen,cube[c].colorBlue);
                   setLed(l,x,y, cube[c].colorRed,cube[c].colorGreen,cube[c].colorBlue);
}   }   }   }   }


/*******************************************************************************
    PURPOSE: handles the moving of the cube

    INPUTS:

    RETURN CODE:
        0 - moved the cube
	1 - did not move, cube would have been off tower
	2 - did not have, cube would have overlapped another cube

    NOTES:

*******************************************************************************/
uint8_t moveCube(uint8_t id, int8_t dl, int8_t dx, int8_t dy) {
    int8_t l, x, y;

    // out of range check
    l = cube[id].l + dl;
    if ((l < 0)||(l > 6)) { return 1; }

    x = cube[id].x + dx;
    if ((x < 0)||(x > 2)) { return 1; }

    y = cube[id].y + dy;
    if ((y < 0)||(y > 2)) { return 1; }

    // hitting another cube
    for (uint8_t c=0;c<NUM_CUBES;c++) {
	if (( id != c) &&
       	    ((cube[c].l+1 == l)||(cube[c].l == l+1)||(cube[c].l == l)) &&
	    ((cube[c].x+1 == x)||(cube[c].x == x+1)||(cube[c].x == x)) &&
	    ((cube[c].y+1 == y)||(cube[c].y == y+1)||(cube[c].y == y))) {
            return 2;
	}
    }

    // do the move
    cube[id].l = l;
    cube[id].x = x;
    cube[id].y = y;
    return(0);
}

/*******************************************************************************
    PURPOSE: generated the pattern file

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void main(void) {

    FILE *ptr = fopen("cubes.pat","wb");
    strncpy(header.name,"Cubes",16);
    fwrite( &header, sizeof(header), 1, ptr);
    printHeader();

    patternFrame.cycles = 8;
    patternFrame.delay = 1;

    render_cubes();
    writeFrame(ptr);

    for (uint16_t loop; loop < 240; loop++) {
        for (uint8_t id=0;id<NUM_CUBES; id++) {
	    if (cube[id].delay != 0) {
		// finish the wait
	        cube[id].delay--;
	    } else if (cube[id].side != 0) {
		// finish the side to side move
		switch (cube[id].side) {
			case 1: moveCube(id, 0,1,0); break;
			case 2: moveCube(id, 0,0,1); break;
			case 3: moveCube(id, 0,0,-1); break;
			case 4: moveCube(id, 0,-1,0); break;
		}
		cube[id].side = 0;
	    } else {
		// try up/down
		uint8_t t = moveCube(id, cube[id].dir,0,0);
		if (t != 0) {
		    // blocked, random side to side
		    uint8_t r = (t==1)? rand()%7 : rand()%3;
		    if ((r == 0) && (moveCube(id, 0,1,0) == 0)) {
	                cube[id].side = 1;
       		    } else if ((r == 0) && (moveCube(id, 0,0,1) == 0)) {
	                cube[id].side = 2;
		    } else if ((r == 0) && (moveCube(id, 0,0,-1) == 0)) {
	                cube[id].side = 3;
		    } else if ((r == 0) && (moveCube(id, 0,-1,0) == 0)) {
	                cube[id].side = 4;
		    } else {
		        // change dir with random wait
                        cube[id].dir = cube[id].dir * -1;
                        cube[id].delay = (t==1)? rand()%7 : rand()%3;
		    }
		}
            }
	}

        render_cubes();
        writeFrame(ptr);
    }

    fclose(ptr);
}
