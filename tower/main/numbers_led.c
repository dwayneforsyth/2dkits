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
//   This routine puts a 2 digit number of the tower
//**********************************************************************


#include <stdbool.h>
#include <stdint.h>

#include "led_driver.h"

const bool digitMap[10][7][4] = {

   [0][6]={1,1,1,1},
   [0][5]={1,0,0,1},
   [0][4]={1,0,0,1},
   [0][3]={1,0,0,1},
   [0][2]={1,0,0,1},
   [0][1]={1,0,0,1},
   [0][0]={1,1,1,1},

   [1][6]={0,0,0,1},
   [1][5]={0,0,0,1},
   [1][4]={0,0,0,1},
   [1][3]={0,0,0,1},
   [1][2]={0,0,0,1},
   [1][1]={0,0,0,1},
   [1][0]={0,0,0,1},

   [2][6]={1,1,1,1},
   [2][5]={0,0,0,1},
   [2][4]={0,0,0,1},
   [2][3]={1,1,1,1},
   [2][2]={1,0,0,0},
   [2][1]={1,0,0,0},
   [2][0]={1,1,1,1},

   [3][6]={1,1,1,1},
   [3][5]={0,0,0,1},
   [3][4]={0,0,0,1},
   [3][3]={0,1,1,1},
   [3][2]={0,0,0,1},
   [3][1]={0,0,0,1},
   [3][0]={1,1,1,1},

   [4][6]={1,0,0,1},
   [4][5]={1,0,0,1},
   [4][4]={1,0,0,1},
   [4][3]={1,1,1,1},
   [4][2]={0,0,0,1},
   [4][1]={0,0,0,1},
   [4][0]={0,0,0,1},

   [5][6]={1,1,1,1},
   [5][5]={1,0,0,0},
   [5][4]={1,0,0,0},
   [5][3]={1,1,1,1},
   [5][2]={0,0,0,1},
   [5][1]={0,0,0,1},
   [5][0]={1,1,1,1},

   [6][6]={1,1,1,1},
   [6][5]={1,0,0,0},
   [6][4]={1,0,0,0},
   [6][3]={1,1,1,1},
   [6][2]={1,0,0,1},
   [6][1]={1,0,0,1},
   [6][0]={1,1,1,1},

   [7][6]={1,1,1,1},
   [7][5]={0,0,0,1},
   [7][4]={0,0,0,1},
   [7][3]={0,0,0,1},
   [7][2]={0,0,0,1},
   [7][1]={0,0,0,1},
   [7][0]={0,0,0,1},

   [8][6]={1,1,1,1},
   [8][5]={1,0,0,1},
   [8][4]={1,0,0,1},
   [8][3]={1,1,1,1},
   [8][2]={1,0,0,1},
   [8][1]={1,0,0,1},
   [8][0]={1,1,1,1},

   [9][6]={1,1,1,1},
   [9][5]={1,0,0,1},
   [9][4]={1,0,0,1},
   [9][3]={1,1,1,1},
   [9][2]={0,0,0,1},
   [9][1]={0,0,0,1},
   [9][0]={0,0,0,1}};

void displayNumber(uint8_t number) {
   uint8_t temp;
   uint8_t digit1, digit2;
   uint8_t l,i;

   number++;

   digit2 = number / 10;
   digit1 = number % 10;
   for (l=0;l<7;l++) {
       for(i=0;i<4;i++) {
	   temp = (digitMap[digit1][l][i])? 15 : 0;
	   setLed3(l+1,0,i, 0,temp,temp);
	   temp = (digitMap[digit2][l][i])? 15 : 0;
	   setLed3(l+1,2,i, temp,0,0);
}  }   }

