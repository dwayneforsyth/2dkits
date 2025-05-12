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
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "nvs_flash.h"

#include "esp_system.h"
// #include "esp_http_server.h"
// #include "esp_wifi.h"
#include "driver/gpio.h"
#include "esp_log.h"
// static const char *TAG = "PATTERN";


#include "led_driver.h"
// #include "web_server.h"
// #include "disk_system.h"
//#include "driver/timer.h"
#include "esp_timer.h"
#include "global.h"

bool debugLed = false;

bool getDebugLed( void ) {
    return(debugLed);
}
void setDebugLed( bool onOff ) {
    debugLed = onOff;
}

char d( uint8_t in ) {
   return( (in)? '@' : '.');
}

void display_LED_uart( void) {
    int8_t i;
    for (i=2;i>=0;i--) {
        printf( "%c  %c %c %c  %c %c  %c %c %c\n",
            d(LEDs[i][0]),
            d(LEDs[i][1]), d(LEDs[i][2]), d(LEDs[i][3]),
            d(LEDs[i][4]), d(LEDs[i][5]), d(LEDs[i][6]),
            d(LEDs[i][7]), d(LEDs[i][8]));
        }
}

esp_timer_handle_t periodic_timer;

void timer_cb( void * arg ) {
	static uint8_t strobeLine = 0;
	const uint8_t strobeToDigit[6] = { 0, 1, 1, 2, 3, 3 };

	strobeLine = (strobeLine > 5)? 0 : strobeLine+1;

        esp_timer_start_once(periodic_timer, 50 + getDigiBrightness( strobeToDigit[strobeLine])*50);
	

	
	strobe_display( strobeLine );
}

void setupTimer( void ) {
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &timer_cb,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic"
    };
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    /* The timer has been created but is not running yet */
    
    ESP_ERROR_CHECK(esp_timer_start_once(periodic_timer, 500));
}


void updatePatternsTask( void ) {
    struct tm timeinfo;
    char timeBuf[64];
    time_t now, old_now;
    time(&old_now);

    setupTimer();

    while (1) {
        time(&now);
        if ((old_now>>2) != (now>>2)) {
            old_now = now;
            clear_display();
            localtime_r(&now, &timeinfo);
            strftime(timeBuf, sizeof(timeBuf), "%c", &timeinfo);
	    uint8_t hour = timeinfo.tm_hour;
	    if (hour > 13) { // we want 12:30 not 00:30
                hour -= 12;
	    }
	    if (timeinfo.tm_year > 100) {
                display_time(hour, timeinfo.tm_min);
                if (debugLed) printf("%s\n",timeBuf);
                if (debugLed) {display_LED_uart();}
	    } else {
		display_chase();
	    }
        }
	vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

