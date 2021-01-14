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
#include "esp_log.h"

#include "led_driver.h"
#include "web_server.h"
#include "disk_system.h"
#include "pattern_engine.h"
#include "console.h"
#include "global.h"

static const char *TAG = "MAIN";

/*
   This code drive the 2DKits.com 4x4x8 tower
*/

/*******************************************************************************
    PURPOSE: check each file found

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void addPattern_cb( char type, char * size, char * sha, char *name, void *data) {

    if (strcmp(".pat",&(name[strlen(name)-4])) == 0) {
        addPattern(name);
    }
}

void app_main()
{
 
    printf("Hello from Tower\n");
    init_LED_driver();
    allLedsColor(0,15,0);


    static httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(nvs_flash_init());

    initialise_wifi_p1(&server);
    initialise_disk();
    loadSettings();

    if (getSystemType()) { 
        printf("***\n* Test Image\n***\n");
    } else {
        printf("***\n* Production Image\n***\n");
    }

    diskDirCfg_t req = {
        .path = "/spiffs",
        .line_cb = addPattern_cb
    };

    disk_dir(req);

    initialise_wifi_p2(&server);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    xTaskCreate(updatePatternsTask, "updatePatternsTask", 4*1024, NULL, 23, NULL);

    consoleInit();


//  The ESP32 framework has already started the Scheduler, starting it again just
//  causes a crash
}
