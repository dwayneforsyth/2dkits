


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
    char fileName[20];
    char patternName[20];
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

void setLed4RGBOnOff(uint8_t l, uint8_t x, uint16_t r, uint16_t g, uint16_t b, uint16_t mask) {
    uint8_t y;
    for (y=0;y<4;y++) {
       setLed(l,x,y, (r & mask)? 15:0, (g & mask)? 15:0, (b & mask)? 15:0);
       mask = mask >>1;
    }
}

void setLed4RGBUpDown(uint8_t l, uint8_t x, uint16_t r, uint16_t g, uint16_t b, uint16_t mask) {
    uint8_t y;
//    int8_t cr,cg,cb;

    for (y=0;y<4;y++) {
#if (0)
       getLed(l,x,y,&cr,&cg,&cb);
       setLed(l,x,y, (r & mask)? MIN(15,cr++) : MAX(0,cr--),
                     (g & mask)? MIN(15,cg++) : MAX(0,cg--),
		     (b & mask)? MIN(15,cg++) : MIN(0,cg--));
#endif
       setLed(l,x,y, (r & mask)? 15:0, (g & mask)? 15:0, (b & mask)? 15:0);
       mask = mask >>1;
    }
}

void runDiskPattern(char *name, uint16_t cycles, uint16_t delay) {
   uint16_t tBuffer[64*3];
   char filename[40];
   FILE *fh;
   uint8_t type;
   uint8_t speed;
   uint8_t l,x,y;
   uint8_t fad_cycle;
   uint16_t frame = 0;

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
      printf("type=%d speed=%d\n",type,speed);

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
              printf("read frame=%d cycles=%d fad_cycle =%d\n",frame,cycles,fad_cycle);
              setLed4RGBOnOff(0, 0, tBuffer[ 0], tBuffer[ 1], tBuffer[ 2], 0x8000);
              setLed4RGBOnOff(0, 1, tBuffer[ 3], tBuffer[ 4], tBuffer[ 5], 0x8000);
              setLed4RGBOnOff(0, 2, tBuffer[ 6], tBuffer[ 7], tBuffer[ 8], 0x8000);
              setLed4RGBOnOff(0, 3, tBuffer[ 9], tBuffer[10], tBuffer[11], 0x8000);

              setLed4RGBOnOff(1, 0, tBuffer[12], tBuffer[13], tBuffer[14], 0x8000);
              setLed4RGBOnOff(1, 1, tBuffer[15], tBuffer[16], tBuffer[17], 0x8000);
              setLed4RGBOnOff(1, 2, tBuffer[18], tBuffer[19], tBuffer[20], 0x8000);
              setLed4RGBOnOff(1, 3, tBuffer[21], tBuffer[22], tBuffer[23], 0x8000);

              setLed4RGBOnOff(2, 0, tBuffer[ 0], tBuffer[ 1], tBuffer[ 2], 0x0008);
              setLed4RGBOnOff(2, 1, tBuffer[ 3], tBuffer[ 4], tBuffer[ 5], 0x0008);
              setLed4RGBOnOff(2, 2, tBuffer[ 6], tBuffer[ 7], tBuffer[ 8], 0x0008);
              setLed4RGBOnOff(2, 3, tBuffer[ 9], tBuffer[10], tBuffer[11], 0x0008);

              setLed4RGBOnOff(3, 0, tBuffer[12], tBuffer[13], tBuffer[14], 0x0008);
              setLed4RGBOnOff(3, 1, tBuffer[15], tBuffer[16], tBuffer[17], 0x0008);
              setLed4RGBOnOff(3, 2, tBuffer[18], tBuffer[19], tBuffer[20], 0x0008);
              setLed4RGBOnOff(3, 3, tBuffer[21], tBuffer[22], tBuffer[23], 0x0008);

              setLed4RGBOnOff(4, 0, tBuffer[ 0], tBuffer[ 1], tBuffer[ 2], 0x0800);
              setLed4RGBOnOff(4, 1, tBuffer[ 3], tBuffer[ 4], tBuffer[ 5], 0x0800);
              setLed4RGBOnOff(4, 2, tBuffer[ 6], tBuffer[ 7], tBuffer[ 8], 0x0800);
              setLed4RGBOnOff(4, 3, tBuffer[ 9], tBuffer[10], tBuffer[11], 0x0800);

              setLed4RGBOnOff(5, 0, tBuffer[12], tBuffer[13], tBuffer[14], 0x0800);
              setLed4RGBOnOff(5, 1, tBuffer[15], tBuffer[16], tBuffer[17], 0x0800);
              setLed4RGBOnOff(5, 2, tBuffer[18], tBuffer[19], tBuffer[20], 0x0800);
              setLed4RGBOnOff(5, 3, tBuffer[21], tBuffer[22], tBuffer[23], 0x0800);

              setLed4RGBOnOff(6, 0, tBuffer[ 0], tBuffer[ 1], tBuffer[ 2], 0x0080);
              setLed4RGBOnOff(6, 1, tBuffer[ 3], tBuffer[ 4], tBuffer[ 5], 0x0080);
              setLed4RGBOnOff(6, 2, tBuffer[ 6], tBuffer[ 7], tBuffer[ 8], 0x0080);
              setLed4RGBOnOff(6, 3, tBuffer[ 9], tBuffer[10], tBuffer[11], 0x0080);

              setLed4RGBOnOff(7, 0, tBuffer[12], tBuffer[13], tBuffer[14], 0x0080);
              setLed4RGBOnOff(7, 1, tBuffer[15], tBuffer[16], tBuffer[17], 0x0080);
              setLed4RGBOnOff(7, 2, tBuffer[18], tBuffer[19], tBuffer[20], 0x0080);
              setLed4RGBOnOff(7, 3, tBuffer[21], tBuffer[22], tBuffer[23], 0x0080);

              vTaskDelay(delay * speed / portTICK_PERIOD_MS);
	  } else {
	      while (fad_cycle > 0) {
		  fad_cycle--;
                  printf("read frame=%d cycles=%d fad_cycle =%d\n",frame,cycles,fad_cycle);
                  setLed4RGBUpDown(0, 0, tBuffer[ 0], tBuffer[ 1], tBuffer[ 2], 0x8000);
                  setLed4RGBUpDown(0, 1, tBuffer[ 3], tBuffer[ 4], tBuffer[ 5], 0x8000);
                  setLed4RGBUpDown(0, 2, tBuffer[ 6], tBuffer[ 7], tBuffer[ 8], 0x8000);
                  setLed4RGBUpDown(0, 3, tBuffer[ 9], tBuffer[10], tBuffer[11], 0x8000);
    
                  setLed4RGBUpDown(1, 0, tBuffer[12], tBuffer[13], tBuffer[14], 0x8000);
                  setLed4RGBUpDown(1, 1, tBuffer[15], tBuffer[16], tBuffer[17], 0x8000);
                  setLed4RGBUpDown(1, 2, tBuffer[18], tBuffer[19], tBuffer[20], 0x8000);
                  setLed4RGBUpDown(1, 3, tBuffer[21], tBuffer[22], tBuffer[23], 0x8000);
    
                  setLed4RGBUpDown(2, 0, tBuffer[ 0], tBuffer[ 1], tBuffer[ 2], 0x0008);
                  setLed4RGBUpDown(2, 1, tBuffer[ 3], tBuffer[ 4], tBuffer[ 5], 0x0008);
                  setLed4RGBUpDown(2, 2, tBuffer[ 6], tBuffer[ 7], tBuffer[ 8], 0x0008);
                  setLed4RGBUpDown(2, 3, tBuffer[ 9], tBuffer[10], tBuffer[11], 0x0008);
    
                  setLed4RGBUpDown(3, 0, tBuffer[12], tBuffer[13], tBuffer[14], 0x0008);
                  setLed4RGBUpDown(3, 1, tBuffer[15], tBuffer[16], tBuffer[17], 0x0008);
                  setLed4RGBUpDown(3, 2, tBuffer[18], tBuffer[19], tBuffer[20], 0x0008);
                  setLed4RGBUpDown(3, 3, tBuffer[21], tBuffer[22], tBuffer[23], 0x0008);
    
                  setLed4RGBUpDown(4, 0, tBuffer[ 0], tBuffer[ 1], tBuffer[ 2], 0x0800);
                  setLed4RGBUpDown(4, 1, tBuffer[ 3], tBuffer[ 4], tBuffer[ 5], 0x0800);
                  setLed4RGBUpDown(4, 2, tBuffer[ 6], tBuffer[ 7], tBuffer[ 8], 0x0800);
                  setLed4RGBUpDown(4, 3, tBuffer[ 9], tBuffer[10], tBuffer[11], 0x0800);
    
                  setLed4RGBUpDown(5, 0, tBuffer[12], tBuffer[13], tBuffer[14], 0x0800);
                  setLed4RGBUpDown(5, 1, tBuffer[15], tBuffer[16], tBuffer[17], 0x0800);
                  setLed4RGBUpDown(5, 2, tBuffer[18], tBuffer[19], tBuffer[20], 0x0800);
                  setLed4RGBUpDown(5, 3, tBuffer[21], tBuffer[22], tBuffer[23], 0x0800);
    
                  setLed4RGBUpDown(6, 0, tBuffer[ 0], tBuffer[ 1], tBuffer[ 2], 0x0080);
                  setLed4RGBUpDown(6, 1, tBuffer[ 3], tBuffer[ 4], tBuffer[ 5], 0x0080);
                  setLed4RGBUpDown(6, 2, tBuffer[ 6], tBuffer[ 7], tBuffer[ 8], 0x0080);
                  setLed4RGBUpDown(6, 3, tBuffer[ 9], tBuffer[10], tBuffer[11], 0x0080);
    
                  setLed4RGBUpDown(7, 0, tBuffer[12], tBuffer[13], tBuffer[14], 0x0080);
                  setLed4RGBUpDown(7, 1, tBuffer[15], tBuffer[16], tBuffer[17], 0x0080);
                  setLed4RGBUpDown(7, 2, tBuffer[18], tBuffer[19], tBuffer[20], 0x0080);
                  setLed4RGBUpDown(7, 3, tBuffer[21], tBuffer[22], tBuffer[23], 0x0080);

                  vTaskDelay(delay * speed / portTICK_PERIOD_MS);
	      }
	  }
#if (0)
          for (l=0;l<8;l++) {
             for (x=0;x<4;x++) {
                for (y=0;y<4;y++) {
                   setLed(l,x,y, tBuffer[(l*64+x*4+y)*3], tBuffer[(l*64+x*4+y)*3+1], tBuffer[(l*64+x*4+y)*3+2]);
          }  }  }
#endif
	  frame++;
      }
      fclose(fh); 
   }
}

pattern_entry_t patternTable[50] = {
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
    .cycles = 100,
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
        step = ( step+1 % 8);
    }
}

