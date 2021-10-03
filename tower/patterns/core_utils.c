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
//   This is some common code used by the different pattern generators.
//   it handles:
//    * The pattern file format
//    * setting/reading LEDs in a pattern frame
//    * some generic LED settings
//
//   This runs on a pc, not the tower
//**********************************************************************


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "core_utils.h"

blinkieFrame32_t patternFrame = {
   .delay = 10,
};
blinkieHeader_t header = {
   .type = 32,
   .speed = 100,
   .cycles = 20,
   .free = {0x00},
};

const char *LEDValue[19] = { "0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","+","-","#" };
static uint16_t frame=0;
FILE *ptr;

/*******************************************************************************
    PURPOSE: sets an LED value in the pattern frame

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void setLed(uint8_t l, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB) {
    patternFrame.led[l][x][y] = (iR <<10) | (iG <<5) | iB;
}

/*******************************************************************************
    PURPOSE: gets the LED value in the pattern frame

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void getLed(uint8_t l, uint8_t x, uint8_t y, uint8_t *iR, uint8_t *iG, uint8_t *iB) {
    *iB = patternFrame.led[l][x][y] & 0x1f;
    *iG = (patternFrame.led[l][x][y] >>5) & 0x1f;
    *iR = (patternFrame.led[l][x][y] >>10) & 0x1f;
}

/*******************************************************************************
    PURPOSE: prints info on the pattern header

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void printHeader(void) {
   printf("#######################################\n");
   printf("name=%s\n",header.name);
   printf("type=%d\n",header.type);
   printf("speed=%d\n",header.speed);
   printf("cycles=%d\n",header.cycles);
   printf("#######################################\n");
   frame = 0;
}

/*******************************************************************************
    PURPOSE: prints the pattern frame

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void printFrame(void) {
   int8_t l,x,y;
   uint8_t red,green,blue;

   printf("\nframe %d\n",frame);
   for (l=7;l>=0;l--) {
      printf("#level %d\n",l);
      for (x=0;x<4;x++) {
         for (y=0;y<4;y++) {
             getLed(l,x,y, &red, &green, &blue);
             printf(" (%2s,%2s,%2s)",LEDValue[red],LEDValue[green],LEDValue[blue]);
         }
         printf("\n");
      }
   }
   printf("cycles=%d\n",patternFrame.cycles);
   frame++;
}

/*******************************************************************************
    PURPOSE: writes the frame of LED data to file

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void writeFrame( void ) {
        fwrite( &patternFrame, sizeof(patternFrame), 1, ptr);
}

/*******************************************************************************
    PURPOSE: creates a pattern file, and write the header to it.

    INPUTS: file - name of disk file
            title - title of pattern

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void createPattern( char * file, char * title) {
    ptr = fopen(file,"wb");
    strncpy(header.name,title,16);
    fwrite( &header, sizeof(header), 1, ptr);
    printHeader();
}

/*******************************************************************************
    PURPOSE: finishes a pattern file

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void finishPattern( void ) {
    fclose(ptr);
}

