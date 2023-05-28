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
//



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

#define COMSIG0      13
#define COMSIG1      12
#define COMSIG2      14
#define COMSIG3      27
#define COMSIG4      26
#define COMSIG5      25

void strobe_display(void) {
    static uint8_t strobeLine = 0;

    switch (strobeLine) {
    case 0:
            gpio_set_level(COMSIG1, 1);
            gpio_set_level(COMSIG2, 1);
            gpio_set_level(COMSIG3, 1);
            gpio_set_level(COMSIG4, 1);
            gpio_set_level(COMSIG5, 1);
	    gpio_set_direction(COMSIG0, GPIO_MODE_OUTPUT);
	    gpio_set_direction(COMSIG1, (LEDs[1][3] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG2, (LEDs[0][2] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG3, (LEDs[2][4] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG4, (LEDs[2][5] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG5, (LEDs[2][2] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
            gpio_set_level(COMSIG0, 0);
	    break;
    case 1:
            gpio_set_level(COMSIG0, 1);
            gpio_set_level(COMSIG2, 1);
            gpio_set_level(COMSIG3, 1);
            gpio_set_level(COMSIG4, 1);
            gpio_set_level(COMSIG5, 1);
	    gpio_set_direction(COMSIG0, (LEDs[0][8] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG1, GPIO_MODE_OUTPUT);
	    gpio_set_direction(COMSIG2, (LEDs[0][1] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG3, (LEDs[2][6] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG4, (LEDs[1][6] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG5, (LEDs[1][7] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
            gpio_set_level(COMSIG1, 0);
	    break;
    case 2:
            gpio_set_level(COMSIG0, 1);
            gpio_set_level(COMSIG1, 1);
            gpio_set_level(COMSIG3, 1);
            gpio_set_level(COMSIG4, 1);
            gpio_set_level(COMSIG5, 1);
	    gpio_set_direction(COMSIG0, (LEDs[1][5] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG1, (LEDs[1][4] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG2, GPIO_MODE_OUTPUT);
	    gpio_set_direction(COMSIG3, (LEDs[2][7] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG4, (LEDs[2][8] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG5, (LEDs[1][8] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
            gpio_set_level(COMSIG2, 0);
	    break;
    case 3:
            gpio_set_level(COMSIG0, 1);
            gpio_set_level(COMSIG1, 1);
            gpio_set_level(COMSIG2, 1);
            gpio_set_level(COMSIG4, 1);
            gpio_set_level(COMSIG5, 1);
	    gpio_set_direction(COMSIG0, (LEDs[1][0] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG1, (LEDs[0][0] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG2, (LEDs[0][5] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG3, GPIO_MODE_OUTPUT);
	    gpio_set_direction(COMSIG4, (LEDs[2][3] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG5, (LEDs[2][0] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
            gpio_set_level(COMSIG3, 0);
	    break;
    case 4:
            gpio_set_level(COMSIG0, 1);
            gpio_set_level(COMSIG1, 1);
            gpio_set_level(COMSIG2, 1);
            gpio_set_level(COMSIG3, 1);
            gpio_set_level(COMSIG5, 1);
	    gpio_set_direction(COMSIG0, (LEDs[0][6] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG1, (LEDs[1][1] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG2, (LEDs[0][4] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
	    gpio_set_direction(COMSIG3, GPIO_MODE_INPUT); //nc
	    gpio_set_direction(COMSIG4, GPIO_MODE_OUTPUT);
	    gpio_set_direction(COMSIG5, (LEDs[2][1] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT);
            gpio_set_level(COMSIG4, 0);
	    break;
    case 5:
            gpio_set_level(COMSIG0, 1);
            gpio_set_level(COMSIG1, 1);
            gpio_set_level(COMSIG2, 1);
            gpio_set_level(COMSIG3, 1);
            gpio_set_level(COMSIG4, 1);
	    gpio_set_direction(COMSIG0, (LEDs[0][7] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT); //bad
	    gpio_set_direction(COMSIG1, (LEDs[1][2] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT); //bad
	    gpio_set_direction(COMSIG2, (LEDs[0][3] == true)? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT); //bad
	    gpio_set_direction(COMSIG3, GPIO_MODE_INPUT); //nc
	    gpio_set_direction(COMSIG4, GPIO_MODE_INPUT); //nc
	    gpio_set_direction(COMSIG5, GPIO_MODE_OUTPUT);
            gpio_set_level(COMSIG5, 0);
	    break;
    }
    strobeLine = (strobeLine > 4)? 0 : strobeLine+1;
}

void init_LEDs( void ) {

    // LED drive lines
    gpio_pad_select_gpio(COMSIG0);
    gpio_pad_select_gpio(COMSIG1);
    gpio_pad_select_gpio(COMSIG2);
    gpio_pad_select_gpio(COMSIG3);
    gpio_pad_select_gpio(COMSIG4);
    gpio_pad_select_gpio(COMSIG5);
    gpio_set_pull_mode(COMSIG0, GPIO_FLOATING);
    gpio_set_pull_mode(COMSIG1, GPIO_FLOATING);
    gpio_set_pull_mode(COMSIG2, GPIO_FLOATING);
    gpio_set_pull_mode(COMSIG3, GPIO_FLOATING);
    gpio_set_pull_mode(COMSIG4, GPIO_FLOATING);
    gpio_set_pull_mode(COMSIG5, GPIO_FLOATING);

    gpio_set_drive_capability(COMSIG0, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(COMSIG1, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(COMSIG2, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(COMSIG3, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(COMSIG4, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(COMSIG5, GPIO_DRIVE_CAP_3);
    
    // push button
    gpio_pad_select_gpio(0);
    gpio_set_direction(0, GPIO_MODE_INPUT);

}

