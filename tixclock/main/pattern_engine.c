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

#if (0)
#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define TIMER_WITH_RELOAD      1        // testing will be done with auto reload

/*******************************************************************************

    PURPOSE: timer interupt

    INPUTS: None

    RETURN CODE: None

    NOTE: only user group0, timer0, so the TIMER_0 is hard coded, not using the
          para passed in.

*******************************************************************************/

void IRAM_ATTR timer_group0_isr(void *para)
{
    int timer_idx = (int) para;

    /* Clear the interrupt
       and update the alarm time for the timer with without reload */
    TIMERG0.int_clr_timers.t0 = 1;

    /* After the alarm has been triggered
       we need enable it again, so it is triggered the next time */
    TIMERG0.hw_timer[TIMER_GROUP_0].config.alarm_en = TIMER_ALARM_EN;

    strobe_display();
}

/*******************************************************************************

    PURPOSE: setup timer interupt

    INPUTS: None

    RETURN CODE: None

    NOTE: we will be generating an interupt every 2 ms, this will give us a
          100Hz refresh on our display. 2 ms * 5 strobes = 10ms

*******************************************************************************/
void setupTimer( void ) {

    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_WITH_RELOAD,
    }; // default clock source is APB
    timer_init(TIMER_GROUP_0, TIMER_0, &config);

    // 80Mhz / 16 = 

    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 0.002 * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_group0_isr,
                       (void *) TIMER_0, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, TIMER_0);
}
#endif

void setupTimer( void ) {
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &strobe_display,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    /* The timer has been created but is not running yet */

    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 2));
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
	    uint8_t hour = (xAppData.tformat)? timeinfo.tm_hour % 12 : timeinfo.tm_hour;
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

