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
//   This is the main program for an ESP32 based 4x4x8 tower.
//   It runs the initilization sequesnce, all the good stuff is in
//   other files.
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

#include "led_driver.h"
#include "web_server.h"
#include "disk_system.h"
#include "pattern_engine.h"

/*
   This code drive the 2DKits.com 4x4x8 tower
*/

void app_main()
{
 
    printf("Hello from Tower\n");
    init_LED_driver();
    xTaskCreate(updatePatternsTask, "updatePatternsTask", 4*1024, NULL, 23, NULL);


    static httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(nvs_flash_init());
    initialise_wifi(&server);

    initialise_disk();
    disk_dir_list("/spiffs",NULL);


#define GPIO_INPUT_IO_0     34
#define GPIO_INPUT_IO_1     39
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))

    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);



//  The ESP32 framework has already started the Scheduler, starting it again just
//  causes a crash
}
