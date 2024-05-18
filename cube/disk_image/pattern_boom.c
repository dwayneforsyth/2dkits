

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core_utils.h"
#include "core_helper.h"

void main(void) {
    int8_t l,x,y;

    createPattern("boom4.pat", "Boom");

    uint8_t color, red_up, green_up, blue_up;

    for (color=1;color < 8;color++) {
        red_up     = (color & 0x01)? LED_PLUS  : LED_NOOP; 
        green_up   = (color & 0x02)? LED_PLUS  : LED_NOOP; 
        blue_up    = (color & 0x04)? LED_PLUS  : LED_NOOP; 

        allLedsColor(LED_MINUS, LED_MINUS, LED_MINUS);
        for (l=1;l<3;l++) {
            setLed(l,1,1, red_up, green_up, blue_up);
            setLed(l,1,2, red_up, green_up, blue_up);
            setLed(l,2,1, red_up, green_up, blue_up);
            setLed(l,2,2, red_up, green_up, blue_up);
        }
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame();

        for (l=0;l<4;l++) {
            setLed(l,1,1, red_up, green_up, blue_up);
            setLed(l,1,2, red_up, green_up, blue_up);
            setLed(l,2,1, red_up, green_up, blue_up);
            setLed(l,2,2, red_up, green_up, blue_up);
        }
        for (l=0;l<4;l++) {
            setLed(l,0,1, red_up, green_up, blue_up);
            setLed(l,0,2, red_up, green_up, blue_up);
            setLed(l,1,0, red_up, green_up, blue_up);
            setLed(l,1,3, red_up, green_up, blue_up);
            setLed(l,2,0, red_up, green_up, blue_up);
            setLed(l,2,3, red_up, green_up, blue_up);
            setLed(l,3,1, red_up, green_up, blue_up);
            setLed(l,3,2, red_up, green_up, blue_up);
        }
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame();
    
        for (l=0;l<4;l++) {
            setLed(l,1,1, red_up, green_up, blue_up);
            setLed(l,1,2, red_up, green_up, blue_up);
            setLed(l,2,1, red_up, green_up, blue_up);
            setLed(l,2,2, red_up, green_up, blue_up);
        }
        for (l=0;l<4;l++) {
            setLed(l,0,1, red_up, green_up, blue_up);
            setLed(l,0,2, red_up, green_up, blue_up);
            setLed(l,1,0, red_up, green_up, blue_up);
            setLed(l,1,3, red_up, green_up, blue_up);
            setLed(l,2,0, red_up, green_up, blue_up);
            setLed(l,2,3, red_up, green_up, blue_up);
            setLed(l,3,1, red_up, green_up, blue_up);
            setLed(l,3,2, red_up, green_up, blue_up);
        }
        levelLedsColor(1, red_up, green_up, blue_up);
        levelLedsColor(2, red_up, green_up, blue_up);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame();

        for (l=0;l<4;l++) {
            setLed(l,1,1, red_up, green_up, blue_up);
            setLed(l,1,2, red_up, green_up, blue_up);
            setLed(l,2,1, red_up, green_up, blue_up);
            setLed(l,2,2, red_up, green_up, blue_up);
        }
        for (l=0;l<4;l++) {
            setLed(l,0,1, red_up, green_up, blue_up);
            setLed(l,0,2, red_up, green_up, blue_up);
            setLed(l,1,0, red_up, green_up, blue_up);
            setLed(l,1,3, red_up, green_up, blue_up);
            setLed(l,2,0, red_up, green_up, blue_up);
            setLed(l,2,3, red_up, green_up, blue_up);
            setLed(l,3,1, red_up, green_up, blue_up);
            setLed(l,3,2, red_up, green_up, blue_up);
        }
        levelLedsColor(0, red_up, green_up, blue_up);
        levelLedsColor(1, red_up, green_up, blue_up);
        levelLedsColor(2, red_up, green_up, blue_up);
        levelLedsColor(3, red_up, green_up, blue_up);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame();

        for (l=0;l<4;l++) {
            setLed(l,0,1, red_up, green_up, blue_up);
            setLed(l,0,2, red_up, green_up, blue_up);
            setLed(l,1,0, red_up, green_up, blue_up);
            setLed(l,1,3, red_up, green_up, blue_up);
            setLed(l,2,0, red_up, green_up, blue_up);
            setLed(l,2,3, red_up, green_up, blue_up);
            setLed(l,3,1, red_up, green_up, blue_up);
            setLed(l,3,2, red_up, green_up, blue_up);
        }
        levelLedsColor(0, red_up, green_up, blue_up);
        levelLedsColor(1, red_up, green_up, blue_up);
        levelLedsColor(2, red_up, green_up, blue_up);
        levelLedsColor(3, red_up, green_up, blue_up);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame();

    allLedsColor( red_up, green_up, blue_up);
        for (l=1;l<3;l++) {
            setLed(l,1,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,1,2, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,2, LED_MINUS, LED_MINUS, LED_MINUS);
        }
        patternFrame.cycles = 8;
    patternFrame.delay = 1;
        writeFrame();

        for (l=0;l<4;l++) {
            setLed(l,1,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,1,2, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,2, LED_MINUS, LED_MINUS, LED_MINUS);
         }
         for (l=0;l<4;l++) {
             setLed(l,0,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,0,2, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,0, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,3, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,0, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,3, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,3,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,3,2, LED_MINUS, LED_MINUS, LED_MINUS);
         }
         patternFrame.cycles = 8;
         patternFrame.delay = 1;
         writeFrame();
 
         for (l=0;l<4;l++) {
             setLed(l,1,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,2, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,2, LED_MINUS, LED_MINUS, LED_MINUS);
         }
         for (l=0;l<4;l++) {
             setLed(l,0,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,0,2, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,0, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,3, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,0, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,3, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,3,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,3,2, LED_MINUS, LED_MINUS, LED_MINUS);
         }
         levelLedsColor(1, LED_MINUS, LED_MINUS, LED_MINUS);
         levelLedsColor(2, LED_MINUS, LED_MINUS, LED_MINUS);
         patternFrame.cycles = 8;
         patternFrame.delay = 1;
         writeFrame();
 
         for (l=0;l<4;l++) {
             setLed(l,1,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,2, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,2, LED_MINUS, LED_MINUS, LED_MINUS);
         }
         for (l=0;l<4;l++) {
             setLed(l,0,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,0,2, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,0, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,1,3, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,0, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,3, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,3,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,3,2, LED_MINUS, LED_MINUS, LED_MINUS);
        }
        levelLedsColor(0, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(1, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(2, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(3, LED_MINUS, LED_MINUS, LED_MINUS);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame();

        for (l=0;l<4;l++) {
            setLed(l,0,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,0,2, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,1,0, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,1,3, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,0, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,3, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,3,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,3,2, LED_MINUS, LED_MINUS, LED_MINUS);
        }
        levelLedsColor(0, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(1, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(2, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(3, LED_MINUS, LED_MINUS, LED_MINUS);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame();

        allLedsColor(LED_MINUS, LED_MINUS, LED_MINUS);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame();

    }
    finishPattern();
}
