


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "LED_Driver.h"

/*
   This code drive the 2DKits.com 4x4x8 tower
*/

void fad_testing( uint32_t cycles) {

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

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void walking_testing( uint32_t cycles) {
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

	if (x==0) printf("%X [%d,%d,%d] = (%d,%d,%d)\n",step, l,x,y,r,g,b);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void rgb_fade( uint32_t cycles) {
   uint8_t r,g,b,fad,color;

   while(cycles != 0) {
      cycles--;
      for (color=0;color < 7; color++) {
	  for (fad=1; fad < 16; fad++) {
             r = (color & 0x01)? 0 : fad;
             g = (color & 0x02)? 0 : fad;
             b = (color & 0x04)? 0 : fad;
             allLedsColor(r,g,b);
             vTaskDelay(100 / portTICK_PERIOD_MS);
	  }
	  for (fad= 14; fad != 0; fad--) {
             r = (color & 0x01)? 0 : fad;
             g = (color & 0x02)? 0 : fad;
             b = (color & 0x04)? 0 : fad;
             allLedsColor(r,g,b);
             vTaskDelay(100 / portTICK_PERIOD_MS);
	  }
      }
   }
}



void updatePatternsTask(void *param) {

    while (1) {
        allLedsOn();
        vTaskDelay(1000 * 10 / portTICK_PERIOD_MS);
	allLedsOff();
        vTaskDelay(1000 * 1 / portTICK_PERIOD_MS);
        rgb_fade( 5 );
	allLedsOff();
        vTaskDelay(1000 * 1 / portTICK_PERIOD_MS);
        walking_testing(0x800);
        allLedsColor( 15,0,0);
        vTaskDelay(1000 * 3 / portTICK_PERIOD_MS);
        allLedsColor( 0,15,0);
        vTaskDelay(1000 * 3 / portTICK_PERIOD_MS);
        allLedsColor( 0,0,15);
        vTaskDelay(1000 * 3 / portTICK_PERIOD_MS);
        fad_testing( 300);   //30 seconds
    }
}




void app_main()
{
 
    printf("Hello from Tower\n");
    init_LED_driver();
    xTaskCreate(updatePatternsTask, "updatePatternsTask", 4*1024, NULL, 23, NULL);

//  The ESP32 framework has already started the Scheduler, starting it again just
//  causes a crash
}
