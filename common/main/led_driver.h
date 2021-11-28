//   Copyright (C) 2019 Dwayne Forsyth
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

#pragma once

#include <stdint.h>

#define LED_PLUS 16
#define LED_MINUS 17
#define LED_NOOP 18

void init_LED_driver();
void changeBank( uint8_t select );
void setLed(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB);
void setLed2(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB);
void setLed3(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB);
void getLed(uint8_t z, uint8_t x, uint8_t y, uint8_t *iR, uint8_t *iG, uint8_t *iB);
void transferBuffer();

void allLedsOff();
void allLedsOn();
void allLedsColor( uint8_t red, uint8_t green, uint8_t blue);
void allLedsColor2( uint8_t red, uint8_t green, uint8_t blue);
