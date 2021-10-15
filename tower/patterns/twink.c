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

/*******************************************************************************
    PURPOSE: generate pattern

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

#define LOOPS 2000

void main( void ) {

    for (int loop=0; loop < LOOPS; loop++) {
        allLedsColor(  0,  0, 0);
        if (endFrame(250)) { return; }

        setLed(rand()%8,rand()%4,rand()%4, rand()%15,rand()%15,rand()%15);
        if (endFrame(10)) { return; }
    }
}
