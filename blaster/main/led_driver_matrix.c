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
#include "esp32/rom/gpio.h"

#define COLSIG0      17
#define COLSIG1      15
#define COLSIG2      12 
#define COLSIG3      02
#define COLSIG4      13

#define B_COLSIG0      (1<<COLSIG0)
#define B_COLSIG1      (1<<COLSIG1)
#define B_COLSIG2      (1<<COLSIG2)
#define B_COLSIG3      (1<<COLSIG3)
#define B_COLSIG4      (1<<COLSIG4)

#define ROWSIG0      21
#define ROWSIG1      22
#define ROWSIG2      25
#define ROWSIG3      26
#define ROWSIG4      27
#define ROWSIG5      32
#define B_ROWSIG0      (1<<21)
#define B_ROWSIG1      (1<<22)
#define B_ROWSIG2      (1<<23)
#define B_ROWSIG3      (1<<05)
#define B_ROWSIG4      (1<<18)
#define B_ROWSIG5      (1<<19)
#define B_ROWALL      ((1<<23)||(1<<21)||(1<<23)||(1<<5)||(1<<18)||(1<<19))

bool LEDs[3][9];

/*******************************************************************************

    PURPOSE: interrupt routine that updates the LED display

    INPUTS: None

    RETURN CODE: None

    NOTE: using rom/gpio.h api which is interupt IRAM_ATTR friendly

*******************************************************************************/

void strobe_display(void * arg) {
    static RTC_DATA_ATTR uint8_t strobeLine = 0;
    uint32_t set = 0;
    uint32_t clear = 0;

    switch (strobeLine) {
    case 0:
        if (LEDs[0][0]) { set += B_ROWSIG0; } else { clear += B_ROWSIG0; }
        if (LEDs[1][0]) { set += B_ROWSIG1; } else { clear += B_ROWSIG1; }
        if (LEDs[2][0]) { set += B_ROWSIG2; } else { clear += B_ROWSIG2; }
	clear += B_ROWSIG3;
	clear += B_ROWSIG4;
	clear += B_ROWSIG5;
        gpio_output_set(B_COLSIG4, 0, B_COLSIG4, 0);
        gpio_output_set(set, clear, B_ROWALL, 0);
        gpio_output_set(0, B_COLSIG0, B_COLSIG0, 0);
        break;

    case 1: // we have a problem with driving hour diffent colors on COL1
	clear += B_ROWSIG0;
	clear += B_ROWSIG1;
	clear += B_ROWSIG2;
        if (LEDs[0][1]) { set += B_ROWSIG3; } else { clear += B_ROWSIG3; }
        if (LEDs[1][1]) { set += B_ROWSIG4; } else { clear += B_ROWSIG4; }
        if (LEDs[2][1]) { set += B_ROWSIG5; } else { clear += B_ROWSIG5; }
        gpio_output_set(B_COLSIG0, 0, B_COLSIG0, 0);
        gpio_output_set(set, clear, B_ROWALL, 0);
        gpio_output_set(0, B_COLSIG0, B_COLSIG0, 0);
        break;

    case 2:
        if (LEDs[0][2]) { set += B_ROWSIG0; } else { clear += B_ROWSIG0; }
        if (LEDs[1][2]) { set += B_ROWSIG1; } else { clear += B_ROWSIG1; }
        if (LEDs[2][2]) { set += B_ROWSIG2; } else { clear += B_ROWSIG2; }
        if (LEDs[0][3]) { set += B_ROWSIG3; } else { clear += B_ROWSIG3; }
        if (LEDs[1][3]) { set += B_ROWSIG4; } else { clear += B_ROWSIG4; }
        if (LEDs[2][3]) { set += B_ROWSIG5; } else { clear += B_ROWSIG5; }
        gpio_output_set(B_COLSIG0, 0, B_COLSIG0, 0);
        gpio_output_set(set, clear, B_ROWALL, 0);
        gpio_output_set(0, B_COLSIG1, B_COLSIG1, 0);
        break;

    case 3:
        if (LEDs[0][4]) { set += B_ROWSIG0; } else { clear += B_ROWSIG0; }
        if (LEDs[1][4]) { set += B_ROWSIG1; } else { clear += B_ROWSIG1; }
        if (LEDs[2][4]) { set += B_ROWSIG2; } else { clear += B_ROWSIG2; }
        if (LEDs[0][5]) { set += B_ROWSIG3; } else { clear += B_ROWSIG3; }
        if (LEDs[1][5]) { set += B_ROWSIG4; } else { clear += B_ROWSIG4; }
        if (LEDs[2][5]) { set += B_ROWSIG5; } else { clear += B_ROWSIG5; }
        gpio_output_set(B_COLSIG1, 0, B_COLSIG1, 0);
        gpio_output_set(set, clear, B_ROWALL, 0);
        gpio_output_set(0, B_COLSIG2, B_COLSIG2, 0);
        break;

    case 4:
        if (LEDs[0][6]) { set += B_ROWSIG0; } else { clear += B_ROWSIG0; }
        if (LEDs[1][6]) { set += B_ROWSIG1; } else { clear += B_ROWSIG1; }
        if (LEDs[2][6]) { set += B_ROWSIG2; } else { clear += B_ROWSIG2; }
        if (LEDs[0][7]) { set += B_ROWSIG3; } else { clear += B_ROWSIG3; }
        if (LEDs[1][7]) { set += B_ROWSIG4; } else { clear += B_ROWSIG4; }
        if (LEDs[2][7]) { set += B_ROWSIG5; } else { clear += B_ROWSIG5; }
        gpio_output_set(B_COLSIG2, 0, B_COLSIG2, 0);
        gpio_output_set(set, clear, B_ROWALL, 0);
        gpio_output_set(0, B_COLSIG3, B_COLSIG3, 0);
        break;

    case 5:
    default:
        if (LEDs[0][8]) { set += B_ROWSIG0; } else { clear += B_ROWSIG0; }
        if (LEDs[1][8]) { set += B_ROWSIG1; } else { clear += B_ROWSIG1; }
        if (LEDs[2][8]) { set += B_ROWSIG2; } else { clear += B_ROWSIG2; }
        gpio_output_set(B_COLSIG3, 0, B_COLSIG3, 0);
        gpio_output_set(set, clear, B_ROWALL, 0);
        gpio_output_set(0, B_COLSIG4, B_COLSIG4, 0);
        break;
    }
    strobeLine = (strobeLine > 5)? 0 : strobeLine+1;
}

