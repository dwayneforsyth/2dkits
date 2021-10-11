
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core_utils.h"
#include "core_helper.h"

void axisLLedsColor( int8_t l, int8_t red, int8_t green, int8_t blue) {
   uint8_t x,y;
   uint8_t r,g,b;

   for (x=0;x<4;x++) {
      for (y=0;y<4;y++) {
          getLed(l,x,y, &r, &g, &b);
          if (red != -1) { r = red; }
          if (green != -1) { g = green; }
          if (blue != -1) { b = blue; }
          setLed(l,x,y, r, g, b);
}  }  }

void axisYLedsColor( int8_t x, int8_t red, int8_t green, int8_t blue) {
   uint8_t l,y;
   uint8_t r,g,b;

   for (y=0;y<4;y++) {
      for (l=0;l<4;l++) {
          getLed(l,x,y, &r, &g, &b);
          if (red != -1) { r = red; }
          if (green != -1) { g = green; }
          if (blue != -1) { b = blue; }
          setLed(l,x,y, r, g, b);
}  }  } 

void axisXLedsColor( int8_t y, int8_t red, int8_t green, int8_t blue) {
   uint8_t l,x;
   uint8_t r,g,b;

   for (l=0;l<4;l++) {
      for (x=0;x<4;x++) {
          getLed(l,x,y, &r, &g, &b);
          if (red != -1) { r = red; }
          if (green != -1) { g = green; }
          if (blue != -1) { b = blue; }
          setLed(l,x,y, r, g, b);
}  }  } 

void main( void ) {
    createPattern("space.pat","space");
    uint8_t i;
    uint8_t r,g,b;

    patternFrame.cycles = 4;
    patternFrame.delay = 4;

    allLedsColor(LED_MINUS, LED_MINUS, LED_MINUS);

    // 0
    axisLLedsColor( 0, 15, -1, -1);
    writeFrame();

    // 1
    axisLLedsColor( 1, 15, -1, -1);
    writeFrame();

    // 2
    axisLLedsColor( 2, 15, -1, -1);
    writeFrame();

    // 3
    axisLLedsColor( 3, 15, -1, -1);
    writeFrame();

    // 4
    axisYLedsColor( 3,  0, -1, -1);
    writeFrame();

    // 5
    axisYLedsColor( 2,  0, -1, -1);
    writeFrame();

    // 6
    axisYLedsColor( 1,  0, -1, -1);
    writeFrame();

    // 7
    axisYLedsColor( 0,  0, -1, -1);
    writeFrame();

    // 8
    axisYLedsColor( 0, -1, 15, -1);
    writeFrame();

    // 9
    axisYLedsColor( 1, -1, 15, -1);
    writeFrame();

    // 10
    axisYLedsColor( 2, -1, 15, -1);
    writeFrame();

    // 11
    axisYLedsColor( 3, -1, 15, -1);
    writeFrame();

    // 12
    axisXLedsColor( 3, -1,  0, -1);
    writeFrame();

    // 13
    axisXLedsColor( 2, -1,  0, -1);
    writeFrame();

    // 14
    axisXLedsColor( 1, -1,  0, -1);
    writeFrame();

    // 15
    axisXLedsColor( 0, -1,  0, -1);
    writeFrame();

    // 16
    axisXLedsColor( 0, -1, -1, 15);
    writeFrame();

    // 17
    axisXLedsColor( 1, -1, -1, 15);
    writeFrame();

    // 18
    axisXLedsColor( 2, -1, -1, 15);
    writeFrame();

    // 19
    axisXLedsColor( 3, -1, -1, 15);
    writeFrame();

    // 20
    axisLLedsColor( 3, -1, -1,  0);
    writeFrame();

    // 21
    axisLLedsColor( 2, -1, -1,  0);
    writeFrame();

    // 22
    axisLLedsColor( 1, -1, -1,  0);
    writeFrame();

    // 23
    axisLLedsColor( 0, -1, -1,  0);
    writeFrame();

    // 24
    axisLLedsColor( 0, -1, -1, 15);
    writeFrame();

    // 25
    axisLLedsColor( 1, -1, -1, 15);
    writeFrame();

    // 26
    axisLLedsColor( 2, -1, -1, 15);
    writeFrame();

    // 27
    axisLLedsColor( 3, -1, -1, 15);
    writeFrame();

    // 28
    axisYLedsColor( 3, -1, -1,  0);
    writeFrame();

    // 29
    axisYLedsColor( 2, -1, -1,  0);
    writeFrame();

    // 30
    axisYLedsColor( 1, -1, -1,  0);
    writeFrame();

    // 31
    axisYLedsColor( 0, -1, -1,  0);
    writeFrame();

    // 32
    axisYLedsColor( 0, -1, 15, -1);
    writeFrame();

    // 33
    axisYLedsColor( 1, -1, 15, -1);
    writeFrame();

    // 34
    axisYLedsColor( 2, -1, 15, -1);
    writeFrame();

    // 35
    axisYLedsColor( 3, -1, 15, -1);
    writeFrame();

    // 36
    axisXLedsColor( 3, -1,  0, -1);
    writeFrame();

    // 37
    axisXLedsColor( 2, -1,  0, -1);
    writeFrame();

    // 38
    axisXLedsColor( 1, -1,  0, -1);
    writeFrame();

    // 39
    axisXLedsColor( 0, -1,  0, -1);
    writeFrame();

    // 40
    axisXLedsColor( 0, -1, -1, 15);
    writeFrame();

    // 41
    axisXLedsColor( 1, -1, -1, 15);
    writeFrame();

    // 42
    axisXLedsColor( 2, -1, -1, 15);
    writeFrame();

    // 43
    axisXLedsColor( 3, -1, -1, 15);
    writeFrame();

    // 44
    axisLLedsColor( 3, -1, -1,  0);
    writeFrame();

    // 45
    axisLLedsColor( 2, -1, -1,  0);
    writeFrame();

    // 46
    axisLLedsColor( 1, -1, -1,  0);
    writeFrame();

    // 47
    axisLLedsColor( 0, -1, -1,  0);
    writeFrame();

    finishPattern();
}
