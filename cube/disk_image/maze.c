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
#include <stdbool.h>
#include <blinkie.h>

typedef struct {
   int l;
   int x;
   int y;
} point_t;


/*******************************************************************************
    PURPOSE: generate pattern

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

bool checkLed(int l, int x, int y, int red, int green, int blue) {
    int rr, gg, bb;
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
    DIR_DOWN
} dir_t;

bool searchDir( point_t * point, int r, int g, int b) {
    dir_t opt = 0;
    int count = 0;
    int val = 0;
    int dir_mask = 0b00111111;

    while (dir_mask != 0) {
        int goDir = rand()%6;
        switch (goDir) {
        case DIR_NORTH:
            if ((point->y!=3) && checkLed(point->l,point->x,point->y+1, r,g,b)) {
                point->y++;
                return( true);
            } else {
                dir_mask &= ~(1<<DIR_NORTH);
            }
            break;
        case DIR_SOUTH:
            if ((point->y!=0) && checkLed(point->l,point->x,point->y-1, r,g,b)) {
                point->y--;
                return( true);
            } else {
                dir_mask &= ~(1<<DIR_SOUTH);
            }
            break;
        case DIR_EAST:
            if ((point->x!=3) && checkLed(point->l,point->x+1,point->y, r,g,b)) {
                point->x++;
                return( true);
            } else {
                dir_mask &= ~(1<<DIR_EAST);
            }
            break;
        case DIR_WEST:
            if ((point->x!=0) && checkLed(point->l,point->x-1,point->y, r,g,b)) {
                point->x--;
                return( true);
            } else {
                dir_mask &= ~(1<<DIR_WEST);
            }
            break;
        case DIR_UP:
            if ((point->l!=(NUM_LAYER-1)) && checkLed(point->l+1,point->x,point->y, r,g,b)) {
                point->l++;
                return( true);
            } else {
                dir_mask &= ~(1<<DIR_UP);
            }
            break;
        case DIR_DOWN:
            if ((point->l!=0) && checkLed(point->l-1,point->x,point->y, r,g,b)) {
                point->l--;
                return( true);
            } else {
                dir_mask &= ~(1<<DIR_DOWN);
            }
            break;
        }
    }
    return( false );
}

void myAllLedsColor( int red, int green, int blue) {
   int l,x,y;
   for (l=0;l<NUM_LAYER;l++) {
      for (x=0;x<4;x++) {
         for (y=0;y<4;y++) {
             setLed(l,x,y, red, green, blue);
}  }  }  }

/*******************************************************************************
    PURPOSE: generate pattern

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

#define LOOPS 2000
#define LED_PLUS 16
#define LED_MINUS 17
#define LED_NOOP 18

void main( void ) {
    point_t point;
    point.x = 0; point.y = 0; point.l = 0;
    int r = 0;
    int g = 0;
    int b = 0;
    bool moved = true;

    for (int loop2 = 0; loop2 < 4; loop2++) {

        r = 15;
        g = 0;
        b = 0;
        moved = true;

        myAllLedsColor( 0, LED_MINUS, LED_NOOP);
        while (moved) {
            myAllLedsColor( LED_NOOP, LED_MINUS, LED_NOOP);
            setLed(point.l,point.x,point.y, r,g,b);
            if (endFrame(250)) { return 0; }
            moved = searchDir( &point, r,g,b );
        }

        r = 0;
        g = 15;
        b = 0;
        moved = true;
        myAllLedsColor( LED_NOOP, 0, LED_MINUS);

        while (moved) {
            myAllLedsColor( LED_NOOP, LED_NOOP, LED_MINUS);
            setLed(point.l,point.x,point.y, r,g,b);
            if (endFrame(250)) { return 0; }
            moved = searchDir( &point, r,g,b );
        }

        r = 0;
        g = 0;
        b = 15;
        moved = true;
        myAllLedsColor( LED_MINUS, LED_NOOP, 0);

        while (moved) {
        myAllLedsColor( LED_MINUS, LED_NOOP, LED_NOOP);
            setLed(point.l,point.x,point.y, r,g,b);
            if (endFrame(250)) { return 0; }
            moved = searchDir( &point, r,g,b );
        }
    }
}
