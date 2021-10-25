//   Copyright (C) 2020 Dwayne Forsyth
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
//   This is the LED driver for an ESP32 based tixClock
//   It runs the GPIO drive the physical LEDs.
//**********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_system.h"
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "driver/gpio.h"

#include "led_driver.h"
#include "web_server.h"
//#include "disk_system.h"
#include "global.h"
#include "driver/uart.h"

bool LEDs[3][9];

#define MATRIX

#ifdef MATRIX
#include "led_driver_matrix.c"
#else
#include "led_driver_charlie.c"
#endif

typedef struct {
    uint8_t row;
    uint8_t col;
} led_pair_t;

/*******************************************************************************

    PURPOSE: clear_display

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

void clear_display(void) {
    uint8_t i,k;

    for (i=0;i<3;i++) {
        for (k=0;k<9;k++) {
            LEDs[i][k] = false;
     	}
    }
}

/*******************************************************************************

    PURPOSE: updates the LEDs

    INPUTS: led_total  - the the display how many leds are there 3-9
            leds_on - how many LEDs need to be turned on
	    led_map - how to the leds maps into the display

    RETURN CODE: None

*******************************************************************************/

// design error: new digit display should not equal old if possible.
void display_chase() {
   static uint8_t count = 1;

   for (uint8_t x = 0; x < 9; x++) {
	   switch (count) {
	   case 0: LEDs[0][x] = true; break;
	   case 1: LEDs[1][x] = true; break;
	   case 2: LEDs[2][x] = true; break;
	   case 3: LEDs[1][x] = true; break;
	   }
           vTaskDelay(100);
	   LEDs[0][x] = false;
	   LEDs[1][x] = false;
	   LEDs[2][x] = false;
   }
   count = (count+1) % 4;
}


/*******************************************************************************

    PURPOSE: updates the LEDs

    INPUTS: led_total  - the the display how many leds are there 3-9
            leds_on - how many LEDs need to be turned on
	    led_map - how to the leds maps into the display

    RETURN CODE: None

*******************************************************************************/

// design error: new digit display should not equal old if possible.
void display_digit( uint8_t led_total, uint8_t leds_on, led_pair_t led_map[]) {
    uint8_t i;

    while (leds_on > 0) {
        i = rand() % led_total;
	if (LEDs[led_map[i].row][led_map[i].col] == false) {
            LEDs[led_map[i].row][led_map[i].col] = true;
	    leds_on = leds_on - 1;
        }
    }
}
/*******************************************************************************

    PURPOSE: updates the LEDs with the time.

    INPUTS: hour and min

    RETURN CODE: None

*******************************************************************************/

void display_time( uint8_t hour, uint8_t min) {

    led_pair_t LED_map_ml[9] = {{0,6},{0,7},{0,8},
	                        {1,6},{1,7},{1,8},
		                {2,6},{2,7},{2,8}};

    led_pair_t LED_map_mh[6] = {{0,4},{0,5},
	                        {1,4},{1,5},
		                {2,4},{2,5}};

    led_pair_t LED_map_hl[9] = {{0,1},{0,2},{0,3},
	                        {1,1},{1,2},{1,3},
		                {2,1},{2,2},{2,3}};

    led_pair_t LED_map_hh[3] = {{0,0},
	                        {1,0},
		                {2,0}};

    display_digit( 9,  min % 10, LED_map_ml);
    display_digit( 6,  min / 10, LED_map_mh);
    display_digit( 9, hour % 10, LED_map_hl);
    display_digit( 3, hour / 10, LED_map_hh);
}
