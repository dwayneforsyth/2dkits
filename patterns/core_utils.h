
#ifndef PATTERN_CORE_H
#define PATTERN_CORE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// header is 24 bytes
typedef struct blinkieHeader_t {
   uint8_t type;
   uint8_t speed;
   uint8_t cycles;
   uint8_t free[5];
   uint8_t name[16];
} blinkieHeader_t;

typedef struct blinkieFrame32_t {
   uint16_t led[8][4][4];
   uint8_t cycles;
   uint8_t delay;
} blinkieFrame32_t;

#define LED_PLUS 16
#define LED_MINUS 17
#define LED_NOOP 18

extern blinkieFrame32_t patternFrame;
extern blinkieHeader_t header;

void setLed(uint8_t l, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB);
void getLed(uint8_t l, uint8_t x, uint8_t y, uint8_t *iR, uint8_t *iG, uint8_t *iB);
void allLedsColor( uint8_t red, uint8_t green, uint8_t blue);
void levelLedsColor( uint8_t l, uint8_t red, uint8_t green, uint8_t blue);
void allLedsOff(void);
void printHeader(void);
void printFrame(void);
void writeFrame(FILE *ptr);
#endif
