
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core_utils.h"
#include "core_helper.h"

void main( void ) {
    createPattern("twink.pat","twink");
    uint8_t i;
    uint8_t r,g,b;

    patternFrame.cycles = 1;

    for (uint16_t loop=0; loop < 100; loop++) {
        patternFrame.delay = 10;
        allLedsColor(  0,  0, 0);
        writeFrame();

        patternFrame.delay = 1;
        setLed(rand()%4,rand()%4,rand()%4, rand()%15,rand()%15,rand()%15);
        writeFrame();
    }

    finishPattern();
}
