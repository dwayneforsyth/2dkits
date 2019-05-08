//   Copyright (C) 2019 Dwayne Forsyth
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
//   This is the pattern engine for an ESP32 based 4x4x8 tower.
//   It updates the LEDs based on the patterns, internal and disk
//   based, and takes action when buttons are pressed
//**********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_system.h"
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "driver/gpio.h"

#include "led_driver.h"
#include "web_server.h"
#include "disk_system.h"

#define MAX_PATTERN_ENTRY 50

typedef enum patternType_t {
    PATTERN_NONE,
    PATTERN_BUILT_IN,
    PATTERN_FILE
} patternType_t;

typedef struct pattern_entry_t {
    patternType_t patternType;
    void (*runMe)(uint16_t cycles, uint16_t delay);
    uint16_t delay;
    uint16_t cycles;
    char fileName[20];
    char patternName[40];
    bool enabled;
} pattern_entry_t;


bool delay_and_buttons(uint16_t delay) {
    bool exit = false;

    vTaskDelay(delay / portTICK_PERIOD_MS);
    if (gpio_get_level(39) == 0) {
        printf("Button 39\n");
	while (gpio_get_level(39) == 0)  vTaskDelay(1);
        return(true);
    }

    if (gpio_get_level(34) == 0) {
        printf("Button 34\n");
	while (gpio_get_level(34) == 0)  vTaskDelay(1);
        return(true);
    }

    return(exit);
}

/*
   This code drive the 2DKits.com 4x4x8 tower
*/

void fad_testing( uint16_t cycles, uint16_t delay) {

    bool dr = true;
    bool db = true;
    bool dg = false;
    uint8_t r = 0;
    uint8_t b = 11;
    uint8_t g = 11;

    while(cycles != 0) {
	cycles--;

	b = (db==true)? b+1 : b-1; 

	if (b==15) { db = false; }
       	else if (b==0) { db = true; }

	r = (dr==true) ? r+1 : r-1;

	if (r==15) { dr = false; }
       	else if (r==0) { dr = true; }

	g = (dg==true)? g+1 : g-1;

	if (g==15) { dg = false; }
       	else if (g==0) { dg = true; }

        setLed(0,0,0, b,b,b);
        setLed(0,0,1, r,g,b);
        setLed(0,0,2, r,g,b);
        setLed(0,0,3, r,g,b);
        setLed(0,1,0, b,b,b);
        setLed(0,1,1, r,r,r);
        setLed(0,1,2, r,r,r);
        setLed(0,1,3, r,r,r);
        setLed(0,2,0, b,b,b);
        setLed(0,2,1, g,g,g);
        setLed(0,2,2, g,g,g);
        setLed(0,2,3, g,g,g);
        setLed(0,3,0, b,b,b);
        setLed(0,3,1, r,g,g);
        setLed(0,3,2, r,g,g);
        setLed(0,3,3, r,g,g);

        if (delay_and_buttons(delay)) return;
    }
}

void walking_testing( uint16_t cycles, uint16_t delay) {
    uint16_t step = 0x007f;
    uint8_t l,x,y,r,g,b;

    while(cycles != 0) {
	cycles--;
	step++;

//      -DBG RLLL YYXX
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
	setLed(l,x,y,r,g,b);

//	if (x==0) printf("%X [%d,%d,%d] = (%d,%d,%d)\n",step, l,x,y,r,g,b);
        if (delay_and_buttons(delay)) return;
    }
}

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

void rgb_test( uint16_t cycles, uint16_t delay) {

   while(cycles != 0) {
      cycles--;
      allLedsColor( 15,0,0);
      if (delay_and_buttons(delay)) return;
      allLedsColor( 0,15,0);
      if (delay_and_buttons(delay)) return;
      allLedsColor( 0,0,15);
      if (delay_and_buttons(delay)) return;
   }
}

void setLed4RGBOnOff(uint8_t l, uint8_t x, uint16_t r, uint16_t g, uint16_t b, uint16_t mask) {
    uint8_t y;
    for (y=0;y<4;y++) {
       setLed(l,x,y, (r & mask)? 15:0, (g & mask)? 15:0, (b & mask)? 15:0);
       mask = mask >>1;
    }
}

uint8_t upDown(bool test, uint8_t value) {
   if ((test == true ) && (value < 15)) return( value+1);
   if ((test == false) && (value != 0)) return( value-1);
   return(value);
}

void setLed4RGBUpDown(uint8_t l, uint8_t x, uint16_t red, uint16_t green, uint16_t blue, uint16_t mask) {
    uint8_t y;
    uint8_t cRed,cGreen,cBlue;

    for (y=0;y<4;y++) {
       getLed(l,x,y,&cRed,&cGreen,&cBlue);
       setLed(l,x,y, upDown((red & mask)!=0,cRed), upDown((green & mask),cGreen), upDown((blue & mask),cBlue));
       mask = mask >>1;
    }
}

