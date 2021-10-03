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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "core_utils.h"
#include "core_helper.h"

typedef struct {
   uint8_t l;
   uint8_t x;
   uint8_t y;
} point_t;


/*******************************************************************************
    PURPOSE: generate pattern

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

bool checkLed(uint8_t l, uint8_t x, uint8_t y, uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t rr, gg, bb;
    getLed( l, x, y, &rr, &gg, &bb);
    if (red && rr)  { return false; }
    if (green && gg) { return false; }
    if (blue && bb) { return false; }
    return(true);
}

typedef enum {
    DIR_NORTH, 
    DIR_SOUTH,
    DIR_EAST,
    DIR_WEST,
    DIR_UP,
    DIR_DOWN,
} dir_t;

bool searchDir( point_t * point, uint8_t r, uint8_t g, uint8_t b) {
    dir_t opt = 0;
    uint8_t count = 0;
    uint8_t val = 0;
    uint8_t dir_mask = 0b00111111;

    while (dir_mask != 0) {
        uint8_t goDir = rand()%6;
        switch (goDir) {
        case DIR_NORTH:
	    if ((point->y!=3) && checkLed(point->l,point->x,point->y+1, r,g,b)) {
		point->y++;
                return( true);
	    } else {
                dir_mask -= (1<<DIR_NORTH);
	    }
            break;
        case DIR_SOUTH:
	    if ((point->y!=0) && checkLed(point->l,point->x,point->y-1, r,g,b)) {
		point->y--;
                return( true);
	    } else {
                dir_mask -= (1<<DIR_SOUTH);
	    }
            break;
        case DIR_EAST:
	    if ((point->x!=3) && checkLed(point->l,point->x+1,point->y, r,g,b)) {
		point->x++;
                return( true);
	    } else {
                dir_mask -= (1<<DIR_UP);
	    }
            break;
        case DIR_WEST:
	    if ((point->x!=0) && checkLed(point->l,point->x-1,point->y, r,g,b)) {
		point->x--;
                return( true);
	    } else {
                dir_mask -= (1<<DIR_UP);
	    }
            break;
        case DIR_UP:
	    if ((point->l!=7) && checkLed(point->l+1,point->x,point->y, r,g,b)) {
		point->l++;
                return( true);
	    } else {
                dir_mask -= (1<<DIR_UP);
	    }
            break;
        case DIR_DOWN:
	    if ((point->l!=0) && checkLed(point->l-1,point->x,point->y, r,g,b)) {
		point->l--;
                return( true);
	    } else {
                dir_mask -= (1<<DIR_DOWN);
	    }
            break;
        }
    }
    printf("no path to move\n");
    return( false );
}

#define LOOPS 130

void main( void ) {
    point_t point = {0,0,0};
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    bool moved = true;

    createPattern("maze.pat","maze");

    patternFrame.cycles = 8;
    patternFrame.delay = 1;

    for (uint8_t loop2 = 0; loop2 < 4; loop2++) {
    r = 15;
    g = 0;
    b = 0;
    moved = true;
    allLedsColor( 0, 17, 18);

    for (uint16_t loop = 0; (loop < LOOPS) && moved; loop++) {
        setLed(point.l,point.x,point.y, r,g,b);
        writeFrame();
        moved = searchDir( &point, r,g,b );
    }
    r = 0;
    g = 15;
    b = 0;
    moved = true;
    allLedsColor( 18, 0, 17);

    for (uint16_t loop = 0; (loop < LOOPS) && moved; loop++) {
        setLed(point.l,point.x,point.y, r,g,b);
        writeFrame();
        moved = searchDir( &point, r,g,b );
    }
    r = 0;
    g = 0;
    b = 15;
    moved = true;
    allLedsColor( 17, 18, 0);

    for (uint16_t loop = 0; (loop < LOOPS) && moved; loop++) {
        setLed(point.l,point.x,point.y, r,g,b);
        writeFrame();
        moved = searchDir( &point, r,g,b );
    }
    }
    finishPattern();
}

