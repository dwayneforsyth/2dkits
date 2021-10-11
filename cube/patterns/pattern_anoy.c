
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core_utils.h"
#include "core_helper.h"

void main( void ) {
    createPattern("anoy.pat","anoy");
    uint8_t i;
    uint8_t r,g,b;

    patternFrame.cycles = 1;
    patternFrame.delay = 1;

    allLedsColor( 15,  0,  0);
    writeFrame();

    allLedsColor(  0, 15,  0);
    writeFrame();

    allLedsColor(  0,  0, 15);
    writeFrame();

    allLedsColor( 15,  0,  0);
    writeFrame();

    allLedsColor(  0, 15,  0);
    writeFrame();

    allLedsColor(  0,  0, 15);
    writeFrame();

    allLedsColor( 15,  0,  0);
    writeFrame();

    allLedsColor(  0, 15,  0);
    writeFrame();

    allLedsColor(  0,  0, 15);
    writeFrame();

    finishPattern();
}