void init_LEDs( void ) {

    // LED drive lines
    gpio_pad_select_gpio(COLSIG0);
    gpio_pad_select_gpio(COLSIG1);
    gpio_pad_select_gpio(COLSIG2);
    gpio_pad_select_gpio(COLSIG3);
    gpio_pad_select_gpio(COLSIG4);
    gpio_pad_select_gpio(ROWSIG0);
    gpio_pad_select_gpio(ROWSIG1);
    gpio_pad_select_gpio(ROWSIG2);
    gpio_pad_select_gpio(ROWSIG3);
    gpio_pad_select_gpio(ROWSIG4);
    gpio_pad_select_gpio(ROWSIG5);

    gpio_set_pull_mode(COLSIG0, GPIO_FLOATING);
    gpio_set_pull_mode(COLSIG1, GPIO_FLOATING);
    gpio_set_pull_mode(COLSIG2, GPIO_FLOATING);
    gpio_set_pull_mode(COLSIG3, GPIO_FLOATING);
    gpio_set_pull_mode(COLSIG4, GPIO_FLOATING);
    gpio_set_pull_mode(ROWSIG0, GPIO_FLOATING);
    gpio_set_pull_mode(ROWSIG1, GPIO_FLOATING);
    gpio_set_pull_mode(ROWSIG2, GPIO_FLOATING);
    gpio_set_pull_mode(ROWSIG3, GPIO_FLOATING);
    gpio_set_pull_mode(ROWSIG4, GPIO_FLOATING);
    gpio_set_pull_mode(ROWSIG5, GPIO_FLOATING);

    gpio_set_direction(COLSIG0, GPIO_MODE_OUTPUT);
    gpio_set_direction(COLSIG1, GPIO_MODE_OUTPUT);
    gpio_set_direction(COLSIG2, GPIO_MODE_OUTPUT);
    gpio_set_direction(COLSIG3, GPIO_MODE_OUTPUT);
    gpio_set_direction(COLSIG4, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROWSIG0, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROWSIG1, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROWSIG2, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROWSIG3, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROWSIG4, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROWSIG5, GPIO_MODE_OUTPUT);

    gpio_set_drive_capability(COLSIG0, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(COLSIG1, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(COLSIG2, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(COLSIG3, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(COLSIG4, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(ROWSIG0, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(ROWSIG1, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(ROWSIG2, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(ROWSIG3, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(ROWSIG4, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(ROWSIG5, GPIO_DRIVE_CAP_3);

    // push button
    gpio_pad_select_gpio(0);
    gpio_set_direction(0, GPIO_MODE_INPUT);

}

