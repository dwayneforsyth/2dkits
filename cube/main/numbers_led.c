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

void displayNumber(uint8_t number) {
   uint8_t temp;
   uint8_t digit1, digit2;
   uint8_t l,i;

   number++;

   setLed3(0,0,0, ((number&0x01)==0x01)? 1:0,0,0);
   setLed3(0,0,1, ((number&0x02)==0x02)? 1:0,0,0);
   setLed3(0,0,2, ((number&0x04)==0x04)? 1:0,0,0);
   setLed3(0,0,3, ((number&0x08)==0x08)? 1:0,0,0);

   setLed3(0,1,0, ((number&0x10)==0x10)? 1:0,0,0);
   setLed3(0,1,1, ((number&0x20)==0x20)? 1:0,0,0);
   setLed3(0,1,2, ((number&0x40)==0x40)? 1:0,0,0);
   setLed3(0,1,3, ((number&0x80)==0x80)? 1:0,0,0);
}

