

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core_utils.h"

void make_boom(void) {
    int8_t l,x,y;

    FILE *ptr = fopen("boom.pat","wb");
    strncpy(header.name,"Boom",16);
    fwrite( &header, sizeof(header), 1, ptr);
    printHeader();
    uint8_t color, red_up, green_up, blue_up;

    for (color=1;color < 8;color++) {
        red_up     = (color & 0x01)? LED_PLUS  : LED_NOOP; 
        green_up   = (color & 0x02)? LED_PLUS  : LED_NOOP; 
        blue_up    = (color & 0x04)? LED_PLUS  : LED_NOOP; 

        allLedsColor(LED_MINUS, LED_MINUS, LED_MINUS);
        for (l=3;l<5;l++) {
            setLed(l,1,1, red_up, green_up, blue_up);
            setLed(l,1,2, red_up, green_up, blue_up);
            setLed(l,2,1, red_up, green_up, blue_up);
            setLed(l,2,2, red_up, green_up, blue_up);
        }
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame(ptr);

        for (l=2;l<6;l++) {
            setLed(l,1,1, red_up, green_up, blue_up);
            setLed(l,1,2, red_up, green_up, blue_up);
            setLed(l,2,1, red_up, green_up, blue_up);
            setLed(l,2,2, red_up, green_up, blue_up);
        }
        for (l=3;l<5;l++) {
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
        writeFrame(ptr);
    
        for (l=1;l<7;l++) {
            setLed(l,1,1, red_up, green_up, blue_up);
            setLed(l,1,2, red_up, green_up, blue_up);
            setLed(l,2,1, red_up, green_up, blue_up);
            setLed(l,2,2, red_up, green_up, blue_up);
        }
        for (l=2;l<6;l++) {
            setLed(l,0,1, red_up, green_up, blue_up);
            setLed(l,0,2, red_up, green_up, blue_up);
            setLed(l,1,0, red_up, green_up, blue_up);
            setLed(l,1,3, red_up, green_up, blue_up);
            setLed(l,2,0, red_up, green_up, blue_up);
            setLed(l,2,3, red_up, green_up, blue_up);
            setLed(l,3,1, red_up, green_up, blue_up);
            setLed(l,3,2, red_up, green_up, blue_up);
        }
        levelLedsColor(3, red_up, green_up, blue_up);
        levelLedsColor(4, red_up, green_up, blue_up);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame(ptr);

        for (l=0;l<8;l++) {
            setLed(l,1,1, red_up, green_up, blue_up);
            setLed(l,1,2, red_up, green_up, blue_up);
            setLed(l,2,1, red_up, green_up, blue_up);
            setLed(l,2,2, red_up, green_up, blue_up);
        }
        for (l=1;l<7;l++) {
            setLed(l,0,1, red_up, green_up, blue_up);
            setLed(l,0,2, red_up, green_up, blue_up);
            setLed(l,1,0, red_up, green_up, blue_up);
            setLed(l,1,3, red_up, green_up, blue_up);
            setLed(l,2,0, red_up, green_up, blue_up);
            setLed(l,2,3, red_up, green_up, blue_up);
            setLed(l,3,1, red_up, green_up, blue_up);
            setLed(l,3,2, red_up, green_up, blue_up);
        }
        levelLedsColor(2, red_up, green_up, blue_up);
        levelLedsColor(3, red_up, green_up, blue_up);
        levelLedsColor(4, red_up, green_up, blue_up);
        levelLedsColor(5, red_up, green_up, blue_up);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame(ptr);

        for (l=0;l<8;l++) {
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
        levelLedsColor(3, red_up, green_up, blue_up);
        levelLedsColor(4, red_up, green_up, blue_up);
        levelLedsColor(5, red_up, green_up, blue_up);
        levelLedsColor(6, red_up, green_up, blue_up);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame(ptr);

    allLedsColor( red_up, green_up, blue_up);
        for (l=3;l<5;l++) {
            setLed(l,1,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,1,2, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,2, LED_MINUS, LED_MINUS, LED_MINUS);
        }
        patternFrame.cycles = 8;
    patternFrame.delay = 1;
        writeFrame(ptr);

        for (l=2;l<6;l++) {
            setLed(l,1,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,1,2, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,2, LED_MINUS, LED_MINUS, LED_MINUS);
         }
         for (l=3;l<5;l++) {
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
         writeFrame(ptr);
 
         for (l=1;l<7;l++) {
             setLed(l,1,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,2, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,2, LED_MINUS, LED_MINUS, LED_MINUS);
         }
         for (l=2;l<6;l++) {
             setLed(l,0,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,0,2, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,0, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,3, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,0, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,3, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,3,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,3,2, LED_MINUS, LED_MINUS, LED_MINUS);
         }
         levelLedsColor(3, LED_MINUS, LED_MINUS, LED_MINUS);
         levelLedsColor(4, LED_MINUS, LED_MINUS, LED_MINUS);
         patternFrame.cycles = 8;
         patternFrame.delay = 1;
         writeFrame(ptr);
 
         for (l=0;l<8;l++) {
             setLed(l,1,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,2, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,2,2, LED_MINUS, LED_MINUS, LED_MINUS);
         }
         for (l=1;l<7;l++) {
             setLed(l,0,1, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,0,2, LED_MINUS, LED_MINUS, LED_MINUS);
             setLed(l,1,0, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,1,3, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,0, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,2,3, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,3,1, LED_MINUS, LED_MINUS, LED_MINUS);
            setLed(l,3,2, LED_MINUS, LED_MINUS, LED_MINUS);
        }
        levelLedsColor(2, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(3, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(4, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(5, LED_MINUS, LED_MINUS, LED_MINUS);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame(ptr);

        for (l=0;l<8;l++) {
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
        levelLedsColor(3, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(4, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(5, LED_MINUS, LED_MINUS, LED_MINUS);
        levelLedsColor(6, LED_MINUS, LED_MINUS, LED_MINUS);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame(ptr);

        allLedsColor(LED_MINUS, LED_MINUS, LED_MINUS);
        patternFrame.cycles = 8;
        patternFrame.delay = 1;
        writeFrame(ptr);

    }
    fclose(ptr);
}
