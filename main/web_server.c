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

#include <esp_http_server.h>
#include <esp_sntp.h>

#include "disk_system.h"

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

esp_err_t file_get_handler(httpd_req_t *req, char *filename)
{
    ESP_LOGI(TAG, "Reading file : %s", filename);

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        const char * resp_str = "File doesn't exist";
        httpd_resp_send(req, resp_str, strlen(resp_str));
        return ESP_OK;
    }

    /* Read file in chunks (relaxes any constraint due to large file sizes)
     * and send HTTP response in chunked encoding */
    char   chunk[1024];
    size_t chunksize;
    do {
        chunksize = fread(chunk, 1, sizeof(chunk), f);
	if (chunksize != 0) {
            if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK) {
                fclose(f);
                return ESP_FAIL;
            }
	}
    } while (chunksize != 0);

    fclose(f);
    // Adding this logging caused the code to start working.....
    ESP_LOGI(TAG, "done file : %s", filename);
    return ESP_OK;
}

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
    if (headFoot) file_get_handler(req, "/spiffs/header.html");
    file_get_handler(req, req->user_ctx);
    if (headFoot) file_get_handler(req, "/spiffs/footer.html");
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}


esp_err_t get_root_handler(httpd_req_t *req)
{
    /* Send a simple response */
    httpd_resp_set_hdr(req, "Content-type", "text/html");
    file_get_handler(req, "/spiffs/header.html");
    file_get_handler(req, "/spiffs/index.html");
    file_get_handler(req, "/spiffs/footer.html");
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

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

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    httpd_handle_t *server = (httpd_handle_t *) ctx;

    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        ESP_LOGI(TAG, "Got IP: %s",
                ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        /* Start the web server */
        if (*server == NULL) {
            *server = start_webserver();
        }

        ESP_LOGI(TAG, "Initializing SNTP");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
	sntp_init();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
        ESP_ERROR_CHECK(esp_wifi_connect());

        /* Stop the web server */
        if (*server) {
            stop_webserver(*server);
            *server = NULL;
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

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
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta));
    ESP_ERROR_CHECK(esp_wifi_start());
}
