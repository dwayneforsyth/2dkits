


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_system.h"
#include "esp_http_server.h"
#include "esp_wifi.h"

#include "led_driver.h"
#include "web_server.h"
#include "disk_system.h"

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
    char name[20];
    bool enabled;
} pattern_entry_t;
 

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

        vTaskDelay(delay / portTICK_PERIOD_MS);
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
        vTaskDelay(delay / portTICK_PERIOD_MS);
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
             vTaskDelay(delay / portTICK_PERIOD_MS);
	  }
	  for (fad= 14; fad != 0; fad--) {
             r = (color & 0x01)? 0 : fad;
             g = (color & 0x02)? 0 : fad;
             b = (color & 0x04)? 0 : fad;
             allLedsColor(r,g,b);
             vTaskDelay(delay / portTICK_PERIOD_MS);
	  }
      }
   }
}

void rgb_test( uint16_t cycles, uint16_t delay) {

   while(cycles != 0) {
      cycles--;
      allLedsColor( 15,0,0);
      vTaskDelay(delay / portTICK_PERIOD_MS);
      allLedsColor( 0,15,0);
      vTaskDelay(delay / portTICK_PERIOD_MS);
      allLedsColor( 0,0,15);
      vTaskDelay(delay / portTICK_PERIOD_MS);
   }
}

void runDiskPattern(char *name, uint16_t cycles, uint16_t delay) {
   char tBuffer[128*3+1];
   FILE *fh;
   uint8_t type;
   uint8_t speed;
   uint8_t l,x,y;

   while(cycles != 0) {
      cycles--;
      sprintf(tBuffer, "/spiffs/%s",name);
      fh = fopen(tBuffer, "R");

      // read header
      fgets(tBuffer, 24, fh);
      type = tBuffer[0];
      speed = tBuffer[1];

      while (!feof(fh)) {
          // read entry
          fgets(tBuffer, 128*3, fh);
          for (l=0;l<8;l++) {
             for (x=0;x<4;x++) {
                for (y=0;y<4;y++) {
                   setLed(l,x,y, tBuffer[(l*64+x*4+y)*3], tBuffer[(l*64+x*4+y)*3+1], tBuffer[(l*64+x*4+y)*3+2]);
          }  }  }
          vTaskDelay(delay / portTICK_PERIOD_MS);
      }
      fclose(fh); 
   }
}

pattern_entry_t patternTable[50] = {
   {.patternType = PATTERN_BUILT_IN,
    .runMe = fad_testing,
    .delay = 100,
    .cycles = 100,
    .enabled = true},
   {.patternType = PATTERN_BUILT_IN,
    .runMe = walking_testing,
    .delay = 200,
    .cycles = 100,
    .enabled = true},
   {.patternType = PATTERN_BUILT_IN,
    .runMe = rgb_fade,
    .delay = 100,
    .cycles = 100,
    .enabled = true},
   {.patternType = PATTERN_BUILT_IN,
    .runMe = rgb_test,
    .delay = 1000 * 3,
    .cycles = 100,
    .enabled = true},
   {.patternType = PATTERN_FILE,
    .delay = 100,
    .cycles = 100,
    .name = "test.pat",
    .enabled = true},
  };


void updatePatternsTask(void *param) {
    uint8_t step = 0;

    allLedsOff();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    allLedsOff();

    while (1) {
        switch (patternTable[step].patternType) {
            case PATTERN_BUILT_IN:
                (patternTable[step].runMe)(patternTable[step].cycles, patternTable[step].delay);
                break;
            case PATTERN_FILE:
                runDiskPattern(patternTable[step].name, patternTable[step].cycles, patternTable[step].delay);
                break;
            case PATTERN_NONE:
            default:
                break;
        }
        step = ( step+1 % 3);
    }
}

