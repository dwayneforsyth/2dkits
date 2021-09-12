
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "core_utils.h"

typedef struct {
   uint8_t len;
   uint8_t colorRed;
   uint8_t colorGreen;
   uint8_t colorBlue;
} drop_t;

void shiftDown( void ) {
    uint8_t r,g,b;
    for (uint8_t l = 1; l < 8; l++) {
        for (uint8_t x = 0; x < 4; x++) {
            for (uint8_t y = 0; y < 4; y++) {
                getLed(l,x,y,&r, &g, &b);
		setLed(l-1,x,y, r, g, b);
	    }
	}
    }
}

#define LOOPS 100

void main( void ) {

    FILE *ptr = fopen("rain.pat","wb");
    strncpy(header.name,"Rain",16);
    fwrite( &header, sizeof(header), 1, ptr);
    printHeader();

    patternFrame.cycles = 8;
    patternFrame.delay = 1;

    drop_t top[4][4];
    for (int8_t x = 0; x < 4; x++) {
        for (int8_t y=0; y < 4; y++) {
	    top[x][y].len = 0;
            setLed(7,x,y, 0, 0, 0);
	}
    }

    for (uint16_t loop = 0; loop < LOOPS; loop++) {

	if (loop < (LOOPS - 8)) {
            uint8_t drops = rand()%4;    
            for (uint8_t drop=0; drop < drops; drop++) {
                uint8_t x = rand()%4;
                uint8_t y = rand()%4;
	        top[x][y].len = rand()%4;
	        top[x][y].colorRed = rand()%16;
	        top[x][y].colorGreen = rand()%16;
	        top[x][y].colorBlue = rand()%16;
            }
        }

	shiftDown();

	for (int8_t x = 0; x < 4; x++) {
	    for (int8_t y=0; y < 4; y++) {
                if (top[x][y].len == 0) {
                    setLed(7,x,y, 0, 0, 0);
		} else {
                    setLed(7,x,y, top[x][y].colorRed, top[x][y].colorGreen, top[x][y].colorBlue);
		    top[x][y].len--;
		}
	    }
	}

        writeFrame(ptr);
    }

    fclose(ptr);
}