void runDiskPattern(char *name, uint16_t cycles, uint16_t delay) {
   uint16_t tBuffer[64*3];
   char filename[40];
   FILE *fh;
   uint8_t type;
   uint8_t speed;
   uint8_t loop;
   uint8_t fad_cycle;
   uint16_t frame = 0;
   bool once = true;

   while(cycles != 0) {
      cycles--;
      sprintf(filename, "/spiffs/%s",name);
      printf("file=%s\n", filename);
      fh = fopen(filename, "r");
      frame = 0;

      // read header
      fread(tBuffer,1, 24, fh);
      type = tBuffer[0];
      speed = tBuffer[1];
 
      if (once==true) {
	  printf("type=%d speed=%d\n",type,speed);
	  once = false;
      }

      while (!feof(fh)) {
          // read entry
          fread(tBuffer,2,(8*3), fh);
	  if (type == 2) {
              fread(&fad_cycle,1,1, fh);
              fad_cycle *= 2;
	  } else {
              fad_cycle = 0;
	  }
	  if (fad_cycle == 0) {
//              printf("read frame=%d cycles=%d fad_cycle =%d\n",frame,cycles,fad_cycle);
	      for (loop=0;loop<8;loop++) {
                  setLed4RGBOnOff(loop/4,   loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x8000);
                  setLed4RGBOnOff(loop/4+2, loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0008);
                  setLed4RGBOnOff(loop/4+4, loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0800);
                  setLed4RGBOnOff(loop/4+6, loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0080);
              }

              if (delay_and_buttons(delay*speed)) {
                  fclose(fh); 
		  return;
	      }

	  } else {
	      while (fad_cycle > 0) {
		  fad_cycle--;
//                  printf("read frame=%d cycles=%d fad_cycle =%d\n",frame,cycles,fad_cycle);
	          for (loop=0;loop<8;loop++) {
                      setLed4RGBUpDown(loop/4,   loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x8000);
                      setLed4RGBUpDown(loop/4+2, loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0008);
                      setLed4RGBUpDown(loop/4+4, loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0800);
                      setLed4RGBUpDown(loop/4+6, loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0080);
                  }
                  if (delay_and_buttons(delay*speed)) {
                      fclose(fh); 
		      return;
	          }
	      }
	  }
	  frame++;
      }
      fclose(fh); 
   }
}

pattern_entry_t patternTable[MAX_PATTERN_ENTRY] = {
   {.patternType = PATTERN_BUILT_IN,
    .runMe = fad_testing,
    .patternName = "Fad testing Level 1",
    .delay = 100,
    .cycles = 100,
    .enabled = true},
   {.patternType = PATTERN_BUILT_IN,
    .runMe = walking_testing,
    .patternName = "Walking LED test",
    .delay = 200,
    .cycles = 1000,
    .enabled = true},
   {.patternType = PATTERN_FILE,
    .delay = 100,
    .cycles = 10,
    .fileName = "test.pat",
    .patternName = "test.pat off disk",
    .enabled = true},
   {.patternType = PATTERN_FILE,
    .delay = 100,
    .cycles = 3,
    .fileName = "rainbow3.pat",
    .patternName = "rainbow3.pat off disk",
    .enabled = true},
   {.patternType = PATTERN_FILE,
    .delay = 100,
    .cycles = 10,
    .fileName = "test3.pat",
    .patternName = "test.pat off disk",
    .enabled = true},
   {.patternType = PATTERN_FILE,
    .delay = 100,
    .cycles = 10,
    .fileName = "rainbow.pat",
    .patternName = "rainbow - off disk",
    .enabled = true},
   {.patternType = PATTERN_BUILT_IN,
    .runMe = rgb_fade,
    .patternName = "RGB all Fade test",
    .delay = 100,
    .cycles = 10,
    .enabled = true},
   {.patternType = PATTERN_BUILT_IN,
    .runMe = rgb_test,
    .patternName = "Just a RGB test",
    .delay = 1000 * 3,
    .cycles = 10,
    .enabled = true},
  };

void addPattern( char * filename) {
    uint8_t index;

    for (index = 0; index < MAX_PATTERN_ENTRY; index++) {
        if ((patternTable[index].patternType == PATTERN_FILE) &&
            (strcmp(patternTable[index].fileName, filename)==0)) {
	    return; // found it, don't need to save it
	}
    }
    for (index = 0; patternTable[index].patternType != PATTERN_NONE && index < MAX_PATTERN_ENTRY; index++) ;

    if (index == MAX_PATTERN_ENTRY) {
	return; // table is full
    }

    strcpy(patternTable[index].fileName, filename);
    patternTable[index].delay = 100;
    patternTable[index].cycles = 10;
    patternTable[index].enabled = true;
    strcpy(patternTable[index].patternName, filename);
    patternTable[index].patternType = PATTERN_FILE;
}


void updatePatternsTask(void *param) {
    uint8_t step = 0;

    allLedsOff();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    allLedsOff();

    while (1) {
	printf("running pattern %d %s\n", step, patternTable[step].patternName);
        switch (patternTable[step].patternType) {
            case PATTERN_BUILT_IN:
                (patternTable[step].runMe)(patternTable[step].cycles, patternTable[step].delay);
                break;
            case PATTERN_FILE:
                runDiskPattern(patternTable[step].fileName, patternTable[step].cycles, patternTable[step].delay);
                break;
            case PATTERN_NONE:
            default:
                break;
        }
        step = ((step+1) % MAX_PATTERN_ENTRY);
    }
}

