
#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// header is 24 bytes
typedef struct {
   uint8_t type;
   uint8_t speed;
   uint8_t cycles;
   uint8_t free[5];
   uint8_t name[16];
} blinkieHeader_t;

typedef struct {
   uint16_t led[8][4][4];
   uint8_t cycles;
   uint8_t delay;
} blinkieFrame32_t;

typedef struct {
   uint16_t led[4][4][4];
   uint8_t cycles;
   uint8_t delay;
} blinkieFrame33_t;

#define LED_PLUS 16
#define LED_MINUS 17
#define LED_NOOP 18

extern blinkieFrame33_t patternFrame;
extern blinkieHeader_t header;

// core make 
void writeFrame(void);
void createPattern( char * file, char * title);
void finishPattern( void );

// update LEDs in fame
void setLed(uint8_t l, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB);
void getLed(uint8_t l, uint8_t x, uint8_t y, uint8_t *iR, uint8_t *iG, uint8_t *iB);

// Debug
void printHeader(void);
void printFrame(void);
