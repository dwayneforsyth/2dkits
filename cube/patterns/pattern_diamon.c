
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core_utils.h"
#include "core_helper.h"

static void dia(uint8_t line, uint8_t l, uint8_t r, uint8_t g, uint8_t b) {

    switch (line) {
        case 0:
            setLed(l,0,0, r,g,b);
	    break;

	case 1:
            setLed(l,1,0, r,g,b);
            setLed(l,0,1, r,g,b);
            break;

	case 2:
            setLed(l,2,0, r,g,b);
            setLed(l,1,1, r,g,b);
            setLed(l,0,2, r,g,b);
            break;

	case 3:
            setLed(l,3,0, r,g,b);
            setLed(l,2,1, r,g,b);
            setLed(l,1,2, r,g,b);
            setLed(l,0,3, r,g,b);
            break;

	case 4:
            setLed(l,3,1, r,g,b);
            setLed(l,2,2, r,g,b);
            setLed(l,1,3, r,g,b);
            break;

	case 5:
            setLed(l,3,2, r,g,b);
            setLed(l,2,3, r,g,b);
            break;

	case 6:
            setLed(l,3,3, r,g,b);
            break;
    }
}

void diamon_slicer(uint8_t slice, uint8_t r, uint8_t g, uint8_t b) {

    switch (slice) {
    case 0:
        dia(0, 0, r,g,b);
	break;

    case 1:
        dia(0, 1, r,g,b);
        dia(1, 0, r,g,b);
	break;
   
    case 2:	
        dia(0, 2, r,g,b);
        dia(1, 1, r,g,b);
        dia(2, 0, r,g,b);
	break;

    case 3:
        dia(0, 3, r,g,b);
        dia(1, 2, r,g,b);
        dia(2, 1, r,g,b);
        dia(3, 0, r,g,b);
	break;
   
    case 4:	
        dia(0, 4, r,g,b);
        dia(1, 3, r,g,b);
        dia(2, 2, r,g,b);
        dia(3, 1, r,g,b);
        dia(4, 0, r,g,b);
        break;

    case 5:
        dia(0, 5, r,g,b);
        dia(1, 4, r,g,b);
        dia(2, 3, r,g,b);
        dia(3, 2, r,g,b);
        dia(4, 1, r,g,b);
        dia(5, 0, r,g,b);
	break;
   
    case 6:	
        dia(0, 6, r,g,b);
        dia(1, 5, r,g,b);
        dia(2, 4, r,g,b);
        dia(3, 3, r,g,b);
        dia(4, 2, r,g,b);
        dia(5, 1, r,g,b);
        dia(6, 0, r,g,b);
	break;

    case 7:	
        dia(0, 7, r,g,b);
        dia(1, 6, r,g,b);
        dia(2, 5, r,g,b);
        dia(3, 4, r,g,b);
        dia(4, 3, r,g,b);
        dia(5, 2, r,g,b);
        dia(6, 1, r,g,b);
        break;

    case 8:
        dia(1, 7, r,g,b);
        dia(2, 6, r,g,b);
        dia(3, 5, r,g,b);
        dia(4, 4, r,g,b);
        dia(5, 3, r,g,b);
        dia(6, 2, r,g,b);
        break;

    case 9:
        dia(2, 7, r,g,b);
        dia(3, 6, r,g,b);
        dia(4, 5, r,g,b);
        dia(5, 4, r,g,b);
        dia(6, 3, r,g,b);
	break;

    case 10:
        dia(3, 7, r,g,b);
        dia(4, 6, r,g,b);
        dia(5, 5, r,g,b);
        dia(6, 4, r,g,b);
	break;
   
    case 11:	
        dia(4, 7, r,g,b);
        dia(5, 6, r,g,b);
        dia(6, 5, r,g,b);
	break;
   
    case 12:	
        dia(5, 7, r,g,b);
        dia(6, 6, r,g,b);
	break;

    case 13:
        dia(6, 7, r,g,b);
	break;
    }    
}
void make_diamon1( void ) {
    int8_t i;

    createPattern("diamon2.pat","Diamon 2");

    patternFrame.cycles = 8;
    patternFrame.delay = 1;

    for (i=0;i<17;i++) {
        allLedsColor(LED_MINUS, LED_MINUS, LED_MINUS);
        if ((i>4)&&(i<19)) diamon_slicer(i-5, LED_MINUS, LED_MINUS, LED_PLUS); // 5 - 18
        if ((i>3)&&(i<18)) diamon_slicer(i-4, LED_MINUS, LED_MINUS, LED_PLUS); // 4 - 17
        if ((i>2)&&(i<17)) diamon_slicer(i-3, LED_MINUS, LED_PLUS, LED_MINUS); // 3 - 16
        if ((i>1)&&(i<16)) diamon_slicer(i-2, LED_MINUS, LED_PLUS, LED_MINUS); // 2 - 15
        if ((i>0)&&(i<15)) diamon_slicer(i-1, LED_PLUS, LED_MINUS, LED_MINUS); // 1 - 14
        if         (i<14)  diamon_slicer(i,   LED_PLUS, LED_MINUS, LED_MINUS); // 0 - 13
        writeFrame();
    }
    finishPattern();
}

void make_diamon2( void ) {
    int8_t i;

    createPattern("diamon1.pat","Diamon 1");

    patternFrame.cycles = 8;
    patternFrame.delay = 4;

    for (i=0;i<22;i++) {
        allLedsColor(LED_MINUS, LED_MINUS, LED_MINUS);
        if ((i>7)&&(i<22)) diamon_slicer(i-5, LED_PLUS, LED_MINUS, LED_MINUS); // 8 - 21
        if ((i>6)&&(i<21)) diamon_slicer(i-5, LED_PLUS, LED_MINUS, LED_MINUS); // 7 - 20
        if ((i>5)&&(i<20)) diamon_slicer(i-4, LED_PLUS, LED_MINUS, LED_MINUS); // 6 - 19
        if ((i>4)&&(i<19)) diamon_slicer(i-3, LED_MINUS, LED_PLUS, LED_MINUS); // 5 - 18
        if ((i>3)&&(i<18)) diamon_slicer(i-3, LED_MINUS, LED_PLUS, LED_MINUS); // 4 - 17
        if ((i>2)&&(i<17)) diamon_slicer(i-2, LED_MINUS, LED_PLUS, LED_MINUS); // 3 - 16
        if ((i>1)&&(i<16)) diamon_slicer(i-1, LED_MINUS, LED_MINUS, LED_PLUS); // 2 - 15
        if ((i>0)&&(i<15)) diamon_slicer(i-1, LED_MINUS, LED_MINUS, LED_PLUS); // 1 - 14
        if         (i<14)  diamon_slicer(i,   LED_MINUS, LED_MINUS, LED_PLUS); // 0 - 13
        writeFrame();
    }
    finishPattern();
}

void main( void ) {
    make_diamon1();
    make_diamon2();
}
