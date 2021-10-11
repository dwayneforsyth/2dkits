
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
    createPattern("axis.pat","axis 1");
    uint8_t i;
    uint8_t r,g,b;

    patternFrame.cycles = 4;
    patternFrame.delay = 4;

    for (i=0;i<15;i++) {
        allLedsColor(LED_MINUS, LED_MINUS, LED_MINUS);
        switch ( i ) {
	case 0:
            axisLLedsColor( 0, 15, -1, -1);
            axisYLedsColor( 1, -1, 15, -1);
	    break;
	case 1:
            axisLLedsColor( 1, 15, -1, -1);
            axisYLedsColor( 2, -1, 15, -1);
            axisXLedsColor( 0, -1, -1, 15);
	    break;
	case 2:
            axisLLedsColor( 2, 15, -1, -1);
            axisYLedsColor( 3, -1, 15, -1);
            axisXLedsColor( 1, -1, -1, 15);
	    break;
	case 3:
            axisLLedsColor( 3, 15, -1, -1);
            axisXLedsColor( 2, -1, -1, 15);
	    break;
	case 4:
            axisYLedsColor( 1, 15, -1, -1);
            axisYLedsColor( 3, -1, -1, 15);
	    break;
	case 5:
            axisXLedsColor( 0, 15, -1, -1);
            axisYLedsColor( 1, -1, 15, -1);
	    break;
	case 6:
            axisXLedsColor( 1, 15, -1, -1);
            axisYLedsColor( 2, -1, 15, -1);
            axisLLedsColor( 0, -1, -1, 15);
	    break;
	case 7:
            axisXLedsColor( 2, 15, -1, -1);
            axisYLedsColor( 3, -1, 15, -1);
            axisLLedsColor( 1, -1, -1, 15);
	    break;
	case 8:
            axisXLedsColor( 3, 15, -1, -1);
            axisLLedsColor( 2, -1, -1, 15);
	    break;
	case 9:
            axisLLedsColor( 0, -1, 15, -1);
            axisLLedsColor( 1, -1, -1, 15);
	    break;
	case 10:
            axisYLedsColor( 0, 15, -1, -1);
            axisLLedsColor( 1, -1, 15, -1);
	    break;
	case 11:
            axisYLedsColor( 1, 15, -1, -1);
            axisLLedsColor( 2, -1, 15, -1);
            axisXLedsColor( 0, -1, -1, 15);
	    break;
	case 12:
            axisYLedsColor( 2, 15, -1, -1);
            axisLLedsColor( 3, -1, 15, -1);
            axisXLedsColor( 1, -1, -1, 15);
	    break;
	case 13:
            axisYLedsColor( 3, 15, -1, -1);
            axisXLedsColor( 2, -1, -1, 15);
	    break;
	case 14:
            axisXLedsColor( 3, -1, 15, -1);
            axisXLedsColor( 0, -1, -1, 15);
	    break;
        }
        writeFrame();
    }
    finishPattern();
}
