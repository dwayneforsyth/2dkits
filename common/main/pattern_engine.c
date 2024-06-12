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
#include <esp_log.h>

#include "led_driver.h"
#include "web_server.h"
#include "disk_system.h"
#include "numbers_led.h"
#include "global.h"
#include "download_file.h"
#include "sha_file.h"
#include "picoc.h"
#include "version.h"
#include "board_pins.h"
#include "font.h"

#define MAX_PATTERN_ENTRY 50

typedef enum patternType_t {
    PATTERN_NONE,
    PATTERN_BUILT_IN,
    PATTERN_FILE,
    PATTERN_SCRIPT_FILE
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

static const char *TAG="PENG";

uint8_t pendingExit = false;

// this is a kludge. the pattern timeout should be in
// delay_and_buttons() but it is current in the pattern
// engine for historical reasons.
uint8_t exitReason = false;

bool patternRun = true;

void setPatternRun( bool onOff ) {
    printf("SetPatternRun %d -> %d\n", patternRun, onOff);
    patternRun = onOff;
}

uint8_t step = 0;


bool printPattern = false;

void layer_test( uint16_t cycles, uint16_t delay);
void rgb_test( uint16_t cycles, uint16_t delay);
void walking_testing( uint16_t cycles, uint16_t delay);
void rgb_fade( uint16_t cycles, uint16_t delay);
/*******************************************************************************

    Pattern table

*******************************************************************************/
pattern_entry_t patternTable[MAX_PATTERN_ENTRY] = {
   {.patternType = PATTERN_BUILT_IN,
    .runMe = layer_test,
    .patternName = "Layer test",
    .delay = 500,
    .cycles = 10,
    .enabled = true},
   {.patternType = PATTERN_BUILT_IN,
    .runMe = rgb_test,
    .patternName = "Just a RGB test",
    .delay = 1000 * 3,
    .cycles = 10,
    .enabled = true},
   {.patternType = PATTERN_BUILT_IN,
    .runMe = walking_testing,
    .patternName = "Walking LED test",
    .delay = 200,
    .cycles = 1000,
    .enabled = true},
   {.patternType = PATTERN_BUILT_IN,
    .runMe = rgb_fade,
    .patternName = "RGB all Fade test",
    .delay = 100,
    .cycles = 10,
    .enabled = true},
  };

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void setPrintPattern( bool onOff ) {
    printPattern = onOff;
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
bool getPrintPattern( void ) {
    return(printPattern);
}

/*******************************************************************************
    PURPOSE: returns the last pattern number

    INPUTS: none

    RETURN CODE: last pattern index

    NOTES: zero based number!

*******************************************************************************/
uint8_t getLastPattern() {
    uint8_t index;
    for (index = 0; index < MAX_PATTERN_ENTRY; index++) {
        if (patternTable[index].patternType == PATTERN_NONE) {
            printf("last_pattern = %d\n", index);
            return( index-1);
        }
    }
    return(MAX_PATTERN_ENTRY-1);
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void setPatternPlus() {
    step = (step+1) % (getLastPattern()+1);
    pendingExit = true;
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void setPatternMinus() {
    step = (step == 0)? getLastPattern() : step -1;
    pendingExit = true;
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
bool demoMode = true;
bool getDemoMode() {
    return(demoMode);
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void setDemoMode(bool onOff) {
    demoMode = onOff;
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void patternEngineOff(void) {
    changeBank(1);
    allLedsOff();
}


/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
bool delay_and_buttons(uint16_t delay) {
    bool exit = pendingExit;
    uint16_t delayCount = 0;

    vTaskDelay(delay / portTICK_PERIOD_MS);

    while (patternRun == false) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    if (gpio_get_level(BUTTON1) == 0) {
        printf("Button 1\n");
	changeBank( 1 );
	displayNumber(step);
	while (gpio_get_level(BUTTON1) == 0) {
	     vTaskDelay(1);
	     delayCount++;
	     if (delayCount > 500) {
		     delayCount = 0;
                     setPatternPlus();
		     displayNumber(step);
		     exit = true;
	     }
	}
	changeBank( 0 );
        return(exit);
    }

    if (gpio_get_level(BUTTON2) == 0) {
        printf("Button 2\n");
	changeBank( 1 );
	displayNumber(step);
	while (gpio_get_level(BUTTON2) == 0) {
	     vTaskDelay(1);
	     delayCount++;
	     if (delayCount > 500) {
		     delayCount = 0;
                     setPatternMinus();
		     displayNumber(step);
		     exit = true;
	     }
	}
	changeBank( 0 );
        return(exit);
    }

    if (pendingExit==true) {
        pendingExit = false;
        exit = true;
    }

    exitReason = exit;
    return(exit);
}

/*
   This code drive the 2DKits.com 4x4x8 tower
*/


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
void setLed4RGBOnOff(uint8_t l, uint8_t x, uint16_t r, uint16_t g, uint16_t b, uint16_t mask) {
    uint8_t y;
    for (y=0;y<4;y++) {
       setLed(l,x,y, (r & mask)? 15:0, (g & mask)? 15:0, (b & mask)? 15:0);
       mask = mask >>1;
    }
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
uint8_t upDown(bool test, uint8_t value) {
   if ((test == true ) && (value < 15)) return( value+1);
   if ((test == false) && (value != 0)) return( value-1);
   return(value);
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void setLed4RGBUpDown(uint8_t l, uint8_t x, uint16_t red, uint16_t green, uint16_t blue, uint16_t mask) {
    uint8_t y;
    uint8_t cRed,cGreen,cBlue;

    for (y=0;y<4;y++) {
       getLed(l,x,y,&cRed,&cGreen,&cBlue);
       setLed(l,x,y, upDown((red & mask)!=0,cRed), upDown((green & mask),cGreen), upDown((blue & mask),cBlue));
       mask = mask >>1;
    }
}

uint8_t bit( uint8_t data, uint8_t bit ) {
	return( (data & (1<<bit))? 0x0f: 0x00);
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void addAndShift( uint8_t red, uint8_t green, uint8_t blue) {
   uint8_t cRed = 0;
   uint8_t cGreen = 0;
   uint8_t cBlue = 0;

   for (uint8_t i=1;i<=3;i++) { // 1,2,3
      for (uint8_t j=0;j<4;j++) { // 0,1,2,3
         getLed(0,j,i,   &cRed,&cGreen,&cBlue);
         setLed(0,j,i-1, cRed, cGreen, cBlue);
         getLed(1,j,i,   &cRed,&cGreen,&cBlue);
         setLed(1,j,i-1, cRed, cGreen, cBlue);
      }
   }

   for (uint8_t j=0;j<4;j++) { // 0,1,2,3
      getLed(4,j,0, &cRed,&cGreen,&cBlue);
      setLed(0,j,3, cRed, cGreen, cBlue);
      getLed(5,j,0, &cRed,&cGreen,&cBlue);
      setLed(1,j,3, cRed, cGreen, cBlue);
   }

   for (uint8_t i=1;i<=3;i++) { // 1,2,3
      for (uint8_t j=0;j<4;j++) { // 0,1,2,3
         getLed(4,j,i,   &cRed,&cGreen,&cBlue);
         setLed(4,j,i-1, cRed, cGreen, cBlue);
         getLed(5,j,i,   &cRed,&cGreen,&cBlue);
         setLed(5,j,i-1, cRed, cGreen, cBlue);
      }
   }

   for (uint8_t j=0;j<4;j++) { // 0,1,2,3
      getLed(6,j,0, &cRed,&cGreen,&cBlue);
      setLed(4,j,3, cRed, cGreen, cBlue);
      getLed(7,j,0, &cRed,&cGreen,&cBlue);
      setLed(5,j,3, cRed, cGreen, cBlue);
   }

   for (uint8_t i=1;i<=3;i++) { // 1,2,3
      for (uint8_t j=0;j<4;j++) { // 0,1,2,3
         getLed(6,j,i,   &cRed,&cGreen,&cBlue);
         setLed(6,j,i-1, cRed, cGreen, cBlue);
         getLed(7,j,i,   &cRed,&cGreen,&cBlue);
         setLed(7,j,i-1, cRed, cGreen, cBlue);
      }
   }

   for (uint8_t j=0;j<4;j++) { // 0,1,2,3
      getLed(2,j,0, &cRed,&cGreen,&cBlue);
      setLed(6,j,3, cRed, cGreen, cBlue);
      getLed(3,j,0, &cRed,&cGreen,&cBlue);
      setLed(7,j,3, cRed, cGreen, cBlue);
   }

   for (uint8_t i=1;i<=3;i++) { // 1,2,3
      for (uint8_t j=0;j<4;j++) { // 0,1,2,3
         getLed(2,j,i,   &cRed,&cGreen,&cBlue);
         setLed(2,j,i-1, cRed, cGreen, cBlue);
         getLed(3,j,i,   &cRed,&cGreen,&cBlue);
         setLed(3,j,i-1, cRed, cGreen, cBlue);
      }
   }

//  set new row
    for (uint8_t j=0;j<4;j++) { // 0,1,2,3
       setLed(2,j,3, bit(red,7-j),bit(green,7-j),bit(blue,7-j));
       setLed(3,j,3, bit(red,3-j),bit(green,3-j),bit(blue,3-j));
    }
}
/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void runDiskPattern(char *name, uint16_t cycles, uint16_t delay) {
   uint16_t tBuffer[256];
   char filename[40];
   FILE *fh;
   uint8_t type;
   uint8_t speed;
   uint8_t loop;
   uint8_t fad_cycle;
   uint16_t frame = 0;
   bool once = true;
   int ret;
   uint8_t charIn = 'A';
   uint8_t step = 0;
   uint8_t colorCode = 0x07;

   allLedsColor( 0,0,0);

   while(cycles != 0) {
      cycles--;
      sprintf(filename, "/spiffs/%s",name);
      if (once==true) ESP_LOGI(TAG,"file=%s", filename);
      fh = fopen(filename, "r");
      frame = 0;
//      printf("================================================\n");

      // read header
      fread(tBuffer,1, 24, fh);
      type = tBuffer[0];
      speed = tBuffer[1];
 
      if (once==true) {
	  ESP_LOGI(TAG,"type=%d speed=%d delay=%d math=%d",type,speed,delay,delay*speed);
	  once = false;
      }

      while (!feof(fh)) {
          // read entry
          switch (type) {
          case 2: // old tower
              fread(tBuffer,2,(8*3), fh);
              fread(&fad_cycle,1,1, fh);
//              printf("read frame=%d cycles=%d fad=%d\n",frame,cycles, fad_cycle);
	      if (fad_cycle == 0) {
	          for (loop=0;loop<8;loop++) {
                      setLed4RGBOnOff(7- (7-loop)/4,    loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x8000);
                      setLed4RGBOnOff(7-((7-loop)/4+2), loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0008);
                      setLed4RGBOnOff(7-((7-loop)/4+4), loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0800);
                      setLed4RGBOnOff(7-((7-loop)/4+6), loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0080);
                  }
                  if (delay_and_buttons(delay*speed)) {
                      fclose(fh); 
		      return;
	          }
	      } else {
                  fad_cycle = fad_cycle * 2;
	          while (fad_cycle > 0) {
    		      fad_cycle--;
    	              for (loop=0;loop<8;loop++) {
                          setLed4RGBUpDown(7- (7-loop)/4,    loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x8000);
                          setLed4RGBUpDown(7-((7-loop)/4+2), loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0008);
                          setLed4RGBUpDown(7-((7-loop)/4+4), loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0800);
                          setLed4RGBUpDown(7-((7-loop)/4+6), loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0080);
                      }
                      if (delay_and_buttons(delay*speed)) {
                          fclose(fh); 
		          return;
	              }
	          }
	      }
              break;
	  case 16: // old tower
              ret = fread(tBuffer,2,(8*3), fh);
              if (ret == 0) { break;}
//              printf("read frame=%d cycles=%d\n",frame,cycles);
	      for (loop=0;loop<8;loop++) {
                  setLed4RGBOnOff(7- (7-loop)/4,    loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x8000);
                  setLed4RGBOnOff(7-((7-loop)/4+2), loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0008);
                  setLed4RGBOnOff(7-((7-loop)/4+4), loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0800);
                  setLed4RGBOnOff(7-((7-loop)/4+6), loop%4, tBuffer[loop*3], tBuffer[loop*3+1], tBuffer[loop*3+2], 0x0080);
              }

              if (delay_and_buttons(delay*speed)) {
                  fclose(fh); 
		  return;
	      }
	      break;
	  case 17: // old pattern format?
              printf("old pattern format?\n");
              setPatternPlus();
	      return;
	      break;
	  case 18: // old pattern format?
	      if (step == 0) {
	         ret = fread(&charIn,1,1, fh);
		 if (ret == 0) { break;}
		 while ((charIn < 20) && (ret != 0)) {
                     colorCode = charIn;
	             ret = fread(&charIn,1,1, fh);
		     if (ret == 0) { break;}
		 }
		 charIn -= 0x20;
		 addAndShift( 0x00, 0x00, 0x00 );
		 step = 1;
	      } else {

                 addAndShift( (colorCode & 0x01)? charData[charIn].ch[step-1] : 0,
                              (colorCode & 0x02)? charData[charIn].ch[step-1] : 0,
                              (colorCode & 0x04)? charData[charIn].ch[step-1] : 0);
		 if (step >= charData[charIn].sl) { step = 0;}
		 else { step++;}
	      }

              if (delay_and_buttons(delay*speed)) {
                  fclose(fh); 
		  return;
	      }
/*
 * 0000000 00062012 00000000 00000000 00000000
 * 0000020 00000000 00000000 68540101 69207369
 * 0000040 61022073 73657420 6f032074 68742066
 * 0000060 42042065 6b6e696c 05206569 74786574
 * 0000100 79730620 6d657473 7707202e 322e7777
 * 0000120 74694b44 6f632e73 2020206d 20202020
 * 0000140 20202020 20202020 20202020 20202020
 * 0000160 00000020
 * 0000161
 */

	      break;
          case 32: { // tower
//	      const char *LEDValue[19] = { "0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","+","-","#" };
              uint8_t red, green, blue;
	      uint16_t temp;
              uint8_t loops,tLoops[2];
              int8_t l,x,y;
              fread(tBuffer,2,(8*4*4), fh);
              fread(tLoops,1,1, fh);
              fread(&delay,1,1, fh);
//	      printf("\nframe %d\n",frame);
              for (loops=0;loops<tLoops[0];loops++) {
                  for (l=7;l>=0;l--) {
                      for (x=0;x<4;x++) {
                          for (y=0;y<4;y++) {
                              temp = tBuffer[(l*16+x*4+y)];
                              blue =   temp & 0x1f;
                              green = (temp >>5) & 0x1f;
                              red =   (temp >>10) & 0x1f;
                              setLed(l,x,y, red,green,blue);
//			      printf("%4X ", temp);
//			      printf(" (%2s,%2s,%2s)",LEDValue[red],LEDValue[green],LEDValue[blue]);
			  }
//		          printf("\n");
                      }
	       	  }
//		  printf("cycles=%d delay=%d\n",loops,delay*speed);
                  if (delay_and_buttons(delay*speed)) {
                      fclose(fh); 
		      return;
	          }
              }
              break;
          }
#if (NUM_LAYER == 4)
          case 33: { // cube
              uint8_t red, green, blue;
              uint16_t temp;
              uint8_t loops,tLoops[2];
              int8_t l,x,y;
              fread(tBuffer,2,(4*4*4), fh);
              fread(tLoops,1,1, fh);
              fread(&delay,1,1, fh);
//	      printf("\nframe %d\n",frame);
              for (loops=0;loops<tLoops[0];loops++) {
                  for (l=3;l>=0;l--) {
                      for (x=0;x<4;x++) {
                          for (y=0;y<4;y++) {
                              temp = tBuffer[(l*16+x*4+y)];
                              blue =   temp & 0x1f;
                              green = (temp >>5) & 0x1f;
                              red =   (temp >>10) & 0x1f;
                              setLed(l,x,y, red,green,blue);
//			      printf("%4X ", temp);
//			      printf(" (%2s,%2s,%2s)",LEDValue[red],LEDValue[green],LEDValue[blue]);
                          }
//		          printf("\n");
                      }
                  }
 		  //printf("cycles=%d delay=%d\n",loops,delay*speed);
                  if (delay_and_buttons(delay*speed)) {
                      fclose(fh);
		      return;
		  }
	      }
	      break;
          }
#endif
          default:
              printf("unknown pattern type=%d file=%s\n",type, filename);
              setPatternPlus();
	      return;
              break;
	  }
	  frame++;
      }
      fclose(fh); 
   }
}

#define PICOC_STACK_SIZE (1024*40)
void runDiskScriptPattern(char *name) {
   char *filename;
   Picoc pc;

   PicocInitialize(&pc, PICOC_STACK_SIZE);
   asprintf(&filename, "/spiffs/%s",name);
   PicocPlatformScanFile(&pc, filename);
   free(filename);
   PicocCallMain(&pc, 0, NULL);
   PicocCleanup(&pc);
}


/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void addPattern( char * filename) {
    uint8_t index;

    for (index = 0; index < MAX_PATTERN_ENTRY; index++) {
        if (((patternTable[index].patternType == PATTERN_FILE)||(patternTable[index].patternType == PATTERN_SCRIPT_FILE)) &&
            (strcmp(patternTable[index].fileName, filename)==0)) {
	    return; // found it, don't need to save it
	}
    }
    for (index = 0; patternTable[index].patternType != PATTERN_NONE && index < MAX_PATTERN_ENTRY; index++) ;

    if (index == MAX_PATTERN_ENTRY) {
	return; // table is full
    }

    strcpy(patternTable[index].fileName, filename);
    patternTable[index].delay = 63;
    patternTable[index].cycles = 10;
    patternTable[index].enabled = true;
    strcpy(patternTable[index].patternName, filename);
    patternTable[index].patternType = (filename[strlen(filename)-1] == 'c')? PATTERN_SCRIPT_FILE : PATTERN_FILE;
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void deletePattern( char * filename) {
    uint8_t index;

    for (index = 0; index < MAX_PATTERN_ENTRY; index++) {
        if (((patternTable[index].patternType == PATTERN_FILE)||(patternTable[index].patternType == PATTERN_SCRIPT_FILE)) &&
            (strcmp(patternTable[index].fileName, filename)==0)) {
	    patternTable[index].patternType = PATTERN_NONE;
	    return;
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
uint8_t getPatternNumber() {
	return(step);
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void setPatternNumber(uint8_t newStep) {
    if (step != newStep) {
	step = (newStep % MAX_PATTERN_ENTRY);
	pendingExit = true;
    }
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
char * getPatternName() {
        static  char * none = "[no name]";

        char * tmp = patternTable[step].patternName;
        if (tmp[0] == 0) return(none);
	return(tmp);
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void parsePatternFileStr( char * line,  char * name, char * sha, char * url) {

	uint8_t i = 0;
	uint8_t length = strlen(line);
	while ((line[i] != ' ') && (i < length)) { i++;}

	memcpy(name, line, i);
	name[i] = 0;
	while ((line[i] == ' ') && (i < length)) { i++;} // dump 1 or more spaces
	uint8_t offset = i;

	while ((line[i] != ' ') && (i < length)) { i++;}
	memcpy(sha, &line[offset], i - offset);
	sha[i-offset] = 0;

	while ((line[i] == ' ') && (i < length)) { i++;} // dump 1 or more spaces
	memcpy(url, &line[i], length-i); 
	url[length-i-1] = 0; // dump the \n at the end
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
esp_err_t cloud_pattern_list(httpd_req_t *req)  {
    const char *pattern_header = "<table><tr><th>File<th>Sha1 Hash<th>Action<th>&nbsp\n";
    const char *pattern_footer = "</table></body></html>\n";
    const char *pattern_heading = "</div></td> <td valign=\"top\"><div id=\"navBreadCrumb\">Cloud Pattern Library</div><div class=\"centerColumn\" id=\"indexDefault\"><h1 id=\"indexDefaultHeading\"></h1>\n";
    const char *pattern_noweb = "Not connected to web!";

    char tbuffer2[314];

    parseUrl(req);

    httpd_resp_set_hdr(req, "Content-type", "text/html");
    file_get_handler(req, "/spiffs/header.html",true);

    httpd_resp_send_chunk(req, pattern_heading, strlen(pattern_heading));

    // no internet report and die
    if (xAppData.ipName == NULL) {
        httpd_resp_send_chunk(req, pattern_noweb, strlen(pattern_noweb));
        return ESP_OK;
    }

    // sending the MAC so we can see who is using the server
    sprintf( tbuffer2, "%s?id=%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X",CONFIG_CLOUD_PATTERN_URL,
                xAppData.apMac[0], xAppData.apMac[1], xAppData.apMac[2],
                xAppData.apMac[3], xAppData.apMac[4], xAppData.apMac[5]);
    download_file( "/spiffs/cloud.lst", tbuffer2);

    httpd_resp_send_chunk(req, pattern_header, strlen(pattern_header));
    FILE *ptr = fopen("/spiffs/cloud.lst","rb");
    char line[130];
    char name[40];
    char sha[41];
    char url[80];
    char *tpath;
    char diskSha[41];
    char *button;
    while ( fgets( line, 160, ptr ) != NULL ) { 
            parsePatternFileStr( line, name, sha, url);
	    asprintf(&tpath,"/spiffs/%s",name);
	    fileSha(tpath, diskSha);
	    free(tpath);
	    if (strcmp(sha,diskSha) ==0) {
		    asprintf(&button,"loaded");
	    } else if (strlen(diskSha) == 0) {
		    asprintf(&button,"<button onclick=\"window.location.href = '/cloudpatterns.html?download=%s';\">Download</button>",url);
	    } else {
		    asprintf(&button,"<button onclick=\"window.location.href = '/cloudpatterns.html?download=%s';\">Update</button>",url);
	    }
            snprintf(tbuffer2, sizeof(tbuffer2), "<tr><td>%s<td align=\"left\">%s<td>%s\n",
	       name,
               sha,
               button
            );
	    free(button);
            httpd_resp_send_chunk(req, tbuffer2, strlen(tbuffer2));
    }
    fclose(ptr);
    httpd_resp_send_chunk(req, pattern_footer, strlen(pattern_footer));
    file_get_handler(req, "/spiffs/footer.html",false);
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
esp_err_t web_pattern_list(httpd_req_t *req)  {
    const char *pattern_header = "<table><tr><th>Status<th>Id<th>Name<th>Type<th>Speed<th>Cycles<th>&nbsp\n";
    const char *pattern_footer = "</table><br>\n";
    const char *pattern_heading = "</div></td> <td valign=\"top\"><div id=\"navBreadCrumb\">Pattern List</div><div class=\"centerColumn\" id=\"indexDefault\"><h1 id=\"indexDefaultHeading\"></h1>\n";

    char tbuffer2[350];
    char deleteButton[124];
    uint8_t index;

    parseUrl(req);

    httpd_resp_set_hdr(req, "Content-type", "text/html");
    file_get_handler(req, "/spiffs/header.html",true);

    httpd_resp_send_chunk(req, pattern_heading, strlen(pattern_heading));

    httpd_resp_send_chunk(req, pattern_header, strlen(pattern_header));
    for (index = 0; index < MAX_PATTERN_ENTRY; index++) {
	if (patternTable[index].patternType != PATTERN_NONE) {
	    if (patternTable[index].patternType != PATTERN_BUILT_IN) {
	            sprintf(deleteButton," <button onclick=\"window.location.href = '/patterns.html?delete=/%s';\">Delete</button>",patternTable[index].patternName);
	    } else {
		    deleteButton[0] = 0;
	    }
            snprintf(tbuffer2, sizeof(tbuffer2), "<tr><td>%s<td>%d<td align=\"left\">%s<td>%s<td>%d<td>%d<td><button onclick=\"window.location.href = '/patterns.html?pattern=%d';\">Select</button>%s\n",
	       (step == index)? "==>" : "",
               index+1,
               patternTable[index].patternName,
               (patternTable[index].patternType == PATTERN_BUILT_IN)? "Built-in":"File System",
	       patternTable[index].delay,
	       patternTable[index].cycles,
	       index,
	       deleteButton
            );
            httpd_resp_send_chunk(req, tbuffer2, strlen(tbuffer2));
	}
    }
    httpd_resp_send_chunk(req, pattern_footer, strlen(pattern_footer));
    file_get_handler(req, "/spiffs/patterns.html",true);
    file_get_handler(req, "/spiffs/footer.html",false);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void updatePatternsTask(void *param) {

    allLedsOff();

//    gpio_pad_select_gpio(BUTTON1);
    gpio_reset_pin(BUTTON1);
    gpio_set_direction(BUTTON1 , GPIO_MODE_INPUT);

//    gpio_pad_select_gpio(BUTTON2);
    gpio_reset_pin(BUTTON2);
    gpio_set_direction(BUTTON2 , GPIO_MODE_INPUT);

    while (1) {
	if (patternRun == true) {
	    if (printPattern) {ESP_LOGI(TAG,"running pattern %d %s\n", step, patternTable[step].patternName);}
            switch (patternTable[step].patternType) {
                case PATTERN_BUILT_IN:
                    (patternTable[step].runMe)(patternTable[step].cycles, patternTable[step].delay);
                    break;
                case PATTERN_FILE:
                    runDiskPattern(patternTable[step].fileName, patternTable[step].cycles, patternTable[step].delay);
                    break;
                case PATTERN_SCRIPT_FILE:
                    ESP_LOGI(TAG,"running %s script", patternTable[step].patternName);
		    runDiskScriptPattern(patternTable[step].patternName);
                    break;
                case PATTERN_NONE:
                default:
                    break;
            }
            if ((demoMode) && (exitReason == false)) {
                printf("demo mode forward\n");
                setPatternPlus();
                delay_and_buttons(1); // clean the pending stop
            }
        } else {
            vTaskDelay(1000); // wait a second
        }
    }
}
