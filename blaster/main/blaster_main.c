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
//   This is the main program for an ESP32 based TixClock
//   It runs the initilization sequesnce, all the good stuff is in
//   other files.
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
#include "disk_system.h"
#include "global.h"
#include "driver/uart.h"
#include "console.h"
#include "pattern_engine.h"
#include "display.h"

#include "esp_log.h"
// static const char *TAG = "MAIN";

void init_gpio( void ) {

    // push button
    esp_rom_gpio_pad_select_gpio(0);
    gpio_set_direction(0, GPIO_MODE_INPUT);

}

void app_main()
{
    time_t now;
    
    printf("Hello from Blaster\n");

    init_gpio();
    init_LEDs();
    setenv("TZ", "CST6CDT", 1);
    tzset();

    static httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(nvs_flash_init());
    time(&now);

    initialise_wifi_p1(&server);
    initialise_disk();

    loadSettings();
    
    initialise_wifi_p2(&server);

//    xTaskCreate(updatePatternsTask, "updatePatternsTask", 4*1024, NULL, 23, NULL);

    display_init();

    consoleInit();
    //consoleInit does not return
}
