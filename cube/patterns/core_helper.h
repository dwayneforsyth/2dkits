
#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Helpers
void allLedsColor( uint8_t red, uint8_t green, uint8_t blue);
void levelLedsColor( uint8_t l, uint8_t red, uint8_t green, uint8_t blue);
void allLedsOff(void);
void shiftDown( void );
