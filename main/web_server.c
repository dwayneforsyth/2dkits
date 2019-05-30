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
//   This is the web server, wifi, and internet stuff for an ESP32 based 4x4x8 tower.
//   Why does a Blinkie need to talk to the internet? because it can!
//**********************************************************************
//   We are running a dual mode, we can connect to a network, and operate
//   as a hotspot too. 
//**********************************************************************


/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <time.h>

#include <esp_http_server.h>
#include <esp_sntp.h>

#include "disk_system.h"
#include "pattern_engine.h"
#include "global.h"
extern blinkieAppData_t xAppData;

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 * The examples use simple WiFi configuration that you can set via
 * 'make menuconfig'.
 * If you'd rather not, just change the below entries to strings
 * with the config you want -
 * ie. #define EXAMPLE_WIFI_SSID "mywifissid"
*/
//#define EXAMPLE_WIFI_SSID "Optimal-LAN"
//#define EXAMPLE_WIFI_PASS "wifiworks"
#define EXAMPLE_WIFI_SSID "dforsythnet"
#define EXAMPLE_WIFI_PASS ""
#define AP_EXAMPLE_WIFI_SSID "blinkie\0"
#define AP_EXAMPLE_WIFI_PASS "12345678"

static const char *TAG="APP";


/*******************************************************************************
    PURPOSE: pushes context to the web connection based on the token passed in.

    INPUTS:
        Web connection
        Token

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:
        sends data down the web connection.

*******************************************************************************/

esp_err_t lookupToken(httpd_req_t *req, char *token) {
    char tBuffer[80];
    time_t now;
    struct tm timeinfo;


    if (strcmp("%patternNumber",token)==0) {
	sprintf(tBuffer, "%d",getPatternNumber());
        httpd_resp_send_chunk(req, tBuffer, strlen(tBuffer));
    } else if (strcmp("%patternName",token)==0) {
	sprintf(tBuffer, "%s",getPatternName());
        httpd_resp_send_chunk(req, tBuffer, strlen(tBuffer));
    } else if (strcmp("%timeDate",token)==0) {
        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(tBuffer, 80, "%c", &timeinfo);
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strcmp("%hssid",token)==0) {
	sprintf(tBuffer, "blinkie"); //DDF hard codded
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strcmp("%hpasswd",token)==0) {
	sprintf(tBuffer, "[none]"); //DDF hard codded
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strcmp("%wssid",token)==0) {
	sprintf(tBuffer, "dforsyth.net"); //DDF hard codded
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strcmp("%wpasswd",token)==0) {
	sprintf(tBuffer, "[none2]"); //DDF hard codded
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strncmp("%hchsel",token,7)==0) {
	//DDF send nothing - hard codded
    } else if (strncmp("%rgb",token,4)==0) {
	//DDF send nothing - hard codded
    } else if (strncmp("%tz",token,3)==0) {
	//DDF send nothing - hard codded
    } else if (strcmp("%sasip",token)==0) {
	if (xAppData.ipName != NULL) {
	    sprintf(tBuffer, "%s", xAppData.ipName);
	} else {
	    sprintf(tBuffer, "[none]");
	}
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
	
    } else {
	sprintf(tBuffer, "%s%%",token);
        httpd_resp_send_chunk(req, tBuffer, strlen(tBuffer));
    }
    return(ESP_OK);
}

