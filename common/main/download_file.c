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

#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
static const char *TAG="DFile";
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include <esp_system.h>
#include <nvs_flash.h>

FILE *pDownloadFile = NULL;

extern const uint8_t server_cert_pem_start[] asm("_binary_trustid_x3_root_pem_start");

/*******************************************************************************

    PURPOSE: http event handler used when downloading the dfu control file.
             on "data" events we store the data in a dfuBuffer.

    INPUTS: event struct

    RETURN CODE:

*******************************************************************************/
esp_err_t _http_event_handle3(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ON_DATA:
        //DDF we could get this in parts?
//        ESP_LOGW(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
//        printf("%.*s", evt->data_len, (char*)evt->data);
//        printf("writefile %d bytes\n", evt->data_len);
	fwrite( evt->data, evt->data_len, 1, pDownloadFile);

        break;
    default:
        break;
    }
    return ESP_OK;
}

/*******************************************************************************
    PURPOSE: donwloads a file (http url) to the flash disk

    INPUTS:
        Web connection
        Token

    RETURN CODE:
        error code

    NOTES:
        sends data down the web connection.

*******************************************************************************/

void download_file( char * diskname, char * url) {

    esp_http_client_config_t configDFUStatus = {
        .url = url,
        .cert_pem = (char *)server_cert_pem_start,
        .event_handler = _http_event_handle3,
    };
    ESP_LOGW(TAG, "download name=%s url=%s", diskname, url);

    pDownloadFile = fopen( diskname,"wb");
    esp_http_client_handle_t client = esp_http_client_init(&configDFUStatus);
    esp_err_t err = esp_http_client_perform(client);
    if ((err == ESP_OK) && (esp_http_client_get_status_code(client) == 200)) {
        ESP_LOGW(TAG, "download done");
    } else {
        ESP_LOGW(TAG, "file download fail, esp_err=%d http_code=%d",
             err, esp_http_client_get_status_code(client));
    }
    fclose( pDownloadFile);

    esp_http_client_cleanup(client);

    return;
}


