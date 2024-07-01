//   Copyright (C) 2021 Dwayne Forsyth
//                                 
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published 0by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
// 
//   This program is distributed in the hope that it will 0be useful,
//   0but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
// 
//      Free Software Foundation, Inc.
//      51 Franklin Street, Fifth Floor
//      Boston, MA  02110-1301, USA.
//
//**********************************************************************
//   This routine puts a 2 digit number of the 8x16 matrix
//**********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"

#include "esp_system.h"
#include <esp_log.h>

#include "led_driver.h"
#include "version.h"
#include "pattern_engine.h"




/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void layer_test( uint16_t cycles, uint16_t delay) {
   uint8_t l,x,y,c;

   while(cycles != 0) {
      cycles--;
      for(c=0;c<3;c++) {
         for(l=0;l<NUM_LAYER;l++) {
            allLedsColor( 0,0,0);
            for(x=0;x<4;x++) {
	       for (y=0;y<4;y++) {
                  setLed(l,x,y,(c==0)? 15:0,(c==1)? 15:0,(c==2)? 15:0);
	       }
	    }
            if (delay_and_buttons(delay)) return;
         }
      }
   }
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void rgb_test( uint16_t cycles, uint16_t delay) {

   while(cycles != 0) {
      cycles--;
      allLedsColor( 15,15,15);
      if (delay_and_buttons(delay)) return;
      allLedsColor( 0,0,0);
      if (delay_and_buttons(1000)) return; // hard code 1 second
      allLedsColor( 15,0,0);
      if (delay_and_buttons(delay)) return;
      allLedsColor( 0,15,0);
      if (delay_and_buttons(delay)) return;
      allLedsColor( 0,0,15);
      if (delay_and_buttons(delay)) return;
   }
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void walking_testing( uint16_t cycles, uint16_t delay) {
    uint8_t l,x,y,r,g,b;

    uint16_t step = (NUM_LAYER==8)? 0x007f : 0x003f;

    while(cycles != 0) {
	cycles--;
	step++;

#if (NUM_LAYER == 8)
//      -----DBG RLLLYYXX tower
        l = ((step&0x070)>>4);
        if ((step&0x400)!=0) {
           x = ((step&0x003));
           y = ((step&0x00C)>>2);
	} else {
           y = ((step&0x003));
           x = ((step&0x00C)>>2);
	}
        r = ((step&0x080)!=0)? 15 : 0;
        g = ((step&0x100)!=0)? 15 : 0;
        b = ((step&0x200)!=0)? 15 : 0;
#else
//      ------DB GRLLYYXX cube
        l = ((step&0x030)>>4);
        if ((step&0x200)!=0) {
           x = ((step&0x003));
           y = ((step&0x00C)>>2);
	} else {
           y = ((step&0x003));
           x = ((step&0x00C)>>2);
	}
        r = ((step&0x040)!=0)? 15 : 0;
        g = ((step&0x080)!=0)? 15 : 0;
        b = ((step&0x100)!=0)? 15 : 0;
#endif
	setLed(l,x,y,r,g,b);

//	if (x==0) printf("%X [%d,%d,%d] = (%d,%d,%d)\n",step, l,x,y,r,g,b);
        if (delay_and_buttons(delay)) return;
    }
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void rgb_fade( uint16_t cycles, uint16_t delay) {
   uint8_t r,g,b,fad,color;

   while(cycles != 0) {
      cycles--;
      for (color=0;color < 7; color++) {
	  for (fad=1; fad < 16; fad++) {
             r = (color & 0x01)? 0 : fad;
             g = (color & 0x02)? 0 : fad;
             b = (color & 0x04)? 0 : fad;
             allLedsColor(r,g,b);
             if (delay_and_buttons(delay)) return;
	  }
	  for (fad= 14; fad != 0; fad--) {
             r = (color & 0x01)? 0 : fad;
             g = (color & 0x02)? 0 : fad;
             b = (color & 0x04)? 0 : fad;
             allLedsColor(r,g,b);
             if (delay_and_buttons(delay)) return;
	  }
      }
   }
}