/*******************************************************************************
    PURPOSE:
        Sends a file on the intern file system to the html handler. If the file
	is not binary, it will scan for %tokens% and call a procedure to send
	other data in its place.

    INPUTS:
        http handler
	file name
	binary flag

    OUTPUTS:
        NONE

    RETURN CODE:
        Error code

    NOTES:

*******************************************************************************/
esp_err_t file_get_handler(httpd_req_t *req, char *filename, bool binary)
{
    ESP_LOGI(TAG, "Reading file : %s", filename);

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        const char * resp_str = "File doesn't exist";
        httpd_resp_send(req, resp_str, strlen(resp_str));
        return ESP_FAIL;
    }

    /* Read file in chunks (relaxes any constraint due to large file sizes)
     * and send HTTP response in chunked encoding */
    char   chunk[1024];
    char   token[20];
    size_t chunkSize;
    bool readingToken = false;
    uint16_t chunkIndex;
    uint16_t chunkHead = 0;
    uint8_t tokenIndex = 0;

    do {
        chunkSize = fread(chunk, 1, sizeof(chunk), f);
	chunkHead = 0;
	if (chunkSize != 0) {
	    for (chunkIndex=0;chunkIndex<chunkSize;chunkIndex++) {
	        if ((binary == false) && (chunk[chunkIndex] == '%')) {
		    if (readingToken == true) {
			readingToken = false;
			chunkHead = chunkIndex+1;
		        token[tokenIndex] = 0;
                        // perform token lookup
                        ESP_LOGI(TAG, "token = %s", token);
			lookupToken(req, token);
		    } else {
			readingToken = true;
		        token[0] = '%';
			tokenIndex = 1;
			// dump current buffer
                        if (httpd_resp_send_chunk(req, &chunk[chunkHead], chunkIndex-chunkHead) != ESP_OK) {
                            fclose(f);
                            return ESP_FAIL;
			}
                    }
		} else if (readingToken == true) {
		    token[tokenIndex] = chunk[chunkIndex];
		    tokenIndex++;
		    // ok, it was not a token...
		    if (tokenIndex > 20) {
                        if (httpd_resp_send_chunk(req, token, tokenIndex) != ESP_OK) {
                            fclose(f);
                            return ESP_FAIL;
			}
			readingToken = false;
			chunkHead = chunkIndex+1;
		        token[tokenIndex] = 0;
		    }
		}
	    }
	    if (readingToken == false) {
	        // dump the buffer
                if (httpd_resp_send_chunk(req, &chunk[chunkHead], chunkIndex-chunkHead) != ESP_OK) {
                    fclose(f);
                    return ESP_FAIL;
	        }
            }
	}
    } while (chunkSize != 0);

    // ok, it was not a token...
    if (readingToken == true) {
        token[tokenIndex] = 0;
        if (httpd_resp_send_chunk(req, token, tokenIndex) != ESP_OK) {
            fclose(f);
            return ESP_FAIL;
	}
    }

    fclose(f);
    // Adding this logging caused the code to start working.....
    ESP_LOGI(TAG, "done file : %s", filename);
    return ESP_OK;
}

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
esp_err_t get_file_handler(httpd_req_t *req)
{
    uint8_t sLength = 0;
    char *filename = (char *) req->user_ctx;
    bool headFoot = false;

    ESP_LOGI(TAG, "ctx = %s", filename);
    sLength = strlen(filename);
    if (strcmp(".jpg",&filename[sLength-4]) == 0) {
        httpd_resp_set_hdr(req, "Content-type", "image/jpg");
        ESP_LOGI(TAG, "found jpg:");
    } else if (strcmp(".html",&filename[sLength-5]) == 0) {
        httpd_resp_set_hdr(req, "Content-type", "text/html");
        ESP_LOGI(TAG, "found html:");
        headFoot = true;
    } else if (strcmp(".css",&filename[sLength-4]) == 0) {
        httpd_resp_set_hdr(req, "Content-type", "text/html");
        ESP_LOGI(TAG, "found css:");
    }
    /* Send a simple response */
    if (headFoot) file_get_handler(req, "/spiffs/header.html", true);
    file_get_handler(req, req->user_ctx,!headFoot);
    if (headFoot) file_get_handler(req, "/spiffs/footer.html", false);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}


/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/index.html"
};

httpd_uri_t about = {
    .uri       = "/about.html",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/about.html"
};

httpd_uri_t settings = {
    .uri       = "/settings.html",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/settings.html"
};

httpd_uri_t patterns = {
    .uri       = "/patterns.html",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/patterns.html"
};

httpd_uri_t logo = {
    .uri       = "/header-bg.jpg",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/header-bg.jpg"
};

httpd_uri_t title = {
    .uri       = "/box-title-bg.jpg",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/box-title-bg.jpg"
};

httpd_uri_t content = {
    .uri       = "/content-bg.jpg",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/content-bg.jpg"
};

httpd_uri_t ccs = {
    .uri       = "/styles.css",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/styles.css"
};

httpd_uri_t web_dir = {
    .uri       = "/dir",
    .method    = HTTP_GET,
    .handler   = web_disk_dir_list,
    .user_ctx  = NULL
};

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 10;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: %d", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &ccs);
        httpd_register_uri_handler(server, &web_dir);
        httpd_register_uri_handler(server, &logo);
        httpd_register_uri_handler(server, &title);
        httpd_register_uri_handler(server, &about);
        httpd_register_uri_handler(server, &settings);
        httpd_register_uri_handler(server, &patterns);
        httpd_register_uri_handler(server, &content);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    wifi_sta_info_t *sta;
    static bool serverInit = false;

    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xAppData.ipName = ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip);
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        ESP_LOGI(TAG, "Got IP: %s",
                ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        /* Start the web server */
	if (serverInit == false) {
	   start_webserver();
	   serverInit = true;
	}

        ESP_LOGI(TAG, "Initializing SNTP");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
	sntp_init();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
        xAppData.ipName = NULL;
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
        ESP_LOGI(TAG,"station connected to access point.");
        /* Start the web server */
	if (serverInit == false) {
	   start_webserver();
	   serverInit = true;
	}
        wifi_sta_list_t sta_list;
        ESP_ERROR_CHECK( esp_wifi_ap_get_sta_list(&sta_list));
        for(int i = 0; i < sta_list.num; i++)
        {
            //Print the mac address of the connected station
            sta =  &(sta_list.sta[i]);
            ESP_LOGI(TAG,"Station %d MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", i,
                sta->mac[0], sta->mac[1], sta->mac[2], sta->mac[3], sta->mac[4], sta->mac[5]);
        }
        break;
    default:
       ESP_LOGI(TAG, "Network event %d",event->event_id);
        break;
    }
    return ESP_OK;
}

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void initialise_wifi(void *arg)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config_sta = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    wifi_config_t wifi_config_ap = {
        .ap = {
            .ssid = AP_EXAMPLE_WIFI_SSID,
            .ssid_len = 0,
            .password = "",
            .channel = 9, // ap will show up to the same network it connects to via sta.
            .authmode = WIFI_AUTH_OPEN,
            .beacon_interval = 400,
            .max_connection = 16,
        }
    };

    ESP_LOGI(TAG, "Setting WiFi AP: SSID %s", wifi_config_ap.ap.ssid);
    ESP_LOGI(TAG, "Setting WiFi STA: SSID %s", wifi_config_sta.sta.ssid);


    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta));
    ESP_ERROR_CHECK(esp_wifi_start());
//    start_webserver();
}
