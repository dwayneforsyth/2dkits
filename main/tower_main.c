


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_system.h"
#include "esp_http_server.h"
#include "esp_wifi.h"

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

//  The ESP32 framework has already started the Scheduler, starting it again just
//  causes a crash
}
