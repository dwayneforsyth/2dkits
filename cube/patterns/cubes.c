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

typedef struct cube_t {
   int x;
   int y;
   int l;
   int dir;
   int side;
   int delay;
   int colorRed;
   int colorGreen;
   int colorBlue;
} cube_t;

#define NUM_CUBES 4
cube_t cube[NUM_CUBES];

void initEntry( int i, int x, int y, int l, int d, int r, int g, int b) {
	cube[i].x = x;
	cube[i].y = y;
	cube[i].l = l;
	cube[i].dir = d;
	cube[i].side = 0;
	cube[i].delay = 0;
	cube[i].colorRed = r;
	cube[i].colorGreen = g;
	cube[i].colorBlue = b;
}

initEntry(0,0,0,0, 1,15, 0, 0);
initEntry(1,0,2,2,-1, 0,15, 0);
initEntry(2,2,0,0, 1, 0, 0,15);
initEntry(3,2,2,2,-1, 9, 9, 0);

/*******************************************************************************
    PURPOSE: draws the cubs in the towers LED frame

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

int render_cubes(void) {
   int c,x,y,l;
   allLedsColor2(0, 0, 0);
   for (c=0;c<NUM_CUBES;c++) {
       for (x=cube[c].x;x<cube[c].x+2;x++) {
           for (y=cube[c].y;y<cube[c].y+2;y++) {
               for (l=cube[c].l;l<cube[c].l+2;l++) {
                   setLed2(l,x,y, cube[c].colorRed,cube[c].colorGreen,cube[c].colorBlue);
    }   }   }   }
    transferBuffer();
    return(endFrame(200));
}


/*******************************************************************************
    PURPOSE: handles the moving of the cube

    INPUTS:

    RETURN CODE:
        0 - moved the cube
	1 - did not move, cube would have been off tower
	2 - did not have, cube would have overlapped another cube

    NOTES:

*******************************************************************************/
int moveCube(int id, int dl, int dx, int dy) {
    int l, x, y;

    // out of range check
    l = cube[id].l + dl;
    if ((l < 0)||(l > 2)) { return 1; }

    x = cube[id].x + dx;
    if ((x < 0)||(x > 2)) { return 1; }

    y = cube[id].y + dy;
    if ((y < 0)||(y > 2)) { return 1; }

    // hitting another cube
    for (int c=0;c<NUM_CUBES;c++) {
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

    int run = render_cubes();

    int loops = 240;
    while ((loops != 0) && (run == 0)) {
        for (int id=0;id<NUM_CUBES; id++) {
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
		int t = moveCube(id, cube[id].dir,0,0);
		if (t != 0) {
		    // blocked, random side to side
		    int r = (t==1)? rand()%7 : rand()%2;
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
        run = render_cubes();
	loops--;
    }
}
