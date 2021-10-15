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
//   This is the web server, wifi, and internet stuff for:
//    * an ESP32 based 4x4x8 tower.
//    * an ESP32 based 4x4x4 cube
//    * an ESP32 based 4x4x8 tixClock
//   Why does a Blinkie need to talk to the internet? because it can!
//**********************************************************************
//   We are running a dual mode, we can connect to a network, and operate
//   as a hotspot too. 
//**********************************************************************

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "string.h"

#include "dfu.h"
#include "version.h"
#include "pattern_engine.h"
#include "global.h"

static const char *TAG = "dfu";

#define MAX_FILE_NAME_LEN 54
#define MAX_DFU_CONTROL_LEN 80
char dfuBuffer[MAX_DFU_CONTROL_LEN];

extern const uint8_t server_cert_pem_start[] asm("_binary_trustid_x3_root_pem_start");

/*******************************************************************************

    PURPOSE: http event handler used when downloading the dfu control file.
             on "data" events we store the data in a dfuBuffer.

    INPUTS: event struct

    RETURN CODE:

*******************************************************************************/
esp_err_t _http_event_handler2(esp_http_client_event_t *evt)
{
    int8_t length = 0;

    switch (evt->event_id) {
    case HTTP_EVENT_ON_DATA:
        //DDF we could get this in parts?
//        ESP_LOGW(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
//        printf("%.*s", evt->data_len, (char*)evt->data);
        length = (evt->data_len < MAX_DFU_CONTROL_LEN)? evt->data_len : MAX_DFU_CONTROL_LEN;
        memcpy( dfuBuffer, evt->data, length);
        break;
    default:
        break;
    }
    return ESP_OK;
}

/*******************************************************************************

    PURPOSE: Checks server if download is needed, if so it returns true, and
             a URL to the DFU bin file.

    INPUTS: pointer to store the firmware url

    RETURN CODE: bool - is the dfu needed

*******************************************************************************/

bool check_dfu(char * firmware_file, char * url) {
    uint8_t major, minor, build;
    char file[MAX_FILE_NAME_LEN+1];
    bool update = false;

    esp_http_client_config_t configDFUStatus = {
        .url = url,
	.cert_pem = (char *)server_cert_pem_start,
//        .auth_type = HTTP_AUTH_TYPE_NONE,
        .event_handler = _http_event_handler2,
    };

    dfuBuffer[0] = '\0';
    esp_http_client_handle_t client = esp_http_client_init(&configDFUStatus);
    esp_err_t err = esp_http_client_perform(client);
    if ((err == ESP_OK) && (esp_http_client_get_status_code(client) == 200)) {
        if ( 4 != sscanf(dfuBuffer,"%hhd.%hhd.%hhd %54s", &major, &minor, &build, file)) {
            ESP_LOGW(TAG, "parsing of control fail failed");
            major=minor=build=0;
            file[0]='\0';
        }

        snprintf(firmware_file,MAX_DFU_CONTROL_LEN,"%s%s",CONFIG_SERVER_URL,file);
        ESP_LOGW(TAG, ">%d.%d.%d %s<", major, minor, build, firmware_file);

        if (major > MAJOR) {
            update = true;
        } else if ((major == MAJOR) && (minor > MINOR)) {
            update = true;
        } else if ((major == MAJOR) && (minor == MINOR)) {
            update = (build > BUILD)? true : false;
        }
    } else {
        ESP_LOGW(TAG, "fetch control file failed, esp_err=%d http_code=%d",
             err, esp_http_client_get_status_code(client));
    }
    esp_http_client_cleanup(client);

    return(update);
}

/*******************************************************************************

    PURPOSE: simeple test if new firmware exist on server

    INPUTS: dfu flags bit 2 - production / test

    RETURN CODE: bool - if newer code exist

*******************************************************************************/

bool checkForUpdate( uint8_t flags) {
    char firmware_file[MAX_DFU_CONTROL_LEN];
    return( check_dfu(firmware_file, (flags==2)? CONFIG_TEST_FIRMWARE_STATUS_URL :  CONFIG_FIRMWARE_STATUS_URL));
}

/*******************************************************************************

    PURPOSE: calls routine to see if download is needed, performs the DFU if needed.

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

#define DFU_TRIES 3

void dfu_task(void *pvParameter)
{
    uint8_t flags = (uint32_t) pvParameter;
    uint8_t tries = DFU_TRIES;


    char firmware_file[MAX_DFU_CONTROL_LEN];

    // this call returns the file name.
    bool needed = check_dfu( firmware_file, (flags==2)? CONFIG_TEST_FIRMWARE_STATUS_URL :  CONFIG_FIRMWARE_STATUS_URL);

    if ((strlen(firmware_file) != 0) && (needed || (flags != 0))) {

        // do the dfu
        esp_http_client_config_t configDFU = {
            .url = firmware_file,
	    .cert_pem = (char *)server_cert_pem_start,
//            .auth_type = HTTP_AUTH_TYPE_NONE,
//	    .skip_cert_common_name_check = true,
        };

        while (tries != 0) {
#ifndef TIXCLOCK
	    setPatternNumber(0); // give visual feedback we are downloading
#endif
            ESP_LOGW(TAG, "Downloading >%s<",firmware_file);
            esp_err_t ret = esp_https_ota(&configDFU);
            if (ret == ESP_OK) {
		setSystemType( (flags==2)? true : false);
                ESP_LOGW(TAG, "Downloadable Firmware Upgrade Done");
#ifndef TIXCLOCK
		patternEngineOff();
#endif
                vTaskDelay(1000 / portTICK_PERIOD_MS); // wait 1 second
                esp_restart();
            } else {
                ESP_LOGE(TAG, "Downloadable Firmware upgrade failed, retry left=%d",tries);
                tries--;
                vTaskDelay(500 / portTICK_PERIOD_MS); // wait 1/2 a second
            }
        }
        ESP_LOGE(TAG, "Downloadable Firmware Upgrade Failed");
    } else {
        ESP_LOGE(TAG, "No update needed");
    }

    vTaskDelete(NULL);
}

/*******************************************************************************

    PURPOSE: creates the dfu task

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

void perform_dfu(uint8_t flags) {
    if (xAppData.ipName == NULL) {
        ESP_LOGW(TAG, "DFU aborted no internet");
        return;
    }
    //ESP_LOGW(TAG, "Starting DFU flags=%d", flags);
    xTaskCreate(dfu_task, "dfu_task", DFU_TASK_STACK_SIZE, (void *) flags, DFU_TASK_PRIORITY, NULL);
}
